# This project requires PyBluez
import bluetooth
# Math libraries
import numpy as np
from room_map import *


############## Functions
# Parse data from IR sensor
def parse_ir_data(raw_data):
    parsed_data = raw_data.split(";")[1]
    parsed_data = parsed_data.split(",")
    first_reading = int(parsed_data[0][3:])
    parsed_data = parsed_data[1:-1]
    parsed_data = list(map(int, parsed_data))
    parsed_data.insert(0, first_reading)
    return parsed_data


# Parse data from Sonar sensor(last reading)
def parse_sonar_data(raw_data):
    parsed_data = raw_data.split(";")[0]
    parsed_data = parsed_data.split(",")
    parsed_data = parsed_data[-2:-1]
    parsed_data = list(map(int, parsed_data))
    return parsed_data[0]


# Get drive distance
def get_drive_distance(raw_data):
    parsed_data = raw_data.split(";")[2]
    parsed_data = float(parsed_data[9:])
    return parsed_data


def record_drive_data(message, room_map_mat, strt_loc, drive_direction):
    ir_data = parse_ir_data(message)
    sonar_data = parse_sonar_data(message)
    distance = get_drive_distance(message)
    print(distance)
    room_map_mat = populate_map_points(room_map_mat, ir_data, sonar_data, strt_loc, distance, drive_direction)
    return room_map_mat

############## Program stats here
# Create Bluetooth socket
sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

# Look for all Bluetooth devices
# the computer knows about.
print("Searching for devices...")
print("")

nearby_devices = bluetooth.discover_devices()
# Run through all the devices found and select HC-05
for addr in nearby_devices:
    if bluetooth.lookup_name(addr) == 'HC-05':
        print("Found HC-05 - MAC Address: ", addr)
        sock.connect((addr, 1))
        break

# Send SYN to Arduino (Initiate communication)
sock.send("START")

# Receive Data from Arduino
print("Waiting for Data: ")
message = ""

# Open CSV file for sensors readings
sensor_file = open("C:\\Users\\Alexey\\Documents\\Arduino\\Autonomous Car Project\\ir_readings.csv", "a")

# Configure bitmap matrix that represents the room (1cm resolution, room size- 300X500cm)
room_map_mat = np.zeros((300, 500))
strt_loc = (250, 15)  # Left bottom point of matrix as shown using imshow

drive_direction = 1  # 1 for forward 0 for backwards (in respect to the initial position)

while True:
    data = sock.recv(1024)
    decoded_data = data.decode('utf-8')
    message += decoded_data
    if message[len(message) - 1] == '|':
        sensor_file.write(message)
        #record_drive_data(message, room_map_mat, strt_loc, drive_direction)
        user_input = input("Proceed robot movement?")
        if user_input == "n":
            sock.send("END")
            break
        else:
            if drive_direction == 1:
                sock.send("L")
            else:
                sock.send("R")
        message = ""

        drive_direction = 1 - drive_direction  # Flip the direction of the next drive

sensor_file.close()

sock.close()

plt.imshow(room_map_mat, cmap='Greys')
plt.show()
