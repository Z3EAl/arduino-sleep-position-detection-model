import serial
import csv
import os
import time

# Set up the Serial connection
ser = serial.Serial('COM3', 9600) 
time.sleep(2)  # Wait for connection to establish

csv_filename = 'rightSide.csv'

# Check if the CSV file exists
file_exists = os.path.isfile(csv_filename)

# Open the CSV file in append mode ('a'), or create it in write mode ('w') if it doesn't exist
with open(csv_filename, 'a', newline='') as csvfile:
    csvwriter = csv.writer(csvfile)
    
    # If the file doesn't exist, write the header row
    if not file_exists:
        csvwriter.writerow(['Posture', 'Accel_X', 'Accel_Y', 'Accel_Z', 'Gyro_X', 'Gyro_Y', 'Gyro_Z', 'Mag_X', 'Mag_Y', 'Mag_Z'])
    
    # Read and write each line from Serial
    while True:
        if ser.in_waiting > 0:
            # Read the data from the serial port
            data = ser.readline().decode('utf-8').strip()
            print(data)  # Print data to the console
            
            # Write the data to the CSV, split by commas (assuming CSV format from Arduino)
            csvwriter.writerow(data.split(','))
