# MPLAB IDE generated this makefile for use with GNU make.
# Project: charlie.mcp
# Date: Mon Sep 27 19:34:13 2010

AS = MPASMWIN.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

charlie.cof : charlie.o
	$(LD) /l"C:\Program Files\Microchip\MCC18\lib" "C:\Program Files\Microchip\MCC18\bin\LKR\18f1320_g.lkr" "charlie.o" /u_CRUNTIME /u_DEBUG /z__MPLAB_BUILD=1 /z__MPLAB_DEBUG=1 /m"charlie.map" /w /o"charlie.cof"

charlie.o : charlie.c C:/Program\ Files/Microchip/MCC18/h/pwm.h envelopes.h scale.h charlie.c C:/Program\ Files/Microchip/MCC18/h/p18f1320.h C:/Program\ Files/Microchip/MCC18/h/pconfig.h C:/Program\ Files/Microchip/MCC18/h/p18cxxx.h
	$(CC) -p=18F1320 "charlie.c" -fo="charlie.o" -D__DEBUG -mL -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

clean : 
	$(RM) "charlie.o" "charlie.cof" "charlie.hex" "charlie.map"

