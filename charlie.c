#define USART_DEBUG
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
#ifdef USART_DEBUG
	#include <stdlib.h>
	#include <usart.h>
	char obuf[6];
#endif
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
#include "score.h"

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

near unsigned char i, attenuation; 
near signed short output;
near signed short raw_level;

near unsigned char current_channel;
//----------------------------------------------------------------
near unsigned char delta = 0;
near unsigned char score_i = 0, note;

//----------------------------------------------------------------

//Initial configuration
void init() {
	//Empty the channels
	for(i=0;i<CHANNEL_COUNT;i++) decay[i] = 0;

	#ifdef USART_DEBUG
		OpenUSART( USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & 
					USART_EIGHT_BIT & USART_SYNC_MASTER & USART_SINGLE_RX, 25);
	#else
		//Set up PWM
		//OpenTimer2(TIMER_INT_OFF & T2_PS_1_16 & T2_POST_1_1);
		OpenPWM1(0xFF); 
		SetOutputPWM1(SINGLE_OUT, PWM_MODE_1);
	#endif

	//Set up sampling timer
	OpenTimer0(	TIMER_INT_OFF & T0_8BIT & T0_SOURCE_INT & T0_PS_1_1 );

	//Set up scoring timer
	OpenTimer1( TIMER_INT_OFF & T1_8BIT_RW & T1_SOURCE_INT & T1_PS_1_8);

	//Enable interrupts
	//INTCONbits.GIEH=1;
}

//----------------------------------------------------------------
//Interrupt handler
#pragma code low_vector=0x08
void int_vector (void) { _asm GOTO isr _endasm }
#pragma interrupt isr 
void isr(void) {

}

near unsigned short xptr;
near signed char out;
near unsigned char d;
#define TEST1
#ifdef TEST1
	#include "tests/test1.h"
#else
void main() {
	init();
	xptr=0;
	d=10;	

/*
	while(1) {
		//Wait until the next sample
		if (INTCONbits.TMR0IF) {
			//Re-set the timer
			INTCONbits.TMR0IF = 0; 

			#define CHAN 0
			if (decay[CHAN]) { 					
				++GET_USHORT(wave_ptr[CHAN],1);
				//Has the pointer reached beyond the wave table?
				if (GET_UCHAR(wave_ptr[CHAN],2) & WAVE_OVERFLOW_MASK) {
					//Jump back to the start of the loop section, and attenuate further
					GET_USHORT(wave_ptr[CHAN],1) -= LOOP_SIZE; 		
					decay[CHAN]++; 									
				} 														
				//Add to the output.
				raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];
				attenuation = decay_table[ decay[CHAN] ];

				output = (raw_level * attenuation);
			} else {
				decay[CHAN]=1;
				wave_ptr[CHAN]=0;
			}
	
			//out = GET_UCHAR(output,1);
			itoa(output, obuf);
			while(BusyUSART());	
			putsUSART( obuf );
			putcUSART('\n');
			//out = 0xff;
			//putcUSART(out);

			//putcUSART(GET_UCHAR(output,1));
			//putcUSART(GET_UCHAR(output,0));
		}
	}*/
	


	while(1) {
		//Does another note need to be played?
		if (PIR1bits.TMR1IF) {
			//Re-set the timer
			PIR1bits.TMR1IF = 0;
			if (--d) continue;
			d=10;
			
			//Is it time for the next note?
			if (!delta) do {
				// Get the next note. If no more notes, end the song.
				note = score_note[ score_i ];
				if (!note) goto stop;

				// Push that note into a channel
				velocity[current_channel] = W_table[ note ];
				decay[current_channel]=1;
				wave_ptr[current_channel] = 0;
				
				// Shift to the next channel
				if (++current_channel==CHANNEL_COUNT) 
					current_channel = 0;
				
				// Set the delay to the next note
				delta = score_delta[ score_i ];

				score_i++;
			} while (!delta); 
			else delta--;
		}
		
		//Wait until the next sample
		if (INTCONbits.TMR0IF) {
			//Re-set the timer
			INTCONbits.TMR0IF = 0; 
			TMR0L += 0x100-SAMPLING_TMR_COUNT;
	
			output = 0;
			//Process the audio channels (four of them)
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
				raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];
				attenuation = decay_table[ decay[CHAN] ];
				output += (raw_level * attenuation)/256;
			}
			#undef CHAN
			#define CHAN 1
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
				raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];
				attenuation = decay_table[ decay[CHAN] ];
				output += (raw_level * attenuation)/256;
			}
			#undef CHAN
			#define CHAN 2
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
				raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];
				attenuation = decay_table[ decay[CHAN] ];
				output += (raw_level * attenuation)/256;
			}
			#undef CHAN
			#define CHAN 3
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
				raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];
				attenuation = decay_table[ decay[CHAN] ];
				output += (raw_level * attenuation)/256;
			}
			#undef CHAN
	
			#ifdef USART_DEBUG
				itoa(output, obuf);
				putsUSART( obuf );
				putcUSART('\n');
				//putcUSART(GET_UCHAR(output,1));
				//putcUSART(GET_UCHAR(output,0));
				
			#else
				//Update the PWM duty cycle
				SetDCPWM1(output); 
			#endif
		}


	}

	//Stop
	stop: while(1);
}
#endif
