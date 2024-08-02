#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <U8g2lib.h>

using namespace websockets;

// Konfigurasi WiFi
const char* ssid = "Pakdiyaa2";       // Ganti dengan SSID WiFi Anda
const char* password = "belikodata";  // Ganti dengan password WiFi Anda
const char* websockets_server_host = "192.168.7.130"; // Ganti dengan IP server WebSocket
const uint16_t websockets_server_port = 8765; // Port server WebSocket

// Konfigurasi OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Pin tombol
const short buttonLeftPin = 14;
const short buttonRightPin = 12;
const short buttonTogglePowerpointPin = 13;

// WebSocket client
WebsocketsClient client;

// Variabel status
bool isConnected = false;
short counter = 0;
String status_text = "Hi Lentunger";

void sendMessage(const String& message) {
    // Kirim pesan ke server WebSocket
    client.send(message);

    // Atur callback untuk menangani pesan yang diterima
    client.onMessage([&](WebsocketsMessage receivedMessage) {
        Serial.print("Received Message: ");
        Serial.println(receivedMessage.data());

        // Cek apakah pesan mengandung kunci 'error'
        if (receivedMessage.data().indexOf("\"error\":") >= 0) {
            handleError(receivedMessage.data());
        } else {
            handleMessage(receivedMessage.data());
        }
    });
}

void handleMessage(const String& message) {
    int messageStartIndex = message.indexOf("'message': '") + 12;
    int messageEndIndex = message.indexOf("'", messageStartIndex);

    if (messageStartIndex > 0 && messageEndIndex > messageStartIndex) {
        status_text = message.substring(messageStartIndex, messageEndIndex);
        Serial.print("Message: ");
        Serial.println(status_text);
    } else {
        status_text = "Firmware Err";
        Serial.println("[Firmware Err] => Message not found.");
    }
}

void handleError(const String& message) {
    // Temukan posisi kunci 'error' dan tanda kutip penutup
    int errorStartIndex = message.indexOf("\"error\": \"") + 10;  // 10 adalah panjang dari "\"error\": \""
    int errorEndIndex = message.indexOf("\"", errorStartIndex);

    // Ambil nilai dari kunci 'error'
    if (errorStartIndex > 0 && errorEndIndex > errorStartIndex) {
        String errorValue = message.substring(errorStartIndex, errorEndIndex);
        Serial.print("[Firmware Err] => ");
        Serial.println(errorValue);
        status_text = "Firmware Err";
        Serial.println("[Firmware Err] => Invalid error format.");
    } else {
        status_text = "Firmware Err";
        Serial.println("[Firmware Err] => Invalid error format.");
    }
}

void setup() {
    Serial.begin(115200);

    // Setup OLED
    Wire.begin();  // Inisialisasi koneksi I2C
    u8g2.begin();
    
    // Setup tombol
    pinMode(buttonLeftPin, INPUT_PULLUP);
    pinMode(buttonRightPin, INPUT_PULLUP);
    pinMode(buttonTogglePowerpointPin, INPUT_PULLUP);

    // Connect to WiFi
    displayAnimation("Conecting");
    
    WiFi.begin(ssid, password);

    // Wait for WiFi connection
    for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        delay(1000);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to WiFi, Connecting to WebSocket server.");

        // Connect to WebSocket server
        bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
        if (connected) {
            isConnected = true;
            displayAnimation("Hi Lentunger");
        } else {
            displayAnimation("Cant Conect");
            Serial.println("Connection failed!");
        }
        
        // Callback when messages are received
    } else {
        displayAnimation("Cant Conect");
        Serial.println("No Wifi!");
    }
}

void loop() {
    // Handle WebSocket messages
    if (client.available()) {
        client.poll();
    }

    // Check button states and send WebSocket messages
    if (digitalRead(buttonLeftPin) == LOW) {
        sendMessage("left");
        delay(100);  // Debounce
    }
    if (digitalRead(buttonRightPin) == LOW) {
        sendMessage("right");
        delay(100);  // Debounce
    }
    if (digitalRead(buttonTogglePowerpointPin) == LOW) {
        sendMessage("swipe");
        delay(100);  // Debounce
    }

    displayAnimation(status_text);
}
