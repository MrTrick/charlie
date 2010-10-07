/* ---------------------------------------------------
   Test 1 : Play the fundamental note, over and over.
   ---------------------------------------------------*/
char count=16;

void outputHex(unsigned char x) {
	while(BusyUSART());
	putcUSART( (x>>4) + 0x30 );
	while(BusyUSART());
	putcUSART( (x&0x0F) + 0x30 );
}	
	
void main() {
	init();

	//Fundamental = velocity 1.
	velocity[0]=0x0;
	
	while(1) {
		//Wait until the previous note has faded - then start a new one.
		if (!decay[0]) {
			wave_ptr[0] = 0;
			velocity[0] += 0x100;
			decay[0] = 1;
			if (!--count) goto stop;
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
				output += 1024; //Convert to unsigned value
				//itoa(output, obuf);
				//putsUSART( obuf );
				//putcUSART('\n');
				//while(BusyUSART());	
				outputHex(GET_UCHAR(output,1));
				outputHex(GET_UCHAR(output,0));
			#else
				//Update the PWM duty cycle
				SetDCPWM1(output); 
			#endif
		}


	}

	//Stop
	stop: while(1);
}

