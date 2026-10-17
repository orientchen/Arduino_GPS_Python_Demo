# Arduino GPS Logger and Python Route Mapping

Record GPS coordinates using an **Arduino Uno R3** and **Adafruit Ultimate GPS Logging Shield**, then display the route on an interactive map with **Python and Folium**. The Python mapping script can run locally or in **GitHub Codespaces**, so no Python installation is required on a MacBook.

## Hardware

- Arduino Uno R3
- Adafruit Ultimate GPS Logging Shield with antenna
- microSD card in the shield and USB SD card reader
- Battery or reliable USB power supply for the Arduino

The GPS shield is set to **Soft Serial**. The existing Arduino sketch uses GPS SoftwareSerial pins **D8 (RX)** and **D7 (TX)**, with SD card chip-select **D10**.

## Repository files

```text
Arduino_GPS_Python_Demo/
├── .devcontainer/
│   └── devcontainer.json
├── GPS_SD_Buffered_Test/
│   └── GPS_SD_Buffered_Test.ino
├── plot_gps.py
├── requirements.txt
├── README.md
└── .gitignore
```

The generated `gps_route.html` and raw `BUFTEST.TXT` should remain untracked, as configured in `.gitignore`.

## 1. Prepare the GPS logger

1. Upload `GPS_SD_Buffered_Test/GPS_SD_Buffered_Test.ino` using the Arduino IDE, if it is not already installed. The sketch remains on the board after power is disconnected.
2. Set the GPS shield switch to **Soft Serial**.
3. Insert the SD card. The sketch creates/appends to `BUFTEST.TXT` and flushes recorded data about every **10 seconds**.
4. For a fresh recording, first back up `BUFTEST.TXT` from the SD card and then delete the SD-card copy. The Arduino will create a new file.
5. Connect the battery/power supply. You do not need a computer connected while logging.

**GPS FIX LED:** approximately one flash per second means the GPS is still acquiring a fix; approximately one flash every 15 seconds indicates a valid GPS fix on this Adafruit module. Allow a few minutes in an open area before starting the trip. The GPS antenna needs a clear view of the sky.

## 2. Record a driving route

1. Before driving, place the Arduino/GPS assembly securely in the car, with the antenna facing upward near a window or windshield and away from obstructions.
2. Make sure the assembly, wires, and battery cannot move, obstruct your view, or interfere with vehicle controls or airbags. Do not operate the equipment while driving.
3. Connect power **before** departure and allow the receiver to acquire a GPS fix. If needed, wait while parked in an open area.
4. Drive normally. The Arduino records GPS NMEA sentences to the SD card automatically.
5. After parking, wait at least **15 seconds** while the Arduino is still powered, then disconnect power.
6. Remove the SD card only after the Arduino is powered off.

A small rectangular 9V battery has limited runtime and may not last through a long drive. Use a fresh battery or a suitable stable power source. Protect the electronics from moisture and excessive heat.

## 3. Copy the SD card file to your MacBook

1. Insert the SD card into a compatible reader connected to the MacBook.
2. Open Finder and locate `BUFTEST.TXT` on the SD card.
3. Copy it to a convenient folder on the MacBook, preserving the original as a backup.
4. Safely eject the SD card.

The SD card may show a file date around **January 1, 2000**, because the Arduino does not maintain normal filesystem timestamps. Actual GPS date/time is contained in the NMEA data when a valid GPS fix is available.

## 4. Open the repository in GitHub Codespaces (MacBook browser)

1. Open the private repository: https://github.com/orientchen/Arduino_GPS_Python_Demo
2. Click **Code** → **Codespaces** → **Create codespace on main**. For economical use, select a small machine (such as 2 cores) if a machine-size choice is offered.
3. Wait for the Codespace to open in the browser. The `.devcontainer/devcontainer.json` configuration installs Python and the Folium dependency listed in `requirements.txt`.
4. Open **Terminal** → **New Terminal** and check:

   ```bash
   python -c "import folium; print(folium.__version__)"
   ```

If Folium is missing, run `python -m pip install -r requirements.txt` in the Codespace terminal.

## 5. Upload GPS data to the Codespace

**Recommended privacy-preserving method:** Upload the data directly into the Codespace workspace, without committing the GPS data to GitHub history.

1. In the Codespace's VS Code Explorer, right-click the repository root folder and choose **Upload...** (or drag `BUFTEST.TXT` from Finder into the Explorer).
2. Place `BUFTEST.TXT` in the **same folder as `plot_gps.py`**. The existing plotting script expects that filename.
3. Confirm the file appears in Explorer.
4. Do **not** force-add `BUFTEST.TXT` to Git; the `.gitignore` intentionally excludes it. Remember that a Codespace is cloud-hosted and its uncommitted files can be lost when the Codespace is deleted, so retain your local SD-card backup.

If you explicitly want to commit your driving coordinates to the private repository, you would need to change the ignore rules first. That is not recommended for routes containing home/work locations.

## 6. Generate and view the map

In the Codespace terminal, from the repository root, run:

```bash
python plot_gps.py
```

The existing Python script reads valid `$GPRMC` coordinates, checks NMEA checksums, separates recording sessions across large time gaps, and generates `gps_route.html` using Folium and Esri World Street Map tiles.

To view the map in Codespaces:

1. Find `gps_route.html` in the VS Code Explorer.
2. Right-click it and select **Download** to save the HTML to your MacBook.
3. Open the downloaded HTML file in Chrome, Safari, or Edge while connected to the internet (map tiles are fetched online).

The HTML output is ignored by Git because it can also reveal your route.

## 7. Running locally on Windows (optional)

From the repository folder:

```powershell
& C:\Python314\python.exe -m pip install -r requirements.txt
& C:\Python314\python.exe plot_gps.py
```

Open `gps_route.html` in a browser.

## 8. Troubleshooting

- **No visible route:** Ensure `BUFTEST.TXT` is in the repository root next to `plot_gps.py`, and the file includes valid `$GPRMC` records with status `A`. A `V` status means no valid position fix.
- **FIX LED flashes about once a second:** Move the GPS antenna to an open area and wait for satellite acquisition.
- **Map background missing:** The browser needs internet access to load Esri tiles.
- **No Folium module:** Run `python -m pip install -r requirements.txt` inside Codespaces.
- **Old and new trips mixed together:** The Arduino sketch appends to `BUFTEST.TXT`. Back up and remove the old SD-card file before starting a new trip if you want a clean recording.
- **Map not updated:** Run `python plot_gps.py` again after replacing the input file, then download/open the newly generated HTML.
- **Codespaces usage:** Stop the Codespace when finished to conserve included compute hours. Deleting a Codespace removes files stored only inside that Codespace.

## Data privacy

`BUFTEST.TXT` and `gps_route.html` can disclose precise travel routes, including home and work locations. The provided `.gitignore` excludes both from normal commits. Keep the repository private and avoid uploading sensitive routes to public repositories.
