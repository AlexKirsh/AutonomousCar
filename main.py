# This project requires PyBluez
import bluetooth
# Math libraries
import numpy as np
from room_map import *


############## Functions
# Parse data from IR sensors
def parse_sensor_data(raw_data):
    parsed_data = raw_data.split(";")[1]
    parsed_data = parsed_data.split(",")
    first_reading = int(parsed_data[0][3:])
    parsed_data = parsed_data[1:-1]
    parsed_data = list(map(int, parsed_data))
    parsed_data.insert(0, first_reading)
    return parsed_data


# Get drive time
def get_drive_time(raw_data):
    parsed_data = raw_data.split(";")[2]
    parsed_data = int(parsed_data[5:]) / 1000
    return parsed_data


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

# Robot speed in [cm/s]
v = 46.66

drive_direction = 1  # 1 for forward 0 for backwards (in respect to the initial position)

while True:
    data = sock.recv(1024)
    decoded_data = data.decode('utf-8')
    message += decoded_data
    if message[len(message) - 1] == '.':
        sensor_file.write(message)
        ir_data = parse_sensor_data(message)
        t = get_drive_time(message)
        print(v*t)
        room_map_mat = populate_map_points(room_map_mat, ir_data, strt_loc, v*t, drive_direction)
        drive_direction = 1 - drive_direction  # Flip the direction of the next drive
        user_input = input("Proceed robot movement?")
        if user_input == "n":
            sock.send("END")
            break
        else:
            sock.send("ACK")
        message = ""

sensor_file.close()

sock.close()

plt.imshow(room_map_mat, cmap='Greys')
plt.show()
