# Arduino GPS Logger and Python Route Mapping

This project uses an **Arduino Uno R3**, an **Adafruit Ultimate GPS Logging Shield**, and an **SD card** to record GPS data. A Python script (`plot_gps.py`) reads the recorded NMEA sentences and creates an interactive route map (`gps_route.html`) using Folium.

## 1. Hardware and software

- Arduino Uno R3
- Adafruit Ultimate GPS Logging Shield with an SD card
- USB cable for uploading the Arduino sketch
- Portable power source (currently a rectangular 9V battery with a DC barrel connector)
- USB SD card reader
- Arduino IDE, Python 3, and the Python package `folium`
- An internet connection to load the background map tiles

**GPS shield settings used by this project:**

| Setting | Value |
| --- | --- |
| GPS shield switch | **Soft Serial** |
| GPS TX → Arduino RX | Digital pin **D8** |
| GPS RX ← Arduino TX | Digital pin **D7** |
| SD card chip select | Digital pin **D10** |
| GPS serial speed | **9600 baud** |
| Arduino Serial Monitor speed | **115200 baud** |

## 2. Repository files

```text
Arduino_GPS_Python_Demo/
├── GPS_SD_Buffered_Test/
│   └── GPS_SD_Buffered_Test.ino   # Arduino GPS-to-SD logger
├── plot_gps.py                    # NMEA parser and route mapper
├── README.md                      # These instructions
├── .gitignore                     # Excludes personal GPS logs and maps
└── .gitattributes
```

The following files are **generated locally** and should not be committed to a public repository:

- `BUFTEST.TXT` — raw GPS recording, including precise coordinates
- `gps_route.html` — interactive map, which also contains coordinates

The existing Arduino sketch appends to `BUFTEST.TXT` and flushes the SD card approximately every **10 seconds**. The Python script reads this file and generates `gps_route.html`.

## 3. Upload the Arduino program (one-time setup)

The tested sketch is already uploaded to the Arduino. **You do not need to upload it again for each walk.** If setting up another board:

1. Connect the Arduino Uno to your computer using USB.
2. Open `GPS_SD_Buffered_Test/GPS_SD_Buffered_Test.ino` in the Arduino IDE.
3. Select the correct **Arduino Uno** board and COM port.
4. Check that the GPS shield switch is set to **Soft Serial** and the SD card is inserted.
5. Upload the sketch.
6. Optionally open the Serial Monitor at **115200 baud** to confirm SD initialization and GPS recording have started.

The logger runs automatically whenever the Arduino receives power.

## 4. Prepare the SD card before a new recording

1. Insert the SD card into the USB card reader.
2. Back up any existing `BUFTEST.TXT` to your computer if you want to preserve earlier recordings.
3. For a clean, single-session test, delete **only** `BUFTEST.TXT` from the SD card after backing it up. The logger will create a new file automatically.
4. Safely eject the SD card and insert it into the GPS shield.

**Important:** The Arduino Uno does not have a real-time clock. Windows may show an SD file creation or modification date of **January 1, 2000**. This is normal; the GPS NMEA data contains its own UTC date and time once a valid satellite fix is obtained.

## 5. Acquire a GPS fix outdoors

1. On a dry day, connect the portable battery to the Arduino. You may turn it on indoors shortly before going outside.
2. Carry the board to an open area with a clear view of the sky. Keep the GPS antenna facing upward and avoid covering it.
3. Observe the **FIX** LED on the GPS shield:
   - **About one flash per second:** searching for a valid GPS position.
   - **About one flash every 15 seconds:** a valid GPS fix has been acquired.
4. If the LED is flashing rapidly, wait outdoors for a fix; allow approximately **5–10 minutes** if necessary.
5. Once the fix is stable, begin walking. The Arduino has already started writing data automatically.

A *GPS fix* means the receiver has determined a valid position from satellite signals. Buildings, indoor conditions, and obstructions can interrupt reception. A valid `$GPRMC` NMEA sentence has status `A`; `V` indicates invalid position data.

## 6. Record a walking route

1. Walk in the open area for about **5–10 minutes**.
2. Keep the antenna facing upward as much as possible.
3. Leave the board powered throughout the walk; do not disconnect the battery mid-route.
4. At the end, stop and leave the Arduino powered for **at least 15 seconds** to allow a scheduled SD flush to complete.
5. Disconnect the battery **before** removing the SD card.

**Power note:** A small rectangular 9V battery has limited capacity. It may be adequate for a short test, but a suitable higher-capacity power source is preferable for longer walks. Keep the exposed electronics dry.

## 7. Copy the recording to the computer

1. Remove the SD card after powering off the Arduino.
2. Insert the card into the USB SD card reader.
3. Open the SD card in Windows File Explorer.
4. Copy `BUFTEST.TXT` into your local project folder:

   ```text
   C:\Users\chenj\Documents\GitHub\Arduino_GPS_Python_Demo
   ```

5. Replace the previous local `BUFTEST.TXT` if you have already saved a backup of it.
6. Safely eject the SD card.

The file contains NMEA sentences such as `$GPGGA`, `$GPRMC`, and `$PGTOP`. **Do not manually delete other NMEA sentence types**; the Python script selects the records it needs.

## 8. Generate the interactive map with Python

Open a terminal in the project folder. If Folium has not yet been installed, run:

```powershell
& C:\Python314\python.exe -m pip install folium
```

Run the mapping program:

```powershell
& C:\Python314\python.exe plot_gps.py
```

Alternatively, if `python` already points to the correct Python installation:

```powershell
python plot_gps.py
```

The script reads `BUFTEST.TXT`, checks NMEA checksums, uses valid `$GPRMC` GPS positions, and separates recording sessions when timestamps jump backward or gaps exceed **30 seconds**. It saves the result as:

```text
gps_route.html
```

Open `gps_route.html` in **Microsoft Edge** (or another modern browser). If the map was already open, refresh with **Ctrl+F5** after generating a new version.

The tested mapping configuration uses **Esri World Street Map** background tiles. An internet connection is required to display the map background.

## 9. Troubleshooting

| Problem | What to check |
| --- | --- |
| FIX LED flashes about once per second | Move into an open area, point the antenna upward, and wait for a satellite fix. |
| NMEA records contain `$GPRMC,...,V,...` | GPS position is invalid; the receiver has not obtained or has lost its fix. |
| No `BUFTEST.TXT` appears | Check SD card insertion, SD initialization, power, and the sketch's Serial Monitor messages. |
| Map has no route | Confirm `BUFTEST.TXT` is in the same folder as `plot_gps.py` and contains valid `$GPRMC` positions. |
| Map background is blank | Check the internet connection and the configured Esri tile URL. |
| File date shows January 1, 2000 | Expected without an Arduino real-time clock; use GPS NMEA timestamps. |
| The map shows multiple recordings | `BUFTEST.TXT` is appended across runs; back up and clear the SD copy before the next test. |
| Arduino stops during a walk | Check the battery and power connector; a rectangular 9V battery may not last long. |

## 10. Privacy and GitHub

GPS logs and generated HTML maps can reveal exact places visited. Keep them on your computer unless you intentionally choose to share them. The project's `.gitignore` should exclude at least:

```gitignore
BUFTEST.TXT
GPSLOG*.TXT
TESTGPS.TXT
gps_route.html
__pycache__/
*.pyc
```

Before publishing or committing changes, review the GitHub Desktop **Changes** list to make sure no personal GPS recordings or generated maps are included. A `.gitignore` rule does not automatically untrack a file that was previously committed.

## 11. Quick checklist for future walks

1. Back up and clear the old SD-card `BUFTEST.TXT` (optional but recommended).
2. Insert the SD card into the GPS shield.
3. Connect the battery and move into an open area.
4. Wait for the FIX LED to flash approximately once every 15 seconds.
5. Walk for 5–10 minutes with the board powered.
6. Wait at least 15 seconds after stopping, then disconnect the battery.
7. Copy `BUFTEST.TXT` from the SD card into the project folder.
8. Run `python plot_gps.py` (or the full `C:\Python314\python.exe` command above).
9. Open `gps_route.html` to view the recorded route.

**No Arduino or Python code changes are needed for the next outdoor test.**
