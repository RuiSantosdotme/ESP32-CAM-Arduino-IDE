/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-cam-qr-code-reader-web-server/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <ESP32QRCodeReader.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "FS.h"
#include "SD_MMC.h"
#include <time.h>
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// FOR THIS PROJECT, YOUR ESP32-CAM NEEDS TO HAVE PSRAM.
// Some of the compatible boards: CAMERA_MODEL_AI_THINKER | CAMERA_MODEL_WROVER_KIT | CAMERA_MODEL_ESP_EYE
// CAMERA_MODEL_M5STACK_PSRAM | CAMERA_MODEL_M5STACK_V2_PSRAM | CAMERA_MODEL_M5STACK_WIDE  
ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);

long timezone = 0;
byte daysavetime = 1;

const int ledPin = 4;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char* PARAM_INPUT_1 = "qrCode";
const char* PARAM_INPUT_2 = "role";
const char* PARAM_INPUT_3 = "delete";
const char* PARAM_INPUT_4 = "delete-user";

String inputMessage;
String inputParam;

void onQrCodeTask(void *pvParameters) {
  struct QRCodeData qrCodeData;

  while (true) {
    if (reader.receiveQrCode(&qrCodeData, 100)) {
      Serial.println("Scanned new QRCode");
      if (qrCodeData.valid) {
        String qrCodeString = String((const char *)qrCodeData.payload);
        Serial.print("Valid payload: ");
        Serial.println(qrCodeString);

        String role = getRoleFromFile("/users.txt", qrCodeString);
        if (role != "") {
          Serial.print("Role for QR Code: ");
          Serial.print(qrCodeString);
          Serial.print(" is ");
          Serial.println(role);
        } else {
          role = "unknown";
          Serial.print("QR Code: ");
          Serial.print(qrCodeString);
          Serial.println(" not found, set user role to unknown");
        }
        String sdMessage = qrCodeString + "," + role;
        appendFile(SD_MMC, "/log.txt", sdMessage.c_str());
        digitalWrite(ledPin, HIGH);
        delay(1000);
        digitalWrite(ledPin, LOW);
      }
      else {
        Serial.print("Invalid payload: ");
        Serial.println((const char *)qrCodeData.payload);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Write to the SD card
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }

  time_t t = file.getLastWrite();
  struct tm *tmstruct = localtime(&t);

  char bufferDate[50]; // Adjust buffer size as needed
  snprintf(bufferDate, sizeof(bufferDate), "%d-%02d-%02d", 
          (tmstruct->tm_year) + 1900, 
          (tmstruct->tm_mon) + 1, 
          tmstruct->tm_mday);
  char bufferTime[50]; // Adjust buffer size as needed
  snprintf(bufferTime, sizeof(bufferTime), "%02d:%02d:%02d", 
          tmstruct->tm_hour, 
          tmstruct->tm_min, 
          tmstruct->tm_sec);
          
  String lastWriteTime = bufferDate;
  String finalString = String(bufferDate) + "," + String(bufferTime) + "," + String(message) + "\n";
  Serial.println(lastWriteTime);
  if(file.print(finalString.c_str())) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

// Append data to the SD card
void appendUserFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }

  String finalString = String(message) + "\n";

  if(file.print(finalString.c_str())) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }

  // If the log.txt file doesn't exist, create a file on the SD card and write the header
  File file = SD_MMC.open("/log.txt");
  if(!file) {
    Serial.println("Creating new log.txt file...");
    writeFile(SD_MMC, "/log.txt", "Date,Time,QR_Code,Role\r\n");
  }
  else {
    Serial.println("log.txt file already exists");  
  }
  file.close();

  // If the users.txt file doesn't exist, create a file on the SD card and write the header
  file = SD_MMC.open("/users.txt");
  if(!file) {
    Serial.println("Creating new users.txt file...");
    writeFile(SD_MMC, "/users.txt", "QR_Code,Role\r\n");
  }
  else {
    Serial.println("users.txt file already exists");  
  }
  file.close();
}

String processor(const String& var){
  return String("HTTP GET request sent to your ESP on input field (" 
                + inputParam + ") with value: " + inputMessage +
                "<br><a href=\"/\"><button class=\"button button-home\">Return to Home Page</button></a>");
}

void deleteLineFromFile(const char* filename, int lineNumber) {
  File file = SD_MMC.open(filename);
  if (!file) {
    Serial.println("Failed to open file for reading.");
    return;
  }

  // Read all lines except the one to delete
  String lines = "";
  int currentLine = 0;
  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (currentLine != lineNumber) {
      lines += line + "\n";
    }
    currentLine++;
  }
  file.close();

  // Write back all lines except the deleted one
  file = SD_MMC.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing.");
    return;
  }

  file.print(lines);
  file.close();
  Serial.println("Line deleted successfully.");
}

String getRoleFromFile(const char* filename, String qrCode) {
  File file = SD_MMC.open(filename);
  if (!file) {
    Serial.println("Failed to open file for reading.");
    return "";
  }

  // Skip the header line
  file.readStringUntil('\n');
  
  // Read each line and check for QR Code
  while (file.available()) {
    String line = file.readStringUntil('\n');
    
    int commaIndex = line.indexOf(',');
    if (commaIndex > 0) {
      String fileQrCode = line.substring(0, commaIndex);
      String role = line.substring(commaIndex + 1);

      // Compare qrCode
      if (fileQrCode == qrCode) {
        file.close();
        role.trim();  // Remove any extra spaces or newline characters
        return role;
      }
    }
  }
  file.close();
  return "";  // Return empty string if qrCode not found
}

void initLittleFS() {
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
        return;
  }
}

void initWifi() {
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  int connectAttempt = 0;
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    connectAttempt ++;
    if (connectAttempt == 10){ 
      ESP.restart();
    }
  }
  // Print ESP32 Local IP Address
  Serial.print("\nESP IP Address: ");
  Serial.println(WiFi.localIP());
}

void initTime() {
  Serial.println("Initializing Time");
  struct tm tmstruct;
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct);
  Serial.printf(
    "Time and Date right now is : %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct.tm_year) + 1900, (tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min,
    tmstruct.tm_sec
  );
}

void initSDCard() {
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // If the log.txt file doesn't exist, create a file on the SD card and write the header
  File file = SD_MMC.open("/log.txt");
  if(!file) {
    Serial.println("log.txt file doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD_MMC, "/log.txt", "Date,Time,QR_Code,Role\r\n");
  }
  else {
    Serial.println("log.txt file already exists");  
  }
  file.close();

  // If the users.txt file doesn't exist, create a file on the SD card and write the header
  file = SD_MMC.open("/users.txt");
  if(!file) {
    Serial.println("users.txt file doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD_MMC, "/users.txt", "QR_Code,Role\r\n");
  }
  else {
    Serial.println("users.txt file already exists");  
  }
  file.close();
}

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  while (!Serial);       // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).

  reader.setup();
  Serial.println("\nSetup QRCode Reader");
  reader.beginOnCore(1);
  Serial.println("Begin on Core 1");
  xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, NULL);

  initWifi();
  initLittleFS();
  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  initTime();
  initSDCard();

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/full-log.html");
  });
  // Route for root /add-user web page
  server.on("/add-user", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/add-user.html");
  });
  // Route for root /manage-users web page
  server.on("/manage-users", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/manage-users.html");
  });

  // Serve Static files
  server.serveStatic("/", LittleFS, "/");

  // Loads the log.txt file
  server.on("/view-log", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD_MMC, "/log.txt", "text/plain", false);
  });
  // Loads the users.txt file
  server.on("/view-users", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD_MMC, "/users.txt", "text/plain", false);
  });
  
  // Receive HTTP GET requests on <ESP_IP>/get?input=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET input1 and input2 value on <ESP_IP>/get?input1=<inputMessage1>&input2=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = String(PARAM_INPUT_1);
      inputMessage += " " + request->getParam(PARAM_INPUT_2)->value();
      inputParam += " " + String(PARAM_INPUT_2);

      String finalMessageInput = String(request->getParam(PARAM_INPUT_1)->value()) + "," + String(request->getParam(PARAM_INPUT_2)->value());
      appendUserFile(SD_MMC, "/users.txt", finalMessageInput.c_str());
    }
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = String(PARAM_INPUT_3);
      if(request->getParam(PARAM_INPUT_3)->value()=="users") {
        deleteFile(SD_MMC, "/users.txt");
      }
      else if(request->getParam(PARAM_INPUT_3)->value()=="log") {
        deleteFile(SD_MMC, "/log.txt");
      }
    }
    else if (request->hasParam(PARAM_INPUT_4)) {
      inputMessage = request->getParam(PARAM_INPUT_4)->value();
      inputParam = String(PARAM_INPUT_4);
      deleteLineFromFile("/users.txt", inputMessage.toInt());
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    request->send(LittleFS, "/get.html", "text/html", false, processor);
  });
  // Start server
  server.begin();
}

void loop() {

}