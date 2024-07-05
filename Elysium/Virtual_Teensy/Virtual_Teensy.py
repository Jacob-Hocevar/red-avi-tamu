# This models the output data [format only] from the Teensy and can read the data sent through the COM signal.

import serial
import time
import random

time_start = time.time_ns()
time.sleep(0.1)

# TODO: Update output format
# TODO: Update timeout / write_timeout values to give fast response time, but not an unstoppable loop.
ser = serial.Serial("COM98", baudrate=115200, timeout=1, write_timeout=5)
while 1:
    incoming_data = ser.readline()
    print(incoming_data)

    try:
        output = ""
        output += str(time.time_ns()*1000 - time_start)[:-6] + ','
        output += str(30 + random.random()) + ','
        output += str(20 + 2*random.random()) + "\r\n"

        #print(output.encode())
        ser.write(output.encode())
    except:
        print("Writing Timed out")
    #time.sleep(0.01)
