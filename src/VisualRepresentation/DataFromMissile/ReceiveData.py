import socket
import struct
import time

HOST = "127.0.0.1"  # Localhost
PORT = 12345        # Port to listen on
TIMEOUT = 30        # Timeout in seconds if no data is received

class Missile:
    def __init__(self, missile_id, missile_team, position, speed, altitude, height_achieved):
        self.missile_id = missile_id
        self.missile_team = missile_team
        self.position = position
        self.speed = speed
        self.altitude = altitude
        self.height_achieved = height_achieved

    def __repr__(self):
        return (f"Missile(ID={self.missile_id}, Team={self.missile_team}, "
                f"Position={self.position}, Speed={self.speed}, "
                f"Altitude={self.altitude}, HeightAchieved={self.height_achieved})")

def deserialize_missile(data):
    # Deserialize the Missile struct from the received data
    missile_id = data[:100].decode('utf-16').strip('\x00')  # 50 wchar_t (100 bytes)
    missile_team = data[100:200].decode('utf-16').strip('\x00')  # 50 wchar_t (100 bytes)
    position = struct.unpack("dd", data[200:216])  # Two doubles (16 bytes)
    speed = struct.unpack("d", data[216:224])[0]  # One double (8 bytes)
    altitude = struct.unpack("d", data[224:232])[0]  # One double (8 bytes)
    height_achieved = struct.unpack("?", data[232:233])[0]  # One bool (1 byte)

    return Missile(missile_id, missile_team, position, speed, altitude, height_achieved)

def listen_for_missiles():
    # Create a socket to listen for incoming connections
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((HOST, PORT))
        server_socket.listen(1)
        print(f"Listening for incoming missile data on {HOST}:{PORT}...")

        # Accept a connection
        server_socket.settimeout(TIMEOUT)  # Set timeout for the socket
        try:
            client_socket, client_address = server_socket.accept()
            print(f"Connection established with {client_address}")
        except socket.timeout:
            print("No connection received within the timeout period. Shutting down.")
            return

        # Receive and process missile data
        with client_socket:
            client_socket.settimeout(TIMEOUT)  # Set timeout for receiving data
            last_received_time = time.time()

            while True:
                try:
                    # Receive serialized missile data (1024 bytes)
                    data = client_socket.recv(1024)
                    if not data:
                        break  # Connection closed by the client

                    # Deserialize and print the missile data
                    missile = deserialize_missile(data)
                    print(f"Received missile data: {missile}")

                    # Update the last received time
                    last_received_time = time.time()

                except socket.timeout:
                    # Check if the timeout period has elapsed
                    if time.time() - last_received_time > TIMEOUT:
                        print("No data received within the timeout period. Shutting down.")
                        break

if __name__ == "__main__":
    listen_for_missiles()