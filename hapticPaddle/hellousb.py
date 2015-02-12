import serial

ser = serial.Serial(
    port='COM5',\
    baudrate=19200,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
        timeout=0)

while True:
    line = ser.readline()
    print line