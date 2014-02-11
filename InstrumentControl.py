#!/usr/bin/python
#This module contains the StepperMotor class and its associated methods, which are used to command an Arduino loaded with the ArduinoStepperControl sketch.

import serial
import numpy
import struct
import time

def packIntegerAsULong(value): #a common function to pack 4 byte unsigned integer to an arduino unsigned long, to be sent over 4 serial writes
    return struct.pack('i', value)

class StepperMotor:
    """
The object for an arduino controlling a stepper motor.
The communication protocol involves 5 bytes per write to the arduino, with the first byte describing the function to be called and the next four an unsigned long int, typically the step to get to. 
    """
    def __init__(self, StepperMotorAddress):
        self.race_time=0.5 #the race condition prevention delay, in seconds
        self.StepperMotorAddress = StepperMotorAddress #the user needs to figure out the serial port path, typically /dev/ttyADC0 or similar
        self.StepperMotor = serial.Serial(StepperMotorAddress, 115200, timeout=0.25) #connect at 115.2 kbd, creating a serial object for the arduino
        time.sleep(4.0*self.race_time) #a very iffy rule of thumb
        self._step_time=self.GetStepTime() #the time per step in us, not user accessible. Use GetStepTime() to get the step time
        time.sleep(self.race_time)
        self._resetbyte()
        print "StepperMotor: Serial Communication Open" 

    def CloseSerial(self):
        #Close Serial Port
        self.StepperMotor.close()
        time.sleep(self.race_time)
        print "StepperMotor: Serial Communication Closed"  
        return

    def _resetbyte(self):
        self.StepperMotor.write(chr(255))
        self.StepperMotor.write(packIntegerAsULong(0))
        time.sleep(self.race_time)
        return

    def SetPosition(self,steps):
       """
This method sets the position of the motor, without reading anything from the analog port.
Note that there are two functions on the arduino sketch for this, for positive and negative steps, as the simple communication protocol only supports positive values.
       """
       position=self.GetPosition()
       if steps > -1:
               self.StepperMotor.write(chr(0))
               self.StepperMotor.write(packIntegerAsULong(steps))
       if steps < 0:
               self.StepperMotor.write(chr(1))
               self.StepperMotor.write(packIntegerAsULong(-steps))
       time.sleep(self.race_time)
       time.sleep(1e-6*self._step_time*numpy.fabs(steps-position))
       return

    def SetPositionAndListen(self,steps):
       """
This method sets the position of the motor, without reading anything from the analog port.
Note that there are two functions on the arduino sketch for this, for positive and negative steps, as the simple communication protocol only supports positive values.
       """
       position=self.GetPosition()
       print position
       serialReads=[]
       if steps > -1:
               self.StepperMotor.write(chr(2))
               self.StepperMotor.write(packIntegerAsULong(steps))
       if steps < 0:
               self.StepperMotor.write(chr(3))
               self.StepperMotor.write(packIntegerAsULong(-steps))
       time.sleep(self.race_time)
       time.sleep(1e-6*self._step_time*numpy.fabs(steps-position))
       serialReads=self.StepperMotor.readlines() #read from serial until nothing happens
       return numpy.array([int(serialReads[i][:-1]) for i in range(len(serialReads))]) #repack the values read from serial (which have a carriage return) and make an array out of the list

    def GetPosition(self): #returns the position of the stepper, with the initial position being 0
        self.StepperMotor.write(chr(4)+chr(4)+chr(4)+chr(4)+chr(4))
        time.sleep(self.race_time)
        return int(self.StepperMotor.readline())

    def ReadAnalog(self): #single analog reading, just in case
        self.StepperMotor.write(chr(5)+chr(5)+chr(5)+chr(5)+chr(5))
        time.sleep(self.race_time)
        return int(self.StepperMotor.readline())

    def GetVersion(self): #returns the sketch version, to make sure the arduino is loaded with the right one
        self.StepperMotor.write(chr(6)+chr(6)+chr(6)+chr(6)+chr(6))
        time.sleep(self.race_time)
        return float(self.StepperMotor.readline())

    def GetStepTime(self): #returns the step time of the arduino sketch, in microseconds
        self.StepperMotor.write(chr(253))
        self.StepperMotor.write(packIntegerAsULong(0))
        time.sleep(self.race_time)
        return int(self.StepperMotor.readline())

    def SetStepTime(self,StepTime): #change the step time of the arduino sketch, in microseconds
        self.StepperMotor.write(chr(254))
        self.StepperMotor.write(packIntegerAsULong(StepTime))
        time.sleep(self.race_time)
        self._step_time=int(self.StepperMotor.readline())
        return 

