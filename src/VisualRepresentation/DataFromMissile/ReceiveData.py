import socket
import struct

HOST = "127.0.0.1"  # Localhost
PORT = 12345        # Port to listen on
BUFFER_SIZE = 232   # Expected size of serialized missile data

class Missile:
    def __init__(self, missile_id, missile_team, position, speed, altitude):
        self.missile_id = missile_id
        self.missile_team = missile_team
        self.position = position
        self.speed = round(speed, 2)
        self.altitude = round(altitude, 2)

    def __repr__(self):
        return (f"Missile(ID={self.missile_id}, Team={self.missile_team}, "
                f"Position={self.position}, Speed={self.speed}, "
                f"Altitude={self.altitude})")

def deserialize_missile(data):
    """Deserialize the received byte data into a Missile object."""
    try:
        # Decode missile ID and team as UTF-16 Little Endian
        missile_id = data[:100].decode('utf-16-le', errors='ignore').strip('\x00')
        missile_team = data[100:200].decode('utf-16-le', errors='ignore').strip('\x00')

        # Extract position, speed, and altitude as doubles
        position = struct.unpack("<dd", data[200:216])  # Two doubles (16 bytes)
        speed = struct.unpack("<d", data[216:224])[0]  # One double (8 bytes)
        altitude = struct.unpack("<d", data[224:232])[0]  # One double (8 bytes)

        return Missile(missile_id, missile_team, position, speed, altitude)
    except Exception as e:
        print(f"[ERROR] Failed to deserialize missile data: {e}")
        return None

def recv_full_message(sock, expected_size):
    """Ensure we read exactly `expected_size` bytes."""
    data = b""
    while len(data) < expected_size:
        packet = sock.recv(expected_size - len(data))
        if not packet:
            raise ConnectionError("Connection closed before full message received")
        data += packet
    return data

def receive_missile(sock):
    """Read and deserialize a single missile message."""
    # 1. Read message size (first 4 bytes)
    msg_size_data = recv_full_message(sock, 4)
    msg_size = struct.unpack("<I", msg_size_data)[0]  # Read as uint32_t

    # 2. Read exactly `msg_size` bytes of missile data
    raw_data = recv_full_message(sock, msg_size)

    # 3. Deserialize and return the missile object
    return deserialize_missile(raw_data)

def listen_for_missiles():
    """Listen for incoming missile data and print it in a readable format."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((HOST, PORT))
        server_socket.listen(1)
        print(f"Listening for incoming missile data on {HOST}:{PORT}...")

        # Accept a connection
        client_socket, client_address = server_socket.accept()
        print(f"Connection established with {client_address}")

        with client_socket:
            while True:
                try:
                    missile = receive_missile(client_socket)
                    if missile:
                        print(missile)
                except ConnectionError:
                    print("Connection closed by the sender.")
                    break
                except struct.error as e:
                    print(f"[ERROR] Deserialization failed: {e}")

if __name__ == "__main__":
    listen_for_missiles()