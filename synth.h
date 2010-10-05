#define W 0
#define F 1

#define RUN_CHANNEL(N, output) if (decay[N]) { 					\
		/*wave_ptr[N] += velocity[N];*/							\
		GET_USHORT(wave_ptr[0],0) += velocity[0];				\
		if (STATUSbits.C) ++GET_UCHAR(wave_ptr[0],2);			\
		/*Has the pointer reached beyond the wave table?*/		\
		if (GET_UCHAR(wave_ptr[N],2) & WAVE_OVERFLOW_MASK) { 	\
			/*Jump back to the start of the loop section.*/  	\
			GET_USHORT(wave_ptr[N],1) -= LOOP_SIZE; 			\
			/*Attenuate the output*/							\
			decay[N]++; 										\
		} 														\
		/*Add to the output.*/									\
		raw_level = wave_table[ GET_USHORT(wave_ptr[N],1) ];	\
		output += (raw_level * decay_table[ decay[N] ])/256;	\
}	