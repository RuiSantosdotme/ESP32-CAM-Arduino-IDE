/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-qr-code-reader-scanner-arduino/
*********/

#include <Arduino.h>
#include <ESP32QRCodeReader.h>

// FOR THIS PROJECT, YOUR ESP32-CAM NEEDS TO HAVE PSRAM.
// Some of the compatible boards: CAMERA_MODEL_AI_THINKER | CAMERA_MODEL_WROVER_KIT | CAMERA_MODEL_ESP_EYE
// CAMERA_MODEL_M5STACK_PSRAM | CAMERA_MODEL_M5STACK_V2_PSRAM | CAMERA_MODEL_M5STACK_WIDE  
ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);

void onQrCodeTask(void *pvParameters) {
  struct QRCodeData qrCodeData;

  while (true) {
    if (reader.receiveQrCode(&qrCodeData, 100)) {
      Serial.println("Scanned new QRCode");
      if (qrCodeData.valid) {
        Serial.print("Valid payload: ");
        Serial.println((const char *)qrCodeData.payload);
      }
      else {
        Serial.print("Invalid payload: ");
        Serial.println((const char *)qrCodeData.payload);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  reader.setup();
  Serial.println("Setup QRCode Reader");

  reader.beginOnCore(1);
  Serial.println("Begin on Core 1");

  xTaskCreate(onQrCodeTask, "onQrCode", 4 * 1024, NULL, 4, NULL);
}

void loop() {
  delay(100);
}