//#define DEBUG_WITH_USART
//#define DEBUG_WITHOUT_BUTTONS
//#define TESTFILE "tests/test2.h"
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
#include <delays.h>
#ifdef DEBUG_WITH_USART
	#include <usart.h>
#endif
#pragma config OSC=HSPLL, FSCM=OFF, IESO=OFF
#pragma config BOR=OFF, PWRT=ON, WDT=OFF, MCLRE=OFF
#pragma config DEBUG=ON, LVP=OFF, STVR=OFF
				

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
//What is the ratio between decay and note tempo
#define TEMPO_SCALING 5
//----------------------------------------------------------------
#include "envelopes.h"
#include "scale.h"
//----------------------------------------------------------------
#define GET_UCHAR(arg, offset) *((unsigned char*)&arg+offset)
#define GET_USHORT(arg, offset) *((unsigned short*)((unsigned char*)&arg+offset))


void init(void);
void isr(void);
void play_song(void);

//----------------------------------------------------------------
// Scoring vars and prototypes

void score_init(void);
void check_score(void);
void note_next(void);
void delta_next(void);
void push_note(void);

struct {
	near unsigned char v, i, d;
	near unsigned char stack[2];	
} note = {0,0,0}, delta = {0,0,0};

near unsigned char tempo_scaling=0;
near unsigned char stop=0, finished=0;

#define CALL 0x80 //In the score - to call a sub-section
#define RET 0xFF //In the score - to return from a sub-section or to end the song.

//----------------------------------------------------------------
// Synthesis vars and prototypes

void attenuate(void);

near unsigned short velocity[CHANNEL_COUNT]; 
near unsigned char decay[CHANNEL_COUNT];
near unsigned short long wave_ptr[CHANNEL_COUNT];

near unsigned char attenuation; 
near signed char raw_level, channel_output;
near signed short output;

near unsigned char current_channel;

//----------------------------------------------------------------
// IO vars
#define button PORTBbits.RB5
#define not_amp_en TRISAbits.TRISA0
//----------------------------------------------------------------
#ifdef TESTFILE
	#include TESTFILE
#else
	#include "score.h"
#endif
//----------------------------------------------------------------
//Initial configuration

void init() {
	#ifdef DEBUG_WITH_USART
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

	//Configure the IO pins
	ADCON1 = 0xFF;				//Disable the analog pins
	INTCON2bits.NOT_RBPU = 0;	//Allow weak pull-ups
	INTCONbits.RBIE = 1; 		//Enable interrupt-on-change for RB4:7
	PORTAbits.RA0 = 0;			//Set RA0 low
	not_amp_en = 1;				//Amplifier initially disabled (amplifier operates through low/hi impedance)
}




//----------------------------------------------------------------
// Interrupt handler
#pragma code int_vector=0x08
void int_vector (void) { _asm GOTO isr _endasm }
#pragma interrupt isr 
void isr(void) {
	//
}

//----------------------------------------------------------------
// Program entry
void main() {
	init();
	
	while (1) {
		#ifndef DEBUG_WITHOUT_BUTTONS
			//Wait for a button click
			while (button) {
				INTCONbits.RBIF = 0; //Clear the pin change flag
				Sleep();			//Sleep until something happens
				Delay10KTCYx(50); //Wait, so that bounces can be ignored
			} 
			while(!button); //Wait for the button to be released
			Delay10KTCYx(50);
		#endif

		//Initialise
		#ifdef TEST_INIT
			test_init();
		#endif
		#ifndef TESTFILE
			score_init();
		#endif
	
		//Play the song in its entirety, unless the button is pressed
		not_amp_en = 0;
		play_song();
		not_amp_en = 1;
		CCPR1L = 0; //Ensure the pwm duty cycle is cleared

		#ifndef DEBUG_WITHOUT_BUTTONS
			//Make sure the button is released 
			Delay10KTCYx(50);
			while(!button); 
			Delay10KTCYx(50);
		#else
			break;
		#endif
	}
	stop: while(1);
}

//----------------------------------------------------------------
// Scoring functions
#ifndef TESTFILE
void score_init(void);

//Reset the score back to the start of the song
void score_init() {
	//Initialise the score pointers
	note.i = NOTE_START;
	note.d = 0;
	delta.i = DELTA_START;
	delta.d = 0;

	//Empty the channels
	decay[0] = 0;
	decay[1] = 0;
	decay[2] = 0;
	decay[3] = 0;

	//Prep the first note to play immediately
	tempo_scaling = 1;
	delta.v = 0;
}

//Called periodically, plays new notes at the right time.
void check_score() {
	//Is it time for the next note?
	if (!delta.v) do {
		// Get the next note. If no more notes, end the song.
		note_next();
		delta_next();
		if (finished) return;

		// Push that note into a channel
		push_note();
	} while (!delta.v); 
	else delta.v--;
}	
#endif


// Add a new note to the channels - overwrite the oldest note there.
void push_note() {
	// Push that note into a channel
	velocity[current_channel] = W_table[ note.v ];
	decay[current_channel]=1;
	wave_ptr[current_channel] = 0;
	
	// Shift to the next channel
	if (++current_channel==CHANNEL_COUNT) 
		current_channel = 0;
}

//Fetch the next note out of the notes table
//(table is compressed using sub-melodies)
void note_next(void) {
	do {
		note.v = score_note[ note.i++ ];		// Get the next element
		if (note.v == RET) { 					// Returning from a subsection?
			if (!note.d) { finished=1; return; }
			note.i = note.stack[--note.d];
		} else if (note.v&CALL) { 				// Going into a sub-section?
			note.stack[note.d++] = note.i;
			note.i = note.v&~CALL;
		} else return;						// Neither, this is the desired element
	} while (1);
}

//Fetch the next delta out of the delta table
//(table is compressed using sub-rhythms)
void delta_next(void) {
	do {
		delta.v = score_delta[ delta.i++ ];		// Get the next element
		if (delta.v == RET) { 					// Returning from a subsection?
			if (!delta.d) { finished=1; return; }
			delta.i = delta.stack[--delta.d];
		} else if (delta.v&CALL) { 				// Going into a sub-section?
			delta.stack[delta.d++] = delta.i;
			delta.i = delta.v&~CALL;
		} else return;						// Neither, this is the desired element
	} while (1);
}
//----------------------------------------------------------------
// Synthesis Functions

//Reduce the raw table lookup by a given attenuation amount
void attenuate() { //mul_8s_8u_geth() {
	_asm
		movf attenuation, 0, ACCESS
		mulwf raw_level, ACCESS
		btfsc raw_level,7, ACCESS
		subwf PRODH,1, ACCESS
		movff PRODH, channel_output
	_endasm
}

//Calculate the waveform for a single channel.
//(macro, because C18 doesn't have inline functions)
#define PROCESS_CHANNEL(CHAN) if (decay[CHAN]) { 								\
	/*wave_ptr[CHAN] += velocity[CHAN];*/										\
	GET_USHORT(wave_ptr[CHAN],0) += velocity[CHAN];								\
	if (STATUSbits.C) ++GET_UCHAR(wave_ptr[CHAN],2);							\
	/*Has the pointer reached beyond the wave table?*/							\
	if (GET_UCHAR(wave_ptr[CHAN],2) & WAVE_OVERFLOW_MASK)						\
		/*Jump back to the start of the loop section, and attenuate further*/ 	\
		GET_USHORT(wave_ptr[CHAN],1) -= LOOP_SIZE; 								\
	/*Add to the output.*/														\
	raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];						\
	attenuation = decay_table[ decay[CHAN] ];									\
	attenuate();																\
	output += channel_output;													\
}

//Play through the score, and output the appropriate 
//waveforms, until stopped or finished.
void play_song() {
	stop = 0;
	finished = 0;

	while(!stop) {
		//Is the button pressed?
		#ifndef DEBUG_WITHOUT_BUTTONS
			if (!button) break;
		#endif

		//Has the melody finished playing?
		if (finished && !decay[0] && !decay[1] && !decay[2] && !decay[3])
			break;	

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
			if (!--tempo_scaling) {
				tempo_scaling = TEMPO_SCALING;
				check_score();
			}
		}

		//Wait for the next sample interval
		if (INTCONbits.TMR0IF) {
			//Re-set the timer
			INTCONbits.TMR0IF = 0; 
			TMR0L += 0x100-SAMPLING_TMR_COUNT;
	
			output = 0;
			//Process the audio channels
			PROCESS_CHANNEL(0)
			PROCESS_CHANNEL(1)
			PROCESS_CHANNEL(2)
			PROCESS_CHANNEL(3)

			//Send the output to the serial port or the PWM dc
			#ifdef DEBUG_WITH_USART
				//Output only 8-bit data
				output = ((output+CHANNEL_COUNT*128)>>2);
				while(BusyUSART());
				putcUSART( (GET_UCHAR(output,0)>>4) + 0x30 );
				while(BusyUSART());
				putcUSART( (GET_UCHAR(output,0)&0x0F) + 0x30 );
			#else
				//Update the PWM duty cycle
				//(At this speed, only 8 bits of resolution are 
				// available, so don't bother with the full precision.
				CCPR1L = (unsigned char)((output+CHANNEL_COUNT*128)>>2);
			#endif
		}
	}
}		
	
