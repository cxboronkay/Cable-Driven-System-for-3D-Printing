
import string
import re
import math

# create and handle G-code coordinates
class GCoord:
    
    def __init__(self, x, y):
        self.x = round (x, 10)
        self.y = round(y,10)
        # self.z = round(z, 10)         ## commented out to perform 2D operations instead
        # self.e = round(e, 10)

    def __str__(self):
        return f'X({self.x}) Y({self.y})'
    
    def __repr__(self):
        return f'X({self.x}) Y({self.y})'

    def getX(self):
        return self.x
    
    def getY(self):
        return self.y
    
    # def getZ(self):
    #     return self.z
    
    # def getE(self):
    #     return self.e
    
# open and read gcode txt file      (write chosen gcode file)
with open('gcode_circle.txt') as f:
    allLines = f.readlines()
f.close()

# create list to store Gcoordinates in
Gcoord_list = []
Gcoord_list.append(GCoord(0,0))

# parse Gcode for coordinates
for line in allLines:
    lineItems = line.split()
    if len(lineItems) != 0:
        xcoord = 0
        ycoord = 0
        # zcoord = 0
        # ecoord = 0
        monitor = [0, 0]
        # monitor = [0, 0, 0, 0]

        if lineItems[0]== 'G1':
            for item in lineItems:
                if item[0] == 'X':
                    xcoord = float(item[1:])
                    monitor[0] = 1
                if item[0] == 'Y':
                    ycoord = float(item[1:])
                    monitor[1] = 1
                # if item[0] == 'Z':
                #     zcoord = float(item[1:])
                #     monitor[2] = 1
                # if item[0] == 'E':
                #     ecoord = float(item[1:])
                #     monitor[3] = 1

            numMissing = 2 - sum(monitor)
            while numMissing != 0:
                for check in monitor:
                    if check == 0:
                        indMissing = monitor.index(0)
                        if indMissing == 0:
                            xcoord = GCoord.getX(Gcoord_list[-1])
                            monitor[0] = 1
                            numMissing = 3 - sum(monitor)
                        if indMissing == 1:
                            ycoord = GCoord.getY(Gcoord_list[-1])
                            monitor[1] = 1
                            numMissing = 3 - sum(monitor)
                        # if indMissing == 2:
                        #     zcoord = GCoord.getZ(Gcoord_list[-1])
                        #     monitor[2] = 1
                        #     numMissing = 3 - sum(monitor)
                        # if indMissing == 3:
                        #     ecoord = GCoord.getE(Gcoord_list[-1])
                        #     monitor[3] = 1
                        #     numMissing = 4 - sum(monitor)
            
            Gcoord_list.append(GCoord(xcoord,ycoord))

if len(Gcoord_list) > 1:
    Gcoord_list.pop(0)

print("G-code Coordinates")
print(Gcoord_list)

print('---------------------------------------------------------------------')

def convert_coord(gcoord):
    xc = GCoord.getX(gcoord)/180*400            ##calculations made based on G-code written for Prusa Mini (180x180mm print bed)
    yc = GCoord.getY(gcoord)/180*400                ## and on pulley configuration with 400mm between pulleys (see schematic)
    # zc = GCoord.getZ(gcoord)/180*400
    # ec = GCoord.getE(gcoord)/180*399
    A = round(math.sqrt(xc**2 + (yc-300)**2))         ## rounds to integers
    B = round(math.sqrt((xc-400)**2 + (yc-300)**2))
    # C = round(math.sqrt(xc**2 + (yc-400)**2 + (zc-250)**2))
    # D = round(math.sqrt((xc-400)**2 + (yc-400)**2 + (zc-400)**2))
    ## in millimeters
    return (A,B)

def python_to_arduino_list(coordlist):          ## create string in list format for Arduino sketch
    arduinoString = "{ \n"
    for crd in coordlist:
        arduinoString += "{"
        for a in crd:
            arduinoString += str(a)
            arduinoString += ","
        arduinoString = arduinoString[:-1]
        arduinoString += "}, \n"
    arduinoString += "}"
    return arduinoString

CDPRCoords = []
for c in Gcoord_list:
    CDPRCoords.append(convert_coord(c))
print("CDPR Coordinates")
print(CDPRCoords)
print('---------------------------------------------------------------------')
print("Number of coordinates:")             ##needed for Arduino pulley control sketch
print(len(CDPRCoords))
stringForArduino = python_to_arduino_list(CDPRCoords)
print("Copy this to Arduino sketch:")
print(stringForArduino)


