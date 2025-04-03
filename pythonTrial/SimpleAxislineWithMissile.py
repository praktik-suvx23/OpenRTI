import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Simulated ship data from C++ (Replace with actual data transfer later)
ship_1 = {"id": 1, "position": [0, 0], "velocity": [1, 0]}  # Moving right
ship_2 = {"id": 2, "position": [8000, 0], "velocity": [-1, 0]}  # Moving left

# Missile parameters
missile = {"position": [ship_1["position"][0], ship_1["position"][1]], "altitude": 0, "fired": False, "path": [], "finished": False}

def missile_trajectory(t, total_time, current_y, peak_altitude=1000):
    """Calculates missile x, y position at time t."""
    if current_y < peak_altitude:
        # Ascending smoothly
        x = ship_1["position"][0] + ship_1["velocity"][0] * t + 1000 * np.cos(np.pi/4) * (t / (total_time/2))
        y = 1000 * np.sin(np.pi/4) * ((t / (total_time/2)) ** 2)  # Quadratic smooth ascent
    else:
        # Descending towards moving ship_2
        ratio = (t - total_time / 2) / (total_time / 2)
        x = (1 - ratio) * (ship_1["position"][0] + 1000 * np.cos(np.pi/4)) + ratio * ship_2["position"][0]
        y = (1 - ratio) * peak_altitude
    return x, y

# Plot setup
fig, ax = plt.subplots()
ax.set_xlim(-500, 8500)
ax.set_ylim(-0, 1200)
ax.set_xlabel("Distance (m)")
ax.set_ylabel("Altitude (m)")
ax.set_title("Missile Flight Path")

ship1_dot, = ax.plot([], [], 'bo', label="Ship 1")
ship2_dot, = ax.plot([], [], 'ro', label="Ship 2")
missile_dot, = ax.plot([], [], 'go', label="Missile")
missile_path, = ax.plot([], [], 'g--', alpha=0.5, label="Missile Path")
ax.legend()

def update(frame):
    """Animation update function."""
    global ship_1, ship_2, missile
    
    # Move ships
    ship_1["position"][0] += ship_1["velocity"][0]
    ship_2["position"][0] += ship_2["velocity"][0]
    
    # Fire missile at frame 10
    if frame == 10:
        missile["fired"] = True
    
    # Update missile position if fired and not finished
    if missile["fired"] and not missile["finished"]:
        x, y = missile_trajectory(frame, 100, missile["position"][1])
        missile["position"] = [x, y]
        missile["path"].append([x, y])
        
        # Stop updating when it reaches the target
        if x >= ship_2["position"][0]:
            missile["finished"] = True
    
    # Update plot data
    ship1_dot.set_data(ship_1["position"][0], ship_1["position"][1])
    ship2_dot.set_data(ship_2["position"][0], ship_2["position"][1])
    missile_dot.set_data(missile["position"][0], missile["position"][1])
    
    # Update missile path
    if missile["path"]:
        missile_path.set_data(*zip(*missile["path"]))
    
    return ship1_dot, ship2_dot, missile_dot, missile_path

    

ani = animation.FuncAnimation(fig, update, frames=100, interval=100, blit=True)
plt.show()
