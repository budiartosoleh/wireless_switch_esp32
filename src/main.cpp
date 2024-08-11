#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include "wifi_handler.h"

// Definisikan pin untuk servo
int servoPin = 18;
Servo myServo;

// Definisikan variabel untuk pengaturan servo
int targetAngle = 90;
int initialAngle = 0;
int servoDelay = 1000;

// Definisikan server web
AsyncWebServer server(80);

// Definisikan OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // Inisialisasi layar OLED
  if (!display.begin(0x3C, OLED_RESET)) { // Ganti dengan alamat I2C yang benar (0x3C atau 0x3D)
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);            // Posisi awal teks

  // Tampilkan pesan "Hello, World!"
  display.println(F("Hello, World!"));

  // Inisialisasi servo
  myServo.attach(servoPin);
  myServo.write(initialAngle);  // Pastikan servo bergerak ke posisi 0 derajat saat start

  // Koneksi ke Wi-Fi
  connect_wifi();

  // Tampilkan alamat IP di serial monitor dan OLED
    // Tampilkan alamat IP di serial monitor dan OLED  
  while (WiFi.status() != WL_CONNECTED) {  
    delay(1000);  
    Serial.print(".");  
  }  

  // Koneksi berhasil  
  Serial.println("");  
  Serial.println("Connected to Wi-Fi");  
  Serial.print("IP Address: ");  
  Serial.println(WiFi.localIP());  

  // Tampilkan di layar OLED  
  display.clearDisplay();  
  display.setCursor(0, 0);  
  display.print("Connected to Wi-Fi");  
  display.setCursor(0, 10);  
  display.print("IP Address:");  
  display.setCursor(0, 20);  
  display.print(WiFi.localIP().toString());  // Pastikan menampilkan alamat IP sebagai String  
  display.display();  

  // Tambahan: Periksa apakah ada tampilan yang lebih baik  
  delay(2000); // Delay untuk melihat hasilnya sebelum halaman dimuat

  // Halaman utama
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head>";
    html += "<style>";
    html += "body {font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; margin-top: 50px;}";
    html += "h2 {color: #4CAF50; font-size: 18px;}";
    html += ".container {display: inline-block; text-align: left; background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);}";
    html += ".setting-button {position: absolute; top: 20px; left: 20px; background-color: #4CAF50; color: white; border: none; padding: 10px 15px; font-size: 16px; cursor: pointer; border-radius: 5px;}";
    html += ".switch {position: relative; display: inline-block; width: 60px; height: 34px;}";
    html += ".switch input {opacity: 0; width: 0; height: 0;}";
    html += ".slider {position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s;}";
    html += ".slider:before {position: absolute; content: ''; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; transition: .4s;}";
    html += "input:checked + .slider {background-color: #4CAF50;}";
    html += "input:checked + .slider:before {transform: translateX(26px);}";
    html += ".slider.round {border-radius: 34px;}";
    html += ".slider.round:before {border-radius: 50%;}";
    html += "label {font-size: 18px;}";
    html += "</style>";
    html += "</head><body>";
    html += "<button class='setting-button' onclick=\"window.location.href='/settings';\">Settings</button>";
    html += "<div class='container'>";
    html += "<h2>ESP32 Wireless Button</h2>";
    html += "<label>Mode: Servo</label><br><br>";
    html += "<label>Set Degree: " + String(targetAngle) + "°</label><br><br>";
    html += "<label>Delay: " + String(servoDelay) + " ms</label><br><br>";
    html += "<label>Servo</label><label class='switch'><input type='checkbox' onclick=\"window.location.href='/on';\"><span class='slider round'></span></label><br>";
    html += "</div>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Halaman pengaturan
  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head>";
    html += "<style>";
    html += "body {font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; margin-top: 50px;}";
    html += "h2 {color: #4CAF50; font-size: 18px;}";
    html += ".container {display: inline-block; text-align: left; background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);}";
    html += "label {font-size: 18px;}";
    html += "input[type='number'], select {font-size: 18px; padding: 5px; width: calc(100% - 12px); margin-top: 10px; margin-bottom: 20px; border-radius: 5px; border: 1px solid #ccc;}";
    html += "button {background-color: #4CAF50; color: white; border: none; padding: 10px 15px; font-size: 18px; cursor: pointer; border-radius: 5px;}";
    html += "</style>";
    html += "</head><body>";
    html += "<div class='container'>";
    html += "<h2>Setting Page</h2>";
    html += "<form action='/save' method='get'>";
    html += "<label>Set Degree: </label><input type='number' name='targetAngle' value='" + String(targetAngle) + "'><br>";
    html += "<label>Delay: </label><input type='number' name='servoDelay' value='" + String(servoDelay) + "'><br>";
    html += "<button type='submit'>Save</button>";
    html += "</form>";
    html += "<br><button onclick=\"window.location.href='/';\">Home</button>";
    html += "</div>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Menyimpan pengaturan
  server.on("/save", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("targetAngle")) {
      targetAngle = request->getParam("targetAngle")->value().toInt();
    }
    if (request->hasParam("servoDelay")) {
      servoDelay = request->getParam("servoDelay")->value().toInt();
    }

    Serial.print("Saved settings - Target Angle: ");
    Serial.print(targetAngle);
    Serial.print(", Delay: ");
    Serial.println(servoDelay);

    request->send(200, "text/html", "<html><body><h1>Settings Saved</h1><br><a href='/'>Back to Home</a></body></html>");
  });

  // Halaman untuk menggerakkan servo
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Servo mulai bergerak");

    myServo.write(targetAngle);  // Menggerakkan servo ke targetAngle
    delay(servoDelay);           // Menunggu dengan delay yang telah ditentukan

    myServo.write(initialAngle); // Kembali ke 0 derajat

    Serial.println("Servo kembali ke posisi awal");

    String html = "<html><head>";
    html += "<style>";
    html += "body {font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; margin-top: 50px;}";
    html += "h2 {color: #4CAF50; font-size: 18px;}";
    html += "</style>";
    html += "<script>setTimeout(function(){ window.location.href = '/'; }, 5000);</script>";
    html += "</head><body>";
    html += "<h2>Servo moved to " + String(targetAngle) + " degrees and returned to 0.</h2>";
    html += "<p>Returning to the main page in 5 seconds...</p>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Memulai server
  server.begin();
}

void loop() {
  // Kosongkan loop jika tidak ada logika tambahan yang perlu dijalankan secara berulang
}
