	list p=18F1320, n=0, t=ON, st=OFF
	#include p18F1320.inc

	#define CHANNEL_COUNT 4
;----------------------------------------------------------
	extern wave_table, decay_table
	extern W_table
	
	udata_shr
velocity 	res CHANNEL_COUNT*2
decay 		res CHANNEL_COUNT
wave_ptr	res CHANNEL_COUNT*3
	

; read table at table[offset], where:
;	* table is a near address (in 16 bits)
; 	* offset is a byte var.
read_table_b macro table, offset
	movlw 	low table			; table_L + offset -> TBLPTRL
	addwf	offset, W, ACCESS	; 
	movwf	TBLPTRL, ACCESS		;
	clrf	TBLPTRH				; table_H + c -> TBLPTRH
	movlw	high table			;
	addwfc	TBLPTRH, F, ACCESS	;
	tblrd*
	endm

; read table at table[offset], where:
;	* table is a near address (in 16 bits)
; 	* offset is a short var.
read_table_s macro table, offset
	movlw 	low table			; table_L + offset_L -> TBLPTRL
	addwf	low offset, W, ACCESS ; 
	movwf	TBLPTRL, ACCESS		;
	movlw	high table			; table_H + offset_H + c -> TBLPTRH
	addwfc	high offset, W, ACCESS ;
	movwf	TBLPTRH, ACCESS		;
	tblrd*
	endm
	
		
; synth_run_channel function
synth_run_channel macro N
	; //If the channel is off, skip it.
	; if (!decay[N]) continue;
	movf	decay+N, W, ACCESS	
	bz		end_synth_run_channel
	; //Move the wave pointer forward by the pitch velocity
	; wave_ptr[N] += velocity[N];
	movf	low velocity + N*2, W, ACCESS
	addwf	low wave_ptr + N*3, F, ACCESS
	movf	high velocity + N*2, W, ACCESS
	addwfc	high wave_ptr + N*3, F, ACCESS
	movlw	0
	addwfc	upper wave_ptr + N*3, F, ACCESS
	; //Has the wave pointer overflowed?
	; if (GET_UCHAR(wave_ptr[N],2) & WAVE_OVERFLOW_MASK) {
	btfsc	upper wave_ptr + N*3, F, ACCESS
	bra		add_output
	;	//Jump back to the start of the loop section
	;	GET_USHORT(wave_ptr[N],1) -= LOOP_SIZE; 			
	movlw	0x80
	subwf	high wave_ptr + N*3, F, ACCESS
	movlw	0x00
	subwfb	upper wave_ptr + N*3, F, ACCESS
	;	//Attenuate the output a little more
	;  	decay[CHAN]++; 										\
	incf	decay + N, F, ACCESS
	;}
add_output	
	; //Add 

MOVF ARG1, W
MULWF ARG2 ; ARG1 * ARG2 ->
; PRODH:PRODL
BTFSC ARG2, SB ; Test Sign Bit
SUBWF PRODH, F ; PRODH = PRODH
; - ARG1
MOVF ARG2, W
BTFSC ARG1, SB ; Test Sign Bit
SUBWF PRODH, F ; PRODH = PRODH
; - ARG2
	; 
		



end_synth_run_channel
	endm

		/*Has the pointer reached beyond the wave table?*/		\
		if (GET_UCHAR(wave_ptr[CHAN],2) & WAVE_OVERFLOW_MASK) { \
			/*Jump back to the start of the loop section.*/  	\
			GET_USHORT(wave_ptr[CHAN],1) -= LOOP_SIZE; 			\
			/*Attenuate the output*/							\
			decay[CHAN]++; 										\
		} 														\
		/*Add to the output.*/									\
		raw_level = wave_table[ GET_USHORT(wave_ptr[CHAN],1) ];	\
		output += (raw_level * decay_table[ decay[CHAN] ])/256;	\
}	
	




test_mac macro chk_cnt
variable i
i = 0
while i < chk_cnt
movlw i
i + = 1
endw
endm
start
test_mac 6
end
The directives related to macro



void synth_channel_init(Channel *c, unsigned char note) {
	c->flags.is_active = 1;
	c->flags.in_loop = 1;
	c->velocity = W_table[note];
	c->decay_ptr = 0;
	c->wave.raw = 0;
}

signed char synth_channel_poll(Channel *c) {
	static char out;
	if (!c->flags.is_active) 
		return 0;
	else if (!c->flags.in_loop) {
		out = attack_table[c->wave.attack_ptr];
		c->wave.raw += c->velocity;
		if (c->wave.attack_ptr >= ATTACK_SIZE) {
			c->flags.in_loop = 1;
			c->wave.attack_ptr -= ATTACK_SIZE;
		}
		return out;
	}
	else {
		out = (loop_table[c->wave.loop_ptr] * envelope_table[c->decay_ptr]) / 256;
		c->wave.raw += c->velocity;
		if (c->wave.loop_ptr >= LOOP_SIZE) {
			c->wave.loop_ptr -= LOOP_SIZE;
			if (!++c->decay_ptr) 
				c->flags.is_active = 0;
		}
		return out;
	}
}

