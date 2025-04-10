import socket
import select
import struct
import threading
import socket as pysocket
import time
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from scipy.interpolate import make_interp_spline
import numpy as np
from datetime import datetime, timedelta, timezone

last_data_received = datetime.now(timezone.utc)
heartbeat_active = True 

HOST = "127.0.0.1"  # Localhost
MISSILE_PORT = 12345        # MISSILE_PORT to listen on
REDSHIP_PORT = 12346        # SHIP_PORT to listen on
BLUESHIP_PORT = 12347       # SHIP_PORT to listen on
BUFFER_SIZE_MISSILE = 232   # Expected size of serialized missile data
BUFFER_SIZE_SHIP = 256      # Expected size of serialized ship data

class Missile:
    def __init__(self, missile_id, missile_team, position, speed, altitude):
        self.missile_id = missile_id
        self.missile_team = missile_team
        self.positions = [position]  # Store all positions for the path
        self.speeds = [round(speed, 2)]
        self.altitudes = [round(altitude, 2)]

    def update(self, position, speed, altitude):
        """Update the missile's position, speed, and altitude."""
        self.positions.append(position)
        self.speeds.append(round(speed, 2))
        self.altitudes.append(round(altitude, 2))

    def __repr__(self):
        return (f"Missile(ID={self.missile_id}, Team={self.missile_team}, "
                f"Last Position={self.positions[-1]}, Speed={self.speeds[-1]}, "
                f"Altitude={self.altitudes[-1]})")
    
class Ship:
    def __init__(self, ship_id, ship_team, position, speed, size, HP):
        self.ship_id = ship_id
        self.ship_team = ship_team
        self.positions = [position]  # Store all positions for the path
        self.speed = round(speed, 2)
        self.size = size
        self.HP = HP

    def update(self, position, speed, size, HP):
        """Update the ship's position, speed, size, and HP."""
        self.positions.append(position)
        self.speed = round(speed, 2)
        self.size = size
        self.HP = HP

    def __repr__(self):
        return (f"Ship(ID={self.ship_id}, Team={self.ship_team}, "
                f"Last Position={self.positions[-1]}, Speed={self.speed}, "
                f"Size={self.size}, HP={self.HP})")

def recv_full_message(sock, expected_size):
    """Ensure we read exactly `expected_size` bytes."""
    data = b""
    while len(data) < expected_size:
        packet = sock.recv(expected_size - len(data))
        if not packet:
            raise ConnectionError("Connection closed before full message received")
        data += packet
    return data

def deserialize_missile(data):
    """Deserialize the received byte data into a Missile object."""
    try:
        # Decode missile ID and team as UTF-16 Little Endian
        missile_id = data[:100].decode('utf-16-le', errors='ignore').strip('\x00')
        missile_team = data[100:200].decode('utf-16-le', errors='ignore').strip('\x00')

        # Extract position, speed, and altitude as doubles
        raw_position = struct.unpack("<dd", data[200:216])  # Two doubles (16 bytes)
        position = (raw_position[1], raw_position[0])  # Swap longitude and latitude
        speed = struct.unpack("<d", data[216:224])[0]  # One double (8 bytes)
        altitude = struct.unpack("<d", data[224:232])[0]  # One double (8 bytes)

        return Missile(missile_id, missile_team, position, speed, altitude)
    except Exception as e:
        print(f"[ERROR] Failed to deserialize missile data: {e}")
        return None
    
def deserialize_ship(data):
    """Deserialize the received byte data into a Ship object."""
    try:
        # Decode ship ID and team as UTF-16 Little Endian
        ship_id = data[:100].decode('utf-16-le', errors='ignore').strip('\x00')
        ship_team = data[100:200].decode('utf-16-le', errors='ignore').strip('\x00')

        # Extract position, speed, size, and HP as doubles
        raw_position = struct.unpack("<dd", data[200:216])  # Two doubles (16 bytes)
        position = (raw_position[1], raw_position[0])  # Swap longitude and latitude
        speed = struct.unpack("<d", data[216:224])[0]  # One double (8 bytes)
        size = struct.unpack("<d", data[224:232])[0]  # One double (8 bytes)
        HP = struct.unpack("<d", data[232:240])[0]  # One double (8 bytes)

        return Ship(ship_id, ship_team, position, speed, size, HP)
    except Exception as e:
        print(f"[ERROR] Failed to deserialize ship data: {e}")
        return None

def receive_missile(sock):
    """Read and deserialize a single missile message."""
    # 1. Read message size (first 4 bytes)
    msg_size_data = recv_full_message(sock, 4)
    msg_size = struct.unpack("<I", msg_size_data)[0]  # Read as uint32_t

    # 2. Read exactly `msg_size` bytes of missile data
    raw_data = recv_full_message(sock, msg_size)

    # 3. Deserialize and return the missile object
    return deserialize_missile(raw_data)

def receive_ship(sock):
    """Read and deserialize a single ship message."""
    # 1. Read message size (first 4 bytes)
    msg_size_data = recv_full_message(sock, 4)
    msg_size = struct.unpack("<I", msg_size_data)[0]  # Read as uint32_t

    # 2. Read exactly `msg_size` bytes of ship data
    raw_data = recv_full_message(sock, msg_size)

    # 3. Deserialize and return the ship object
    return deserialize_ship(raw_data)

def send_with_length(sock, message: str):
    msg_bytes = message.encode('utf-8')
    msg_length = len(msg_bytes).to_bytes(4, byteorder='little')  # 4-byte little endian
    sock.sendall(msg_length + msg_bytes)

def heartbeat_sender(admin_ip='127.0.0.1', admin_port=12348):
    global last_data_received, heartbeat_active
    maxConnectionTries = 0

    while True:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((admin_ip, admin_port))
            print(f"[Heartbeat] Connected to admin at {admin_ip}:{admin_port}")
            break
        except socket.error as e:
            if maxConnectionTries >= 15:
                print(f"[Heartbeat Error] Connection failed after {maxConnectionTries} attempts. Exiting...")
                return
            if maxConnectionTries % 5 == 0:
                print(f"[Heartbeat Error] Connection failed: {e}. Retrying...")
            time.sleep(1)
            maxConnectionTries += 1

    try:
        while heartbeat_active:
            time_since = datetime.now(timezone.utc) - last_data_received
            if time_since < timedelta(seconds=10):
                send_with_length(sock, "1")
            else:
                print(f"[Heartbeat] No activity for {time_since.total_seconds()} seconds. Sent 'complete'.")
                for _ in range(10):
                    send_with_length(sock, "0")
                    time.sleep(0.5)
                heartbeat_active = False
            time.sleep(1)

    except Exception as e:
        print(f"[Heartbeat Error] If [Errno 32] - ignore. \n\tIt's about \'Heartbeat\' is trying to send data over a socket that is closed.\n\t{e}")
    finally:
        sock.close()
        print("[Heartbeat] Socket closed.")



def listen_for_missiles_and_ships():
    global last_data_received
    """Listen for incoming missile and ship data and visualize them in the same 3D plot."""
    # Initialize the 3D plot
    fig = plt.figure(figsize=(10, 6))
    ax = fig.add_subplot(111, projection='3d')
    ax.set_xlabel("Longitude")
    ax.set_ylabel("Latitude")
    ax.set_zlabel("Altitude")
    ax.set_title("Real-Time Missile and Ship Visualization")
    plt.ion()  # Enable interactive mode for real-time updates

    # Dictionaries to store Missile and Ship objects by their IDs
    missiles = {}
    ships = {}

    missile_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    blueship_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    redship_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Bind and listen on all ports
        missile_socket.bind((HOST, MISSILE_PORT))
        missile_socket.listen(1)
        print(f"Listening for incoming missile data on {HOST}:{MISSILE_PORT}...")

        blueship_socket.bind((HOST, BLUESHIP_PORT))
        redship_socket.bind((HOST, REDSHIP_PORT))
        blueship_socket.listen(1)
        redship_socket.listen(1)
        print(f"Listening for incoming ship data on {HOST}:{BLUESHIP_PORT} and {HOST}:{REDSHIP_PORT}...")

        # Accept connections for both missiles and ships
        missile_client, missile_address = missile_socket.accept()
        print(f"Missile connection established with {missile_address}")

        blueship_client, blueship_address = blueship_socket.accept()
        print(f"BlueShip connection established with {blueship_address}")

        redship_client, redship_address = redship_socket.accept()
        print(f"RedShip connection established with {redship_address}")

        threading.Thread(target=heartbeat_sender, daemon=True).start()

        while True:
            try:
                readable, _, _ = select.select([missile_client, blueship_client, redship_client], [], [], 1.0)
                for sock in readable:
                    if sock == missile_client and missile_client is not None:
                        try:
                            missile = receive_missile(missile_client)
                            if missile:
                                last_data_received = datetime.now(timezone.utc)
                                if missile.missile_id in missiles:
                                    missiles[missile.missile_id].update(
                                        missile.positions[-1], missile.speeds[-1], missile.altitudes[-1]
                                    )
                                    print(f"[UPDATE] Missile {missile.missile_id} data.")
                                else:
                                    missiles[missile.missile_id] = missile
                                    print(f"[NEW] Missile {missile.missile_id} data.")
                        except ConnectionError:
                            print(f"{sock.getpeername()} Missile connection closed.")
                            sock.close()
                            if sock in readable:
                                readable.remove(sock)

                    elif sock == blueship_client and blueship_client is not None:
                        try:
                            blue_ship = receive_ship(blueship_client)
                            if blue_ship:
                                last_data_received = datetime.now(timezone.utc)
                                if blue_ship.ship_id in ships:
                                    ships[blue_ship.ship_id].update(
                                        blue_ship.positions[-1], blue_ship.speed, blue_ship.size, blue_ship.HP
                                    )
                                    print(f"[UPDATE] BlueShip {blue_ship.ship_id} data.")
                                else:
                                    ships[blue_ship.ship_id] = blue_ship
                                    print(f"[NEW] BlueShip {blue_ship.ship_id} data.")
                        except ConnectionError:
                            print(f"{sock.getpeername()} Blueship connection closed.")
                            sock.close()
                            if sock in readable:
                                readable.remove(sock)

                    elif sock == redship_client and redship_client is not None:
                        try:
                            red_ship = receive_ship(redship_client)
                            if red_ship:
                                last_data_received = datetime.now(timezone.utc)
                                if red_ship.ship_id in ships:
                                    ships[red_ship.ship_id].update(
                                        red_ship.positions[-1], red_ship.speed, red_ship.size, red_ship.HP
                                    )
                                    print(f"[UPDATE] RedShip {red_ship.ship_id} data.")
                                else:
                                    ships[red_ship.ship_id] = red_ship
                                    print(f"[NEW] RedShip {red_ship.ship_id} data.")
                        except ConnectionError:
                            print(f"{sock.getpeername()} Redship connection closed.")
                            sock.close()
                            if sock in readable:
                                readable.remove(sock)

                    ax.clear()  # Clear the plot for updating
                    ax.set_xlabel("Longitude")
                    ax.set_ylabel("Latitude")
                    ax.set_zlabel("Altitude")
                    ax.set_title("Real-Time Missile and Ship Visualization")

                    for missile_id, missile_obj in missiles.items():
                        x_values = [pos[0] for pos in missile_obj.positions]
                        y_values = [pos[1] for pos in missile_obj.positions]
                        z_values = missile_obj.altitudes

                        color = "red" if missile_obj.missile_team == "Red" else "blue"
                        
                        if len(set(x_values)) > 2 and len(set(y_values)) > 2 and len(set(z_values)) > 2:
                            try:
                                t = np.linspace(0, len(x_values) - 1, 100)  # Generate 100 points for smooth curve
                                spline_x = make_interp_spline(range(len(x_values)), x_values)(t)
                                spline_y = make_interp_spline(range(len(y_values)), y_values)(t)
                                spline_z = make_interp_spline(range(len(z_values)), z_values)(t)

                                ax.plot(spline_x, spline_y, spline_z, linestyle="--", color=color, label=f"Missile {missile_id}")
                            except ValueError as e:
                                print(f"[WARNING] Spline interpolation failed for Missile {missile_id}: {e}")
                                ax.plot(x_values, y_values, z_values, linestyle="--", marker="o", color=color, label=f"Missile {missile_id}")
                        else:
                            # Not enough unique points for smoothing
                            #print(f"[DEBUG] Not enough unique points for Missile {missile_id}. Plotting without smoothing.")
                            #print(f"[DEBUG] Missile {missile_id}: x_values={x_values}, y_values={y_values}, z_values={z_values}")
                            #print(f"[DEBUG] Unique points: x={len(set(x_values))}, y={len(set(y_values))}, z={len(set(z_values))}")
                            ax.plot(x_values, y_values, z_values, linestyle="--", marker="x", color=color, label=f"Missile {missile_id}")

                    # Plot ships
                    for ship_id, ship_obj in ships.items():
                        x_values = [pos[0] for pos in ship_obj.positions]
                        y_values = [pos[1] for pos in ship_obj.positions]
                        z_values = [0] * len(ship_obj.positions)  # Ships are at ground level (altitude = 0)

                        # Assign color based on ship team
                        color = "red" if ship_obj.ship_team == "Red" else "blue"

                        # Plot the trajectory without markers
                        ax.plot(
                            x_values, y_values, z_values,
                            linestyle="-.", color=color, label=f"Ship {ship_id}"
                        )

                        # Plot the current position with a marker
                        ax.scatter(
                            x_values[-1], y_values[-1], z_values[-1],
                            color=color, marker="s", s=100, label=f"Ship {ship_id} (Current Position)"
                        )

                    # Add legend if there are any missiles or ships
                    if missiles or ships:
                        ax.legend(loc="upper left", bbox_to_anchor=(1.05, 1), borderaxespad=0)
                    
                    if missile_client is None and redship_client is None and blueship_client is None:
                        print("[INFO] All connections closed. Exiting...")
                        missile_socket.close()
                        blueship_socket.close()
                        redship_socket.close()
                        missile_client = None
                        blueship_client = None
                        redship_client = None
                        return

            except Exception as e:
                print(f"[ERROR 123456] If 'Heartbeat' = complete, then not an error.\n\t{e}")
                break 

    finally:
        # Close all sockets
        missile_socket.close()
        blueship_socket.close()
        redship_socket.close()

        # Print all received missile and ship data
        print("\nFinal Missile Data:")
        for missile_id, missile_obj in missiles.items():
            print(f"Missile ID: {missile_id}, Data: {missile_obj}")

        print("\nFinal Ship Data:")
        for ship_id, ship_obj in ships.items():
            print(f"Ship ID: {ship_id}, Data: {ship_obj}")

        # Disable interactive mode and show the final plot
        plt.ioff()
        plt.show()  # Keep the plot open after the connection is closed

if __name__ == "__main__":
    listen_for_missiles_and_ships()