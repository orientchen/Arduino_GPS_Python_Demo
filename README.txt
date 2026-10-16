Arduino GPS Python Map Demo
===========================
1. Install Python 3.
2. Open a terminal in this folder.
3. Run: pip install folium
4. Run: python plot_gps.py
5. Open gps_route.html in Chrome.

For future tests, replace BUFTEST.TXT with the data copied from the Arduino
Serial Monitor. Keep the NMEA lines beginning with $GPRMC (other lines are fine).
The script checks NMEA checksums and separates sessions at timestamp gaps
longer than 30 seconds or when timestamps move backward.

OpenStreetMap tiles need an internet connection.
