import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Serial configuration
PORT = 'COM5'  # Change this to your serial port name
BAUD_RATE = 9600
ser = serial.Serial(PORT, BAUD_RATE, timeout=1)

# Set up the plot
fig, ax = plt.subplots()

# Line objects for the data series
ln_target, = ax.plot([], [], 'r-', label='Target', animated=True)
ln_measurement, = ax.plot([], [], 'b-', label='Measurement', animated=True)
ln_state, = ax.plot([], [], 'g-', label='State', animated=True)

x_data, target_data, measurement_data, state_data = [], [], [], []

def init():
    ax.set_xlim(0, 1000)
    ax.set_ylim(0, 250)  # Adjust these values based on expected temperature range
    ax.set_ylabel('Temperature')
    ax.legend()
    return ln_target, ln_measurement

def update(frame):
    try:
        line = ser.readline().decode('utf-8').strip()
        parsed = [s.split('=')[1] for s in line.split(',')]
        print(parsed)
        target, measurement, active, state = float(parsed[0]), float(parsed[1]), int(parsed[2]), int(parsed[3])

        if state == 1:
            state_val = target
        else:
            state_val = 0
        
        if len(x_data) > 0 and state != state_data[-1]:
            # If there's a state change, add an extra data point just before the change
            x_data.append(len(x_data))
            target_data.append(target_data[-1])
            measurement_data.append(measurement_data[-1])
            state_data.append(state_data[-1])

        if len(x_data) < 1000:
            x_data.append(len(x_data))
            measurement_data.append(measurement)
            state_data.append(state_val)
            if active == 1:  # Only add the target if active
                target_data.append(target)
            else:
                target_data.append(None)
        else:
            x_data.pop(0)
            target_data.pop(0)
            measurement_data.pop(0)
            state_data.pop(0)
            x_data.append(x_data[-1] + 1)
            measurement_data.append(measurement)
            state_data.append(state_val)
            if active == 1:  # Only add the target if active
                target_data.append(target)
            else:
                target_data.append(0)

        ln_target.set_data(x_data, target_data)
        ln_measurement.set_data(x_data, measurement_data)
        ln_state.set_data(x_data, state_data)
        ax.set_xlim(x_data[0], x_data[-1])
    except Exception as e:
        print(f"Error: {e}")
    return ln_target, ln_measurement, ln_state

ani = FuncAnimation(fig, update, frames=None, init_func=init, blit=True)

try:
    plt.show()
except KeyboardInterrupt:
    ser.close()
    print("Serial connection closed.")
