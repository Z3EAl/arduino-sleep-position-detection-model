#include "Arduino_BMI270_BMM150.h"  // Include library for IMU sensor to read accelerometer, gyroscope, and magnetometer data
#include <TensorFlowLite.h>  // Include TensorFlow Lite library for running models on microcontrollers
#include "model_pak.h"  // Include the header for the pre-trained TensorFlow Lite model
#include "tensorflow/lite/micro/all_ops_resolver.h"  
#include "tensorflow/lite/micro/micro_interpreter.h"  // TensorFlow Lite interpreter for running the model on microcontrollers
#include "tensorflow/lite/micro/system_setup.h"  // Setup required for TensorFlow Lite on embedded systems
#include "tensorflow/lite/schema/schema_generated.h"  // TensorFlow Lite schema to handle model metadata

constexpr int tensorArenaSize = 4096;  // Define memory size for TensorFlow Lite tensor storage (4KB)
alignas(16) uint8_t tensorArena[tensorArenaSize];  // Allocate memory for TensorFlow Lite tensors with proper alignment
constexpr int kCategoryCount = 5;  // Set number of possible output categories (e.g., Supine, Prone, etc.)

// Define the size of the window for sensor readings (10 readings per window)
constexpr int windowSize = 10;  // Size of the window (number of readings)
String predictedPostures[windowSize];  // Array to store predicted postures for the window

// Predefined mean and standard deviation for sensor data standardization
float means[9] = {-0.036870, 0.023168, 0.184674, 0.212121, 0.108095, -0.095129, -4.213771, -43.677507, -17.852089};
float std_devs[9] = {0.562004, 0.533417, 0.677321, 8.696937, 6.522470, 6.681007, 857.186508, 1081.230804, 32.650765};

// Declare pointers for model, interpreter, input and output tensors
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* inputTensor = nullptr;
TfLiteTensor* outputTensor = nullptr;

// Function to standardize sensor data
void standardizeData(float sensorData[9]) {
    // Standardize each sensor reading (subtract mean and divide by standard deviation)
    for (int i = 0; i < 9; i++) {
        // If the data is missing (NaN or zero), replace it with the mean value
        if (isnan(sensorData[i]) || sensorData[i] == 0) {
            sensorData[i] = 0;  // Placeholder if data is missing
        }
        
        // Standardization formula: (sensorValue - mean) / std_dev
        sensorData[i] = (sensorData[i] - means[i]) / std_devs[i];
    }
}

// Function to initialize the TensorFlow Lite model and interpreter
void initializeModel() {
    model = tflite::GetModel(model_tflite);  // Load the model from the precompiled header
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        Serial.println("Model schema version mismatch!");  // Error if model version is not compatible
        return;
    }

    static tflite::AllOpsResolver resolver;  // Resolver for TensorFlow Lite operations (e.g., matrix multiply, activation)
    static tflite::MicroInterpreter staticInterpreter(model, resolver, tensorArena, tensorArenaSize);  // Create interpreter
    interpreter = &staticInterpreter;

    // Allocate tensors needed for the model
    if (interpreter->AllocateTensors() != kTfLiteOk) {
        Serial.println("Failed to allocate tensors");  // Error message if tensor allocation fails
        while (1) ;  // Halt the program if tensor allocation fails
    }

    inputTensor = interpreter->input(0);  // Set the input tensor
    outputTensor = interpreter->output(0);  // Set the output tensor
}

// Function to read data from the selected sensor (accelerometer, gyroscope, magnetometer)
bool readSensorData(int sensorType, float sensorData[9]) {
    // Reset sensor data to NaN before reading new values
    for (int i = 0; i < 9; i++) {
        sensorData[i] = NAN;  // Set initial values as NaN (Not a Number)
    }

    // Read data based on the selected sensor type
    if (sensorType == 1) {  // Accelerometer
        if (IMU.accelerationAvailable()) {
            IMU.readAcceleration(sensorData[0], sensorData[1], sensorData[2]);  // Read accelerometer data
        }
    } else if (sensorType == 2) {  // Gyroscope
        if (IMU.gyroscopeAvailable()) {
            IMU.readGyroscope(sensorData[3], sensorData[4], sensorData[5]);  // Read gyroscope data
        }
    } else if (sensorType == 3) {  // Magnetometer
    if (IMU.accelerationAvailable()) {
            IMU.readAcceleration(sensorData[0], sensorData[1], sensorData[2]);  // Read accelerometer data
        }
        if (IMU.magneticFieldAvailable()) {
            IMU.readMagneticField(sensorData[6], sensorData[7], sensorData[8]);  // Read magnetometer data
        }
    } else if (sensorType == 4) {  // All sensors (accelerometer + gyroscope + magnetometer)
        // Read data from all sensors
        if (IMU.accelerationAvailable()) {
            IMU.readAcceleration(sensorData[0], sensorData[1], sensorData[2]);
        }
        if (IMU.gyroscopeAvailable()) {
            IMU.readGyroscope(sensorData[3], sensorData[4], sensorData[5]);
        }
        if (IMU.magneticFieldAvailable()) {
            IMU.readMagneticField(sensorData[6], sensorData[7], sensorData[8]);
        }
    }

    // Standardize the data after reading
    standardizeData(sensorData);  // Apply standardization (replace NaNs with mean)
    return true;  // Return true if data was read successfully
}

// Function to perform the prediction using the TensorFlow Lite model
String performPrediction(float sensorData[9]) {
    // Copy sensor data to the input tensor
    for (int i = 0; i < inputTensor->bytes / sizeof(float); i++) {
        inputTensor->data.f[i] = sensorData[i];  // Fill input tensor with sensor data
    }

    // Run the inference on the model
    if (interpreter->Invoke() != kTfLiteOk) {
        Serial.println("Inference failed");  // If inference fails, print error
        return "Unknown";  // Return "Unknown" if inference fails
    }

    // Extract output prediction
    float prediction[kCategoryCount];
    for (int i = 0; i < kCategoryCount; i++) {
        prediction[i] = outputTensor->data.f[i];  // Get prediction results from the output tensor
    }

    // Find the category with the highest probability
    int maxIndex = 0;
    for (int i = 1; i < kCategoryCount; i++) {
        if (prediction[i] > prediction[maxIndex]) {
            maxIndex = i;  // Update maxIndex if a higher probability is found
        }
    }

    // Return the predicted posture based on the index with the highest probability
    switch (maxIndex) {
        case 0: return "Supine";
        case 1: return "Prone";
        case 2: return "Side";
        case 3: return "Sitting";
        case 4: return "Unknown";
        default: return "Unknown";
    }
}

// Function to perform majority voting on predictions from multiple windows
String getMajorityVote() {
    int voteCount[kCategoryCount] = {0};  // Array to store votes for each category

    // Count votes for each posture in the window
    for (int i = 0; i < windowSize; i++) {
        if (predictedPostures[i] == "Supine") voteCount[0]++;
        else if (predictedPostures[i] == "Prone") voteCount[1]++;
        else if (predictedPostures[i] == "Side") voteCount[2]++;
        else if (predictedPostures[i] == "Sitting") voteCount[3]++;
        else if (predictedPostures[i] == "Unknown") voteCount[4]++;
    }

    // Find the category with the most votes
    int maxVotes = voteCount[0];
    int maxIndex = 0;
    for (int i = 1; i < kCategoryCount; i++) {
        if (voteCount[i] > maxVotes) {
            maxVotes = voteCount[i];
            maxIndex = i;
        }
    }

    // Return the most voted posture
    switch (maxIndex) {
        case 0: return "Supine";
        case 1: return "Prone";
        case 2: return "Side";
        case 3: return "Sitting";
        case 4: return "Unknown";
        default: return "Unknown";
    }
}

void setup() {
    Serial.begin(115200);  // Start serial communication at 115200 baud rate
    while (!Serial) ;  // Wait for the serial port to be ready

    // Initialize the IMU sensor
    if (!IMU.begin()) {
        Serial.println("Failed to initialize IMU sensor");  // Error if sensor initialization fails
        while (1) ;  // Halt the program if sensor fails to initialize
    }

    // Initialize the TensorFlow Lite model and interpreter
    initializeModel();
}

void loop() {
    Serial.println("Enter '1, 2, or 3' to read sensor data (10 readings), or '0' to exit:");

    // Wait for user input
    while (Serial.available() == 0) ;  // Block until input is available
    if (Serial.available()) {
        int input = Serial.parseInt();  // Read user input from the serial monitor
        
        // If input is 1, 2, 3, or 4, collect 10 readings and make predictions
        if (input == 1 || input == 2 || input == 3 || input == 4) {
            Serial.print("Using sensor type: ");
            Serial.println(input);
            
            // Collect sensor data for 10 readings (forming a window)
            for (int i = 0; i < windowSize; i++) {
                float sensorData[9];
                if (readSensorData(input, sensorData)) {
                    predictedPostures[i] = performPrediction(sensorData);  // Perform prediction for each reading
                } else {
                    return;  // Exit if sensor data is unavailable
                }
                delay(200);  // Small delay between readings to allow time for sensor data collection
            }

            // After collecting all readings, perform majority voting to get the final predicted posture
            String majorityPosture = getMajorityVote();
            Serial.print("Predicted posture: ");
            Serial.println(majorityPosture);  // Print the final predicted posture
        }
        else if (input == 0) {
            return;  // Exit if user enters '0'
        }
    }
}
