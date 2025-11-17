import serial
ser={}
useSerial=1

while(useSerial==1):
    ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate=9600,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.SEVENBITS
    )

while(useSerial==1):
    ser.write("A")
    data=ser.readline()
    theData=int(data.decode('utf-8').strip())
    print(theData)

while(useSerial==1):
    ser.close()

from tkinter import *
from tkinter import messagebox
top = Tk()
C = Canvas(top, bg="blue", height=250, width=300)
coord = 10, 50, 240, 210
arc = C.create_arc(coord, start=0, extent=150, fill="red")
line = C.create_line(10,10,200,200,fill='white')
C.pack()
top.mainloop()

