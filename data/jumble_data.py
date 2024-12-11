# Import necessary libraries
import pandas as pd

# Define the list of CSV files to combine
data_files = [
    'supine.csv','supine2.csv',
    'prone.csv',
    'leftSide.csv','leftSide2.csv', 
    'rightSide.csv','rightSide2.csv',
    'sitting.csv', 'sitting2.csv',
    'unknown.csv'
]

# Create an empty list to store individual DataFrames
combined_data_list = []

# Loop through each file, read and add to the list
for file in data_files:
    data = pd.read_csv(file)
    combined_data_list.append(data)

# Merge all DataFrames into one
all_data = pd.concat(combined_data_list, ignore_index=True)

# Randomize the order of rows
shuffled_data = all_data.sample(frac=1, random_state=36).reset_index(drop=True)

# Export the shuffled combined data to a new CSV
shuffled_data.to_csv('Shuffled_Combined_Data.csv', index=False)

print("Data has been combined and shuffled successfully!")

