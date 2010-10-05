//----------------------------------------------------------------
//
// Charlie
//
// Ver 0.1 - written in c for the 18F* PIC
//
// Author : Patrick Barnes
// Date : 
//
// Runs at 40MHz - needs a 10Mhz crystal, and is multiplied internally.
//
//----------------------------------------------------------------
#include <p18f1320.h>
#include <pwm.h> 
#include <timers.h>
#pragma config WDT=OFF, OSC=HSPLL, MCLRE=OFF

//Processor speed - controlled by config, and mabye osccon
#define PROC_SPEED 40000000 	
//How far the sampling timer counts before overflowing
#define SAMPLING_TMR_COUNT 250	
//How often it should output a sample. (40000, for 40MHz and 250)
#define SAMPLING_FREQ (PROC_SPEED/4.0/SAMPLING_TMR_COUNT) 
//How many points in the wavetable are equal to one period.
#define P_COUNT 128
//How many simultaneous notes can there be?
#define CHANNEL_COUNT 4
//----------------------------------------------------------------
#include "envelopes.h"
#include "scale.h"
#include "synth.h"
//----------------------------------------------------------------
//Macros and function prototypes
#define GET_UCHAR(arg, offset) *((unsigned char*)&arg+offset)
#define GET_USHORT(arg, offset) *((unsigned short*)((unsigned char*)&arg+offset))

void init();
void isr();
//----------------------------------------------------------------
near unsigned short velocity[CHANNEL_COUNT]; 
near unsigned char decay[CHANNEL_COUNT];
near unsigned short long wave_ptr[CHANNEL_COUNT];

near unsigned char i; 
near unsigned short output;
near signed char raw_level;

//----------------------------------------------------------------
//Initial configuration
void init() {
	//Empty the channels
	for(i=0;i<CHANNEL_COUNT;i++) decay[i] = 0;

	//Set up PWM
	//OpenTimer2(TIMER_INT_OFF & T2_PS_1_16 & T2_POST_1_1);
	OpenPWM1(0xFF); 
	SetOutputPWM1(SINGLE_OUT, PWM_MODE_1);

	//Set up sampling timer
	OpenTimer0(	TIMER_INT_OFF & T0_8BIT & T0_SOURCE_INT & T0_PS_1_1 );

	//Enable interrupts
	//INTCONbits.GIEH=1;
}

//----------------------------------------------------------------
//Interrupt handler
#pragma code low_vector=0x08
void int_vector (void) { _asm GOTO isr _endasm }
#pragma interrupt isr 
//nosave=TBLPTR, TABLAT, PCLATH, PROD, section(".tmpdata")
void isr(void) {

}


void main() {
	init();


	//Wait for the timer to overflow
	while(1) if (INTCONbits.TMR0IF) {
		//Re-set the timer
		INTCONbits.TMR0IF = 0; 
		TMR0L = 0x100-SAMPLING_TMR_COUNT;

		output = 0;
		//Process the audio channels
		#define CHAN 0
		if (decay[CHAN]) { 					
		/*wave_ptr[CHAN] += velocity[CHAN];*/	
		GET_USHORT(wave_ptr[CHAN],0) += velocity[CHAN];
		if (STATUSbits.C) ++GET_UCHAR(wave_ptr[CHAN],2);
		/*Has the pointer reached beyond the wave table?*/
		if (GET_UCHAR(wave_ptr[CHAN],2) & WAVE_OVERFLOW_MASK) {
			/*Jump back to the start of the loop section, and attenuate further*/ 
			GET_USHORT(wave_ptr[CHAN],1) -= LOOP_SIZE; 		
			decay[CHAN]++; 									
		} 														
		/*Add to the output.*/
		/*output+= wave_table[HI(wave_ptr[CHAN])] * decay_table[decay[CHAN]];*/
		raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];
		(raw_level * decay_table[ decay[CHAN] ]);
		if (raw_level&0x80) _asm subwf PRODH, F, ACCESS _endasm;
		GET_UCHAR(output,0) += PRODH;
		if (STATUSbits.C) ++GET_UCHAR(output,1);
	}
	
	//Update the PWM duty cycle
	SetDCPWM1(output); 






	//SetDCPWM1(0x80); 
	//while (1) 
		//synth_channel_poll(&channels[0]);


		//channels[0].wave_ptr.attack++;
	//	for (i=0;i<NOTES_COUNT;i++) 
	//		x = W_table[i];
	
	while(1);
}