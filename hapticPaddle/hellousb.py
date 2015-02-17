import serial
import os

ser = serial.Serial(
    port='COM4',\
    baudrate=19200,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
        timeout=0)

#f = open("values.txt","w+")
while True:
    line = ser.readline()
    if ":" in line:
        print line
        #f.write(line)
        #f.flush();
        #os.fsync(f.fileno())