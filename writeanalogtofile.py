#!/usr/bin/python
from InstrumentControl import *
import numpy
import sys
import glob

nrep=int(sys.argv[1])
fname=str(sys.argv[2])

devices=glob.glob('/dev/ttyACM*')
if len(devices)!=1:
 print 'Several serial ports detected! Aborting'
 quit()
board=StepperMotor(devices[0])

try:
 steptime=int(sys.argv[3])
 board.SetStepTime(steptime)
except:
 pass

try:
 racetime=float(sys.argv[4])
 board.race_time=racetime
except:
 pass

voltages=numpy.zeros((360,nrep), dtype=int)
for i in range(nrep):
 voltages[:,i]=board.SetPositionAndListen(360+i*360)

board.CloseSerial()

numpy.savetxt(fname, voltages, fmt='%d')
