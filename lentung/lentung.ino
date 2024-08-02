#include <ESP8266WiFi.h>
#include <Wire.h>
#include <U8g2lib.h>

// Konfigurasi OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Konfigurasi WiFi
const char* ssid = "Pakdiyaa2";
const char* password = "belikodata";

// URL endpoint server
const char* serverIP = "192.168.7.130";
const int serverPort = 5000;
const String baseURL = "http://" + String(serverIP) + ":" + String(serverPort);

// Pin tombol
const int buttonLeftPin = 12;
const int buttonRightPin = 14;
const int buttonTogglePowerpointPin = 13;

// Variabel untuk menyimpan status tombol
bool buttonLeftState = false;
bool buttonRightState = false;
bool buttonTogglePowerpointState = false;

void setup() {
  Serial.begin(115200);

  // Setup OLED
  Wire.begin();  // Inisialisasi koneksi I2C
  u8g2.begin();

  // Setup WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Update OLED display with animated dots
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(0, 20);
    u8g2.print("Connecting to WiFi");
    u8g2.print(".");
    u8g2.sendBuffer();
    delay(500);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(0, 20);
    u8g2.print("Connecting to WiFi");
    u8g2.sendBuffer();
    delay(500);
  }
  u8g2.clearBuffer();
  u8g2.setCursor(0, 20);
  u8g2.print("Connected to WiFi");
  u8g2.sendBuffer();
  
  // Setup tombol
  pinMode(buttonLeftPin, INPUT_PULLUP);
  pinMode(buttonRightPin, INPUT_PULLUP);
  pinMode(buttonTogglePowerpointPin, INPUT_PULLUP);
}

// Mengirim request ke server
void sendRequest(String endpoint) {
  WiFiClient client;
  if (!client.connect(serverIP, serverPort)) {
    Serial.println("Connection failed");
    return;
  }
  
  client.print(String("POST ") + endpoint + " HTTP/1.1\r\n" +
               "Host: " + serverIP + "\r\n" +
               "Connection: close\r\n" +
               "\r\n");

  // Tunggu response
  String response = "";
  while(client.connected() || client.available()) {
    if (client.available()) {
      response += client.readString();
    }
  }
  
  // Tampilkan request dan response di OLED
  u8g2.clearBuffer();
  u8g2.setCursor(0, 10);
  u8g2.print("Request:");
  u8g2.setCursor(0, 20);
  u8g2.print(endpoint);
  u8g2.setCursor(0, 30);
  u8g2.print("Response:");
  u8g2.setCursor(0, 40);
  u8g2.print(response.substring(0, 64)); // Batasi panjang respons untuk menyesuaikan dengan layar
  u8g2.sendBuffer();
  delay(2000);  // Tampilkan selama 2 detik
}

void loop() {
  // Membaca status tombol
  buttonLeftState = digitalRead(buttonLeftPin) == LOW;
  buttonRightState = digitalRead(buttonRightPin) == LOW;
  buttonTogglePowerpointState = digitalRead(buttonTogglePowerpointPin) == LOW;
  
  // Kirim request berdasarkan tombol yang ditekan
  if (buttonLeftState) {
    sendRequest("/press_left");
    // delay(500);  // Debounce
  }
  if (buttonRightState) {
    sendRequest("/press_right");
    // delay(500);  // Debounce
  }
  if (buttonTogglePowerpointState) {
    sendRequest("/toggle_powerpoint");
    // delay(500);  // Debounce
  }
}