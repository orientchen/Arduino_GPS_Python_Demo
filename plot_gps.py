"""Convert Arduino GPS NMEA RMC records to an interactive Folium map."""
from pathlib import Path
from datetime import datetime, timedelta
import folium

DATA = Path(__file__).with_name('BUFTEST.TXT')
OUTPUT = Path(__file__).with_name('gps_route.html')


def checksum_ok(line):
    if not line.startswith('$') or '*' not in line:
        return False
    body, check = line[1:].rsplit('*', 1)
    if len(check) != 2:
        return False
    value = 0
    for ch in body:
        value ^= ord(ch)
    try:
        return value == int(check, 16)
    except ValueError:
        return False


def decimal_degrees(raw, hemisphere):
    degrees = int(float(raw) // 100)
    minutes = float(raw) - degrees * 100
    result = degrees + minutes / 60
    return -result if hemisphere in ('S', 'W') else result


def read_points(path):
    points = []
    for line in path.read_text(encoding='utf-8', errors='replace').splitlines():
        line = line.strip()
        if not (line.startswith(('$GPRMC,', '$GNRMC,')) and checksum_ok(line)):
            continue
        fields = line.split('*')[0].split(',')
        if len(fields) < 10 or fields[2] != 'A':
            continue
        try:
            lat = decimal_degrees(fields[3], fields[4])
            lon = decimal_degrees(fields[5], fields[6])
            time = datetime.strptime(fields[9] + fields[1][:6], '%d%m%y%H%M%S')
            points.append((time, lat, lon))
        except (ValueError, IndexError):
            continue
    return points


def separate_sessions(points, gap_seconds=30):
    sessions = []
    for p in points:
        if not sessions or (p[0] - sessions[-1][-1][0]).total_seconds() > gap_seconds or p[0] < sessions[-1][-1][0]:
            sessions.append([p])
        else:
            sessions[-1].append(p)
    return sessions


def main():
    points = read_points(DATA)
    if not points:
        raise SystemExit('No valid GPS RMC coordinates found in BUFTEST.TXT')
    sessions = separate_sessions(points)
    m = folium.Map(
    location=[points[0][1], points[0][2]],
    zoom_start=18,
    tiles="https://server.arcgisonline.com/ArcGIS/rest/services/World_Street_Map/MapServer/tile/{z}/{y}/{x}",
    attr="Tiles © Esri"
)
    colors = ['blue', 'green', 'purple', 'orange', 'red']
    for idx, session in enumerate(sessions):
        color = colors[idx % len(colors)]
        coords = [(p[1], p[2]) for p in session]
        folium.PolyLine(coords, color=color, weight=3, opacity=0.8, tooltip=f'Session {idx + 1}').add_to(m)
        for time, lat, lon in session:
            folium.CircleMarker((lat, lon), radius=2, color=color, fill=True, fill_opacity=0.6,
                                popup=f'UTC: {time:%Y-%m-%d %H:%M:%S}<br>Latitude: {lat:.7f}<br>Longitude: {lon:.7f}').add_to(m)
        folium.Marker(coords[0], tooltip=f'Start session {idx+1}', icon=folium.Icon(color='green', icon='play')).add_to(m)
        folium.Marker(coords[-1], tooltip=f'End session {idx+1}', icon=folium.Icon(color='red', icon='stop')).add_to(m)
    m.fit_bounds([(p[1], p[2]) for p in points])
    m.save(str(OUTPUT))
    print(f'Loaded {len(points)} GPS positions in {len(sessions)} recording session(s).')
    print(f'Map saved to: {OUTPUT}')


if __name__ == '__main__':
    main()
