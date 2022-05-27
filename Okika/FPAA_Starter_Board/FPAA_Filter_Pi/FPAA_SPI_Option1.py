import pigpio
import spidev
from time import sleep

# GPIO CLOCK PINs
GPIO_4 = 4

pi = pigpio.pi()
if not pi.connected:
    exit()

# Set FPAA Clock
pi.hardware_clock(GPIO_4, 16000000) # 16 Mhz

# Open FPAA ahf File
file_ahf = open('LP_Test_16MHz.ahf', 'r')

# Convert to list of int's
ahf = []
ahf_raw = ['']                                                                
ahf_raw = file_ahf.readlines()
for loop in range(0, len(ahf_raw)):
    temp_bytes = ahf_raw[loop]
    ahf.append(int(temp_bytes[0:2], 16))

file_ahf.close()

# print(list(ahf)) #DEBUG

# Enable SPI
spi = spidev.SpiDev()

# Open a connection to a specific bus and device (chip select pin)
BUS = 0
DEVICE = 0
spi.open(BUS, DEVICE)

# Set SPI speed and mode
spi.max_speed_hz = 15600000 #15.6 MHz
spi.mode = 0b11
spi.lsbfirst = False
spi.bits_per_word = 8

sleep(0.01) # Not necessary

# Soft Reset the FPAA
reset_config = [0,0,0,0,0,213,1,111,0]
response = spi.xfer2(list(reset_config))

# Load Configuration
response = spi.xfer2(list(ahf))
#print(response) #DEBUG

spi.close()

