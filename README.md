# **Arduino Nano 33 BLE Sense Rev 2 Sleep Position Detection Model**

This repository contains the code and resources for a sleep position detection model designed for the Arduino Nano 33 BLE Sense Rev 2. The project uses onboard sensors and a pre-trained TensorFlow Lite model to classify sleep positions in real time.

---

## **Features**
- Real-time sleep position detection using the **Arduino Nano 33 BLE Sense Rev 2**.
- Accurate classification of six sleep positions:
  - **Supine** (lying on back)
  - **Prone** (lying on stomach)
  - **Side (Left & Right)** (lying on either side)
  - **Sitting**
  - **Unknown** (positions outside defined categories)
- Lightweight TensorFlow Lite model for microcontrollers.
- Efficient integration with onboard IMU sensors for motion data collection.

---

## **Hardware Requirements**
- **Arduino Nano 33 BLE Sense Rev 2** with onboard sensors:
  - **BMI270** (accelerometer and gyroscope)
  - **BMM150** (magnetometer)
- USB cable for programming and power.

---

## **Software Requirements**
- Arduino IDE (version 1.8.0 or higher) or Arduino Pro IDE.
- Required libraries:
  - **Arduino_BMI270_BMM150** for reading accelerometer, gyroscope, and magnetometer data.
  - **TensorFlowLite** for running the TensorFlow Lite model on the Arduino.
  - **TensorFlow Lite Micro** (automatically included with TensorFlowLite library).
- Python (optional, for training the ML model).

---

## **Installation Instructions**

### 1. Clone the Repository
```bash
git clone https://github.com/Z3EAl/arduino-sleep-position-detection-model.git
cd arduino-sleep-position-detection-model
```

### 2. Install Required Libraries
Install the following libraries via the Arduino Library Manager:
- **Arduino_BMI270_BMM150**
- **TensorFlowLite**

### 3. Upload Code
1. Open the Arduino IDE or Arduino Pro IDE.
2. Navigate to the sketch for your application:
   - **`all_data_collection.ino`** for collecting for each position. Tweak the number of samples in the code as needed
   - **`RealTimeTesting.ino`** for deploying the model and running the real-time detection.
3. Select the **Arduino Nano 33 BLE Sense Rev 2** board from the **Tools > Board** menu.
4. Upload the code to your Arduino Nano 33 BLE Sense Rev 2.

---

## **Usage**

### **Step 1: Data Collection**
1. Use the `all_data_collection.ino` sketch to collect motion data for the defined positions:
   - Supine, Prone, Side (Left & Right), Sitting, and Unknown.
2. Follow these steps:
   - Place the Nano 33 BLE Sense in each position.
   - Record sensor data using the serial monitor or onboard storage.
   - Save the data in CSV format for training.

### **Step 2: Model Training**
1. Use the collected data to train a TensorFlow Lite model.
2. Use the jupyter notebook in the repository to train your own model.
3. Export the trained model as a `.tflite` file and convert it to a `.cpp` file using the `xxd` utility:
   ```bash
   xxd -i model.tflite > model_pak.cpp
   ```

### **Step 3: Real-Time Detection**
1. Include the pre-trained model in your sketch using:
   ```cpp
   #include "model_pak.h"
   ```
2. Run the `RealTimeTesting.ino` sketch to classify sleep positions in real time.
3. Monitor the output via the serial monitor.

---

## **Folder Structure**
```plaintext
arduino-sleep-position-detection-model/
├── ReLUModel.ipynb             # Notebook for training the ML model
├── RealTimeTesting/            # Real-time sleep position detection code
│   ├── RealTimeTesting.ino
│   ├── model_pak.cpp           # Pre-trained TensorFlow Lite model
├── all_data_collection/        # Data collection sketch
│   ├── all_data_collection.ino
├── README.md                   # Project documentation
```

---

## **Output**
The system will classify the current position as one of the following:
- **Supine**
- **Prone**
- **Side (Left)**
- **Side (Right)**
- **Sitting**
- **Unknown**

The detected position will be displayed on the serial monitor or transmitted via Bluetooth if configured.

---

## **Contributing**
Contributions are welcome! Here's how you can contribute:
1. Fork the repository.
2. Create a new branch for your feature/bug fix.
3. Commit your changes and submit a pull request.

---
