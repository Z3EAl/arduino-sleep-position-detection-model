#include "Arduino_BMI270_BMM150.h"

// Variables to store accelerometer, gyroscope, and magnetometer data
float accelX, accelY, accelZ;  // Accelerometer data
float gyroX, gyroY, gyroZ;     // Gyroscope data
float magX, magY, magZ;        // Magnetometer data


// Number of samples to collect
const int numSamples = 500;  // Collecting 600 samples per position
const int DEFAULT_DELAY = 200; // Delay between each reading in milliseconds

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud rate
  while (!Serial);  // Wait until the Serial Monitor is opened for viewing
  Serial.println("Started Data Collection");  // Print a message to indicate that data collection has started

  // Initialize the IMU sensor and check if it was successful
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");  // Print error message if the IMU initialization fails
    while (1);  // Stop the program if the IMU fails to initialize
  }

  // Print header for CSV format
  Serial.println("Posture,Accel_X,Accel_Y,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z,Mag_X,Mag_Y,Mag_Z");
}

void loop() {
  for (int i = 0; i < numSamples; i++) {  // Loop to collect the specified number of samples
    // Check if accelerometer data is available
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(accelX, accelY, accelZ);  // Read accelerometer data
    }

    // Check if gyroscope data is available
    if (IMU.gyroscopeAvailable()) {
      IMU.readGyroscope(gyroX, gyroY, gyroZ);  // Read gyroscope data
    }

    // Check if magnetometer data is available
    if (IMU.magneticFieldAvailable()) {
      IMU.readMagneticField(magX, magY, magZ);  // Read magnetometer data
    }

    // Print the sample number and all readings in a single line (CSV format)
    Serial.print("supine");  // Print the current posture name or label
    Serial.print(",");  // Print a comma to separate values
    Serial.print(accelX); Serial.print(",");  // Print accelerometer X value
    Serial.print(accelY); Serial.print(",");  // Print accelerometer Y value
    Serial.print(accelZ); Serial.print(",");  // Print accelerometer Z value
    Serial.print(gyroX); Serial.print(",");   // Print gyroscope X value
    Serial.print(gyroY); Serial.print(",");   // Print gyroscope Y value
    Serial.print(gyroZ); Serial.print(",");   // Print gyroscope Z value
    Serial.print(magX); Serial.print(",");    // Print magnetometer X value
    Serial.print(magY); Serial.print(",");    // Print magnetometer Y value
    Serial.print(magZ); Serial.println();     // Print magnetometer Z value and go to a new line

    delay(DEFAULT_DELAY);  // Delay between samples
  }

  // Print message indicating that data collection is complete
  Serial.println("Data Collection Complete");
  while (1);  // Stop the program after collecting all samples
}
