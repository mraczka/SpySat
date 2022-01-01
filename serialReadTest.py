import serial
import re

serialPort = serial.Serial(port = "COM19", baudrate=115200,
                           bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)

serialString = ""                           # Used to hold data coming over UART

ReceviedSerialData = ""

print(serialPort.name)
while (True):

    
    if(serialPort.in_waiting > 0):
        
        # Read data out of the buffer until a carraige return / new line is found
        serialString = serialPort.readline()

        # Print the contents of the serial data
        print(serialString.decode('Ascii')) #Type: string
        
        ReceviedSerialData = serialString.decode('Ascii')
        if("RSSI" in ReceviedSerialData):
            ReceviedRSSI = int(re.search(r'\d+', ReceviedSerialData).group())
            if('-' in ReceviedSerialData):
                ReceviedRSSI = int(str('-') + str(ReceviedRSSI))
                print(ReceviedRSSI)
            else:
                print(ReceviedRSSI)
        # Tell the device connected over the serial port that we recevied the data!
        # The b at the beginning is used to indicate bytes!
        #serialPort.write(b"Thank you for sending data \r\n")
