# This project requires PyBluez
import bluetooth

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

while True:
    data = sock.recv(1024)
    decoded_data = data.decode('utf-8')
    message += decoded_data
    #print("Received: ")
    #print(message)
    if message[len(message) - 1] == '.':
        sensor_file.write(message)
        user_input = input("Proceed robot movement?")
        if user_input == "n" in message:
            sock.send("END")
            break
        else:
            sock.send("ACK")
        message = ""

sensor_file.close()

sock.close()