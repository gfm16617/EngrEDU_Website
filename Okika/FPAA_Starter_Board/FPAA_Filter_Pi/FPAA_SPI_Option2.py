import pigpio
from time import sleep

# GPIO CLOCK PINs
GPIO_4 = 4

pi = pigpio.pi()
if not pi.connected:
    exit()

# Set FPAA Clock
pi.hardware_clock(GPIO_4, 16000000)

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

#print(list(ahf)) # DEBUG

# Open SPI
SPI_MAX_SPEED = 1000000
SPI_MODE = 3
h = pi.spi_open(0, SPI_MAX_SPEED, SPI_MODE)

# Soft Reset the FPAA
reset_config = [0,0,0,0,0,213,1,111,0]
pi.spi_write(h, list(reset_config))

# Load Configuration
pi.spi_write(h, list(ahf))

pi.spi_close(h)

