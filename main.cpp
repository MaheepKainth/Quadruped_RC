// receiver
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>

Servo FL, FR, RL, RR;

// Servo pins
const int FL_PIN = 17;
const int FR_PIN = 18;
const int RL_PIN = 25;
const int RR_PIN = 33;

// Thresholds
const int FWD_THR = 4000;
const int BACK_THR = 100;

// Servo motion parameters
const int CENTER = 90;
const int AMP = 30;
const int STEP_DELAY = 12;

// Reverse rear legs if needed
const bool REV_FL = false;
const bool REV_FR = false;
const bool REV_RL = true;
const bool REV_RR = true;

// Structure for received data
typedef struct {
  int xValue;
} JoystickData;

JoystickData receivedData;
volatile int joystickX = 2048;

// Callback for receiving data
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  joystickX = receivedData.xValue;
}

// Helper functions
void w(Servo &s, int a, bool r) {
  a = constrain(a, 0, 180);
  if (r) a = 180 - a;
  s.write(a);
}

void setLeft(int fl, int rl) {
  w(FL, fl, REV_FL);
  w(RL, rl, REV_RL);
}

void setRight(int fr, int rr) {
  w(FR, fr, REV_FR);
  w(RR, rr, REV_RR);
}

// LEFT SIDE MOVES
void leftStepForward() {
  setRight(CENTER, CENTER);
  for (int i = 0; i <= 20; i++) {
    float t = (float)i / 20;
    int fl = CENTER + (AMP * t);
    int rl = CENTER + (AMP * t);
    setLeft(fl, rl);
    delay(STEP_DELAY);
  }
  setLeft(CENTER, CENTER);
}

// RIGHT SIDE MOVES
void rightStepForward() {
  setLeft(CENTER, CENTER);
  for (int i = 0; i <= 20; i++) {
    float t = (float)i / 20;
    int fr = CENTER + (AMP * t);
    int rr = CENTER + (AMP * t);
    setRight(fr, rr);
    delay(STEP_DELAY);
  }
  setRight(CENTER, CENTER);
}

// BACKWARD MOVEMENT
void leftStepBack() {
  setRight(CENTER, CENTER);
  for (int i = 0; i <= 20; i++) {
    float t = (float)i / 20;
    int fl = CENTER - (AMP * t);
    int rl = CENTER - (AMP * t);
    setLeft(fl, rl);
    delay(STEP_DELAY);
  }
  setLeft(CENTER, CENTER);
}

void rightStepBack() {
  setLeft(CENTER, CENTER);
  for (int i = 0; i <= 20; i++) {
    float t = (float)i / 20;
    int fr = CENTER - (AMP * t);
    int rr = CENTER - (AMP * t);
    setRight(fr, rr);
    delay(STEP_DELAY);
  }
  setRight(CENTER, CENTER);
}

void setup() {
  Serial.begin(115200);

  // Initialize WiFi and ESP-NOW
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);

  // Initialize servos
  ESP32PWM::allocateTimer(0);
  FL.attach(FL_PIN, 500, 2400);
  FR.attach(FR_PIN, 500, 2400);
  RL.attach(RL_PIN, 500, 2400);
  RR.attach(RR_PIN, 500, 2400);

  setLeft(CENTER, CENTER);
  setRight(CENTER, CENTER);

  Serial.println("Receiver Ready");
}

void loop() {
  int x = joystickX;

  Serial.print("Received X: ");
  Serial.println(x);

  if (x > FWD_THR) {
    leftStepForward();
    rightStepForward();
  }
  else if (x < BACK_THR) {
    leftStepBack();
    rightStepBack();
  }
  else {
    setLeft(CENTER, CENTER);
    setRight(CENTER, CENTER);
    delay(20);
  }
}

// sender

// #include <Arduino.h>
// #include <WiFi.h>
// #include <esp_now.h>

// // Joystick pin
// const int JOY_X = 34;

// // Replace with the MAC address of the RECEIVER ESP32
// uint8_t receiverAddress[] = {0xa4, 0xf0, 0x0f, 0x61, 0xbc, 0x2c};

// // Structure to send data
// typedef struct {
//   int xValue;
// } JoystickData;

// JoystickData data;

// // Callback when data is sent
// void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   Serial.print("Send Status: ");
//   Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
// }

// void setup() {
//   Serial.begin(115200);

//   pinMode(JOY_X, INPUT);

//   // Set ESP32 to Station mode
//   WiFi.mode(WIFI_STA);
//   WiFi.disconnect();

//   // Initialize ESP-NOW
//   if (esp_now_init() != ESP_OK) {
//     Serial.println("ESP-NOW Init Failed");
//     return;
//   }

//   esp_now_register_send_cb(onDataSent);

//   // Register peer
//   esp_now_peer_info_t peerInfo = {};
//   memcpy(peerInfo.peer_addr, receiverAddress, 6);
//   peerInfo.channel = 0;
//   peerInfo.encrypt = false;

//   if (esp_now_add_peer(&peerInfo) != ESP_OK) {
//     Serial.println("Failed to add peer");
//     return;
//   }

//   Serial.println("Sender Ready");
// }

// void loop() {
//   data.xValue = analogRead(JOY_X);

//   esp_now_send(receiverAddress, (uint8_t *)&data, sizeof(data));

//   Serial.print("X Value Sent: ");
//   Serial.println(data.xValue);

//   delay(50);
// }

// #include <WiFi.h>

// void setup() {
//   Serial.begin(115200);
//   WiFi.mode(WIFI_STA);
//   Serial.print("Receiver MAC Address: ");
//   Serial.println(WiFi.macAddress());
// }

// void loop() {}