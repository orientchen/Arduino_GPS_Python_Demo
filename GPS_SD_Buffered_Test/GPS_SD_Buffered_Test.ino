
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

// GPS connections:
// GPS TX -> Arduino D8
// GPS RX -> Arduino D7
SoftwareSerial gpsSerial(8, 7);

const byte SD_CS = 10;
const byte BUFFER_SIZE = 100;

char sentence[BUFFER_SIZE];
byte sentenceLength = 0;

File logFile;

unsigned long validCount = 0;
unsigned long invalidCount = 0;
unsigned long overflowCount = 0;

unsigned long lastFlush = 0;
unsigned long lastReport = 0;

// Convert hexadecimal character to number
int hexValue(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}

// Check NMEA sentence checksum
bool validNMEA(const char *s) {
  if (s[0] != '$') return false;

  byte checksum = 0;
  int i = 1;

  while (s[i] != '\0' && s[i] != '*') {
    checksum ^= (byte)s[i];
    i++;
  }

  if (s[i] != '*') return false;

  char a = s[i + 1];
  char b = s[i + 2];

  if (a == '\0' || b == '\0') return false;
  if (s[i + 3] != '\0') return false;

  int hi = hexValue(a);
  int lo = hexValue(b);

  if (hi < 0 || lo < 0) return false;

  return checksum == ((hi << 4) | lo);
}

void setup() {
  Serial.begin(115200);

  Serial.println(F("=== Buffered GPS SD Test ==="));

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println(F("SD initialization failed!"));
    while (1);
  }

  Serial.println(F("SD initialization successful."));

  // Open GPS log file
  logFile = SD.open("BUFTEST.TXT", FILE_WRITE);

  if (!logFile) {
    Serial.println(F("Cannot open BUFTEST.TXT"));
    while (1);
  }

  Serial.println(F("BUFTEST.TXT opened successfully."));

  // Start GPS communication
  gpsSerial.begin(9600);

  Serial.println(F("GPS recording started."));
  Serial.println(F("SD flush interval: 10 seconds"));
}

void loop() {

  // Read incoming GPS characters
  while (gpsSerial.available()) {
    char c = gpsSerial.read();

    // Start of a new NMEA sentence
    if (c == '$') {
      sentenceLength = 0;
    }

    // End of NMEA sentence
    if (c == '\n') {

      if (sentenceLength > 0) {

        // Remove carriage return
        if (sentence[sentenceLength - 1] == '\r') {
          sentenceLength--;
        }

        sentence[sentenceLength] = '\0';

        // Validate checksum before saving
        if (validNMEA(sentence)) {
          validCount++;
          logFile.println(sentence);
        } else {
          invalidCount++;
        }
      }

      sentenceLength = 0;

    } else {

      // Store character in buffer
      if (sentenceLength < BUFFER_SIZE - 1) {
        sentence[sentenceLength++] = c;
      } else {
        overflowCount++;
        sentenceLength = 0;
      }
    }
  }

  // Flush SD card every 10 seconds
  if (millis() - lastFlush >= 10000) {
    logFile.flush();
    lastFlush = millis();
  }

  // Report statistics every 10 seconds
  if (millis() - lastReport >= 10000) {

    Serial.println(F("--- GPS Statistics ---"));

    Serial.print(F("Valid sentences: "));
    Serial.println(validCount);

    Serial.print(F("Invalid sentences: "));
    Serial.println(invalidCount);

    Serial.print(F("Buffer overflows: "));
    Serial.println(overflowCount);

    lastReport = millis();
  }
}
