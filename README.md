# ESP32 ESP-NOW Quadruped Control 

This file contains **three separate programs**: a Receiver, a Sender, and a MAC Address Utility. Since they are stored together, **only one should be active when flashing**. Comment out the others before uploading.

---

## 1. Receiver Code (Quadruped Controller)

**Purpose:**
Runs on the ESP32 controlling the servos. It receives joystick data via ESP-NOW and generates coordinated walking movements.

**Key Features:**

* Initializes ESP-NOW in station mode.
* Receives joystick input wirelessly.
* Controls four servos: Front Left (FL), Front Right (FR), Rear Left (RL), Rear Right (RR).
* Executes forward and backward stepping sequences.

**Core Components:**

* **Libraries:** `WiFi.h`, `esp_now.h`, `ESP32Servo.h`, `Arduino.h`
* **Servo Pins:** Define GPIOs connected to FL, FR, RL, and RR.
* **Thresholds:**

  * `FWD_THR` – Triggers forward motion.
  * `BACK_THR` – Triggers backward motion.
* **Motion Parameters:**

  * `CENTER` – Neutral position (90°).
  * `AMP` – Step amplitude.
  * `STEP_DELAY` – Speed of motion.
* **Reversal Flags:** Adjust orientation of individual servos.
* **ESP-NOW Callback:** Receives joystick data asynchronously.
* **Helper Functions:**

  * `w()` – Writes constrained angles with optional reversal.
  * `setLeft()` / `setRight()` – Synchronizes leg movement.
* **Gait Functions:**

  * `leftStepForward()`, `rightStepForward()`
  * `leftStepBack()`, `rightStepBack()`
* **Main Loop Logic:**

  * Move forward if `x > FWD_THR`.
  * Move backward if `x < BACK_THR`.
  * Otherwise hold the neutral stance.

**Use Case:**
Flash this code to the ESP32 that controls the servos.

---

## 2. Sender Code (Joystick Transmitter)

**Purpose:**
Runs on a second ESP32. It reads joystick input and transmits the data to the receiver using ESP-NOW.

**Key Features:**

* Reads analog input from the joystick.
* Packages data into a structured format.
* Sends data wirelessly to a specified MAC address.

**Core Components:**

* **Joystick Pin:** `JOY_X` (analog input).
* **Receiver MAC Address:** Identifies the target ESP32.
* **Data Structure:**

  ```cpp
  typedef struct {
    int xValue;
  } JoystickData;
  ```
* **ESP-NOW Initialization:**

  * Configures Wi-Fi in station mode.
  * Registers the receiver as a peer.
* **Send Callback:** Displays transmission status.
* **Main Loop:**

  * Reads `analogRead(JOY_X)`.
  * Sends data using `esp_now_send()`.
  * Prints values to the Serial Monitor.

**Use Case:**
Flash this code to the ESP32 connected to the joystick.

---

## 3. MAC Address Utility

**Purpose:**
Retrieves and prints the ESP32’s MAC address, which is required for ESP-NOW communication.

**Functionality:**

* Initializes Wi-Fi in station mode.
* Prints the device’s MAC address to the Serial Monitor.

**Output Example:**

```
Receiver MAC Address: XX:XX:XX:XX:XX:XX
```

**Use Case:**
Flash this to the receiver ESP32 to obtain its MAC address, then copy it into the sender code.

---

## How to Flash the Correct Program

| Task                 | Receiver Code | Sender Code | MAC Utility |
| -------------------- | ------------- | ----------- | ----------- |
| Control the robot    | ✅ Enabled     | ❌ Commented | ❌ Commented |
| Send joystick data   | ❌ Commented   | ✅ Enabled   | ❌ Commented |
| Retrieve MAC address | ❌ Commented   | ❌ Commented | ✅ Enabled   |

---

## System Workflow

1. Flash the **MAC Utility** to obtain the receiver’s MAC address.
2. Insert the MAC address into the **Sender Code**.
3. Flash the **Receiver Code** onto the servo-controlling ESP32.
4. Flash the **Sender Code** onto the joystick ESP32.
5. Move the joystick to wirelessly control the robot.

---

## Summary

| Program     | Device Role      | Function                                  |
| ----------- | ---------------- | ----------------------------------------- |
| Receiver    | Robot ESP32      | Receives data and controls servo movement |
| Sender      | Controller ESP32 | Reads joystick input and transmits it     |
| MAC Utility | Any ESP32        | Displays the MAC address for pairing      |

This modular structure ensures reliable, low-latency wireless control using ESP-NOW.
