import numpy as np
from scipy.optimize import nnls
import serial
import time

# Define matrix that determines RGB value based on on RYB ratio vector
RYB_to_RGB = np.array([[.4957, .4753, .4401],
                       [.3124, .3331, .3409],
                       [.2305, .2255, .2535]])


# HELPER FUNCTIONS ###########################################################################################################
def readRGB(RGB_string):
    RGB = [float(x) for x in RGB_string.split(",")]
    RGB_array = np.array(RGB)
    return RGB_array

def getRYB(RGB_vector, RYB2RGB):
    RYB_vector, error = nnls(RYB2RGB, RGB_vector)
    scale = 1000000
    RYB_int_vector = np.round(RYB_vector * scale).astype(int)
    norm = np.linalg.norm(RGB_vector)
    if norm == 0:
        rel_error = None
    else:
        rel_error = error / norm
   
    return RYB_int_vector, rel_error
## CLASSES ##################################################################################################################
# Define board class
class Board:
    def __init__(self, name, port, baudrate=9600):
        self.name = name
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        self.connect()

    def connect(self):
        if self.serial is not None and self.serial.is_open:
            self.serial.cose()
        print(f"Connecting to {self.name} on {self.port}...")
        self.serial = serial.Serial(self.port, self.baudrate, timeout=1)
        time.sleep(2) 
        self.serial.reset_input_buffer()
        self.serial.reset_output_buffer()
        print(f"{self.name} connected")

    def disconnect(self):
        if self.serial is not None and self.serial.is_open:
            self.serial.close()
            print(f"{self.name} disconnected.")

    def reset_connection(self):
        print(f"Resetting {self.name} connection...")
        self.disconnect()
        time.sleep(1)
        self.connect()

   
    def send_command(self, command):
        if self.serial is None or not self.serial.is_open:
            self.connect()
        self.serial.write((command + "\n").encode())
        self.serial.flush()
    
    def read_until_done(self, timeout=10):
        start = time.time()
        lines = []
        while True:
            if time.time() - start > timeout:
                print(f"{self.name}: timeout")
                break
            line = (self.serial.readline().decode(errors="replace").strip())
            if not line:
                continue
            if line == "DONE":
                break
            lines.append(line)
        return lines
    
    def ping(self, command):
        self.send_command("ping")
        response = self.read_until_done(timeout=3)
        if "PONG" in response:
            print(f"{self.name} responded.")
            return True
        print(f"{self.name} did not respond.")
        return False

# Define board sub classes
class SensorBoard(Board):
    def __init__(self, name, port):
        super().__init__(name, port)

    def check_sensor(self):
        self.send_command("get sensor status")
        results = self.read_until_done()
        for line in results:
            print(line)
    
    def print_reading(self):
        self.send_command("get reading")
        results = self.read_until_done()
        for line in results:
            print(line)
    
    def get_reading(self):
        self.send_command("get reading")
        results = self.read_until_done()
        if not results:
            print("No reading received from sensor.")
            return None
        try:
            RGB_array = readRGB(results[-1])
        except ValueError:
            print("Could not parse RGB reading:", results[-1])
            return None
        return RGB_array


class PumpBoard(Board):
    def __init__(self, name, port, max_mass):
        super().__init__(name, port)
        self.max = max_mass

    def pump_recipe(self, r, y, b, mass):
        if mass <= 0:
            print("Cannot complete recipe. Mass must be positive.")
            return
        if mass > self.max:
            print("Cannot complete recipe. Mass too high")
            return
        
        cmd = f"{r}, {y}, {b}, {mass}"
        self.send_command(cmd)
        print(f"Sent pump recipe: {cmd}")


# Initialize board objects for sensor board and pump board
pumps = PumpBoard("pumps", "/dev/cu.usbmodem11401", 20.0)
sensor = SensorBoard("sensor", "/dev/cu.usbmodem11301" )

time.sleep(2)

# INPUT COMMAND FUNCTIONS ############################################################################################################################################################
def cmd_pump():
    raw = input("Enter recipe in format (R, Y, B, mass): ")
    try:
        values = [float(x.strip()) for x in raw.split(",")]
        if len(values) != 4:
            print("Pump command needs exactly 4 values.")
            return

    except ValueError:
        print("Invalid input. Use numbers like: 1.2, 3.4, 5.6, 7.8")
        return
    pumps.pump_recipe(values[0], values[1], values[2], values[3])
   
def cmd_reset_connections():
    sensor.reset_connection()
    pumps.reset_connection()

def cmd_ping():
    raw = input("Enter board name to ping: pumps or sensor")
    boardname = raw.strip().lower()
    if boardname == "sensor":
        sensor.ping()
    elif boardname == "pumps":
        pumps.ping()
    else:
        print("Please enter a valid sensor name")

def cmd_checkSensor():
    sensor.check_sensor()

def cmd_printReading():
    sensor.print_reading()

def cmd_duplicate():
    raw = input("Enter quantity to duplicate in grams: ")
    try:
        mass = float(raw)
    except ValueError:
        print("Please enter a valid number")
        return
    
    print("Getting RGB reading...")
    reading = sensor.get_reading()

    if reading is None:
        return

    print("Generating RYB recipe...")
    RYB, rel_error = getRYB(reading, RYB_to_RGB)

    print(f"Relative error: {rel_error}")

    pumps.pump_recipe(RYB[0], RYB[1], RYB[2], mass)


commands = {
    "get status": cmd_checkSensor,
    "print reading": cmd_printReading,
    "gen recipe": cmd_pump,
    "duplicate": cmd_duplicate,
    "reset" : cmd_reset_connections
    
}

# Set up live typing of commands
while True:
    # Send command if user typed something
    command = input("Command: ").strip().lower()
    if command == "quit":
        sensor.disconnect()
        pumps.disconnect()
        break
    if command not in commands:
        print("Unknown command")
        print("Available commands:", ", ".join(commands.keys()))
        continue
    commands[command]()

    
   
   

   
    

   
    
   
   





