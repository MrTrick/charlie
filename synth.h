#ifndef CHAN
#error "CHAN must be defined"
#endif
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
		output += (raw_level * attenuation);
}
#undef CHAN

