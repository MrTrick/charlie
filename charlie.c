#define USART_DEBUG
#define TESTFILE "tests/test6.h"
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
	#include <usart.h>
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
void push_note();
void check_score();
void attenuate();
//----------------------------------------------------------------
near unsigned short velocity[CHANNEL_COUNT]; 
near unsigned char decay[CHANNEL_COUNT];
near unsigned short long wave_ptr[CHANNEL_COUNT];

near unsigned char i, attenuation; 
near signed char raw_level, channel_output;
near signed short output;

near unsigned char current_channel;
//----------------------------------------------------------------
//near unsigned char delta = 0;
//near unsigned char score_i = 0, note;

near char stop=0;
//----------------------------------------------------------------

//Initial configuration
void init() {
	//Empty the channels
	for(i=0;i<CHANNEL_COUNT;i++) decay[i] = 0;

	#ifdef USART_DEBUG
		OpenUSART( USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & 
					USART_EIGHT_BIT & USART_SINGLE_RX & USART_BRGH_HIGH, 0x00);
	#else
		//Set up PWM
		OpenPWM1(0xFF); 
		SetOutputPWM1(SINGLE_OUT, PWM_MODE_1);
	#endif

	//Set up sampling timer
	OpenTimer0(	TIMER_INT_OFF & T0_8BIT & T0_SOURCE_INT & T0_PS_1_2 );

	//Set up scoring/decay timer
	OpenTimer1( TIMER_INT_OFF & T1_8BIT_RW & T1_SOURCE_INT & T1_PS_1_1);

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

#ifdef TESTFILE
	#include TESTFILE
#else
	void check_score() {
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
				push_note();
				
				// Set the delay to the next note
				delta = score_delta[ score_i ];

				score_i++;
			} while (!delta); 
			else delta--;
		}
	}
#endif


/**
 * Add a new note to the channels - overwrites the oldest note there.
 */
void push_note() {
	// Push that note into a channel
	velocity[current_channel] = W_table[ note.v ];
	decay[current_channel]=1;
	wave_ptr[current_channel] = 0;
	
	// Shift to the next channel
	if (++current_channel==CHANNEL_COUNT) 
		current_channel = 0;
}

void attenuate() { //mul_8s_8u_geth() {
	_asm
		movf attenuation, 0, ACCESS
		mulwf raw_level, ACCESS
		btfsc raw_level,7, ACCESS
		subwf PRODH,1, ACCESS
		movff PRODH, channel_output
	_endasm
}
void main() {
	init();

	#ifdef TEST_INIT
		test_init();
	#endif


	//Process the audio channels
	while(1) {
		//Wait for the next attenuation / score interval		
		if (PIR1bits.TMR1IF) {
			PIR1bits.TMR1IF = 0;					//Re-set the timer
			//TMR1H |= 0x80;						//Halve the timer duration.

			//Do the existing notes need to be attenuated?
			if (decay[0]) decay[0]++;
			if (decay[1]) decay[1]++;
			if (decay[2]) decay[2]++;
			if (decay[3]) decay[3]++;

			//Do new notes need to be played?
			check_score();
		}

		//Wait for the next sample interval
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
					//decay[CHAN]++; 									
				} 														
				/*Add to the output.*/
				raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];
				attenuation = decay_table[ decay[CHAN] ];
				attenuate();
				output += channel_output;
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
					//decay[CHAN]++; 									
				} 														
				/*Add to the output.*/
				raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];
				attenuation = decay_table[ decay[CHAN] ];
				attenuate();
				output += channel_output;
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
					//decay[CHAN]++; 									
				} 														
				/*Add to the output.*/
				raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];
				attenuation = decay_table[ decay[CHAN] ];
				attenuate();
				output += channel_output;
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
					//decay[CHAN]++; 									
				} 														
				/*Add to the output.*/
				raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];
				attenuation = decay_table[ decay[CHAN] ];
				attenuate();
				output += channel_output;
			}
			#undef CHAN

			//Send the output to the serial port or the PWM dc
			#ifdef USART_DEBUG
				//Output only 8-bit data
				//output = output>>2;
				while(BusyUSART());
				putcUSART( (GET_UCHAR(output,1)>>4) + 0x30 );
				while(BusyUSART());
				putcUSART( (GET_UCHAR(output,1)&0x0F) + 0x30 );
				while(BusyUSART());
				putcUSART( (GET_UCHAR(output,0)>>4) + 0x30 );
				while(BusyUSART());
				putcUSART( (GET_UCHAR(output,0)&0x0F) + 0x30 );
			#else
				//Update the PWM duty cycle
				SetDCPWM1(output); 
			#endif
		}
		
		if (stop 
		&& !decay[0] 
		&& !decay[1] 
		&& !decay[2] 
		&& !decay[3])
			goto stop;	
	}
	
	//Stop
	stop: while(1);
}