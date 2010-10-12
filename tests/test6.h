/* ---------------------------------------------------
   Test 6 : Recursive melodies and repetitive use
            Plays the Star theme from Super Mario
   ---------------------------------------------------*/
#define TEST_INIT
void test_init(void);

rom near unsigned char score_note[] = {
	F3, C4,	F3, C4, F3, C4,	D3, F3, C4,	F3, C4,	D3, F3, C4,	D3, F3, C4,	RET,//18
	E3, B3, E3, B3, E3, B3, C3, E3, B3, E3, B3, C3, E3, B3, C3, E3, B3, RET,//18
	CALL|0, CALL|18, CALL|0, CALL|18, CALL|0, CALL|18, CALL|0, CALL|18, RET
};
	
rom near unsigned char score_delta[] = {
	0, 0x20, 0, 0x20, 0, 0x20, 0x10, 0, 0x20, 0, 0x20, 0x10, 0, 0x10, 0x10, 0, 0x20, RET, //18
	CALL, CALL, CALL, CALL, CALL, CALL, CALL, CALL, RET,
};


void test_init() {
	note.i = 36; //set start points
	delta.i = 18;
}

void check_score() {
	//Is it time for the next note?
	if (!delta.v) do {
		// Get the next note. If no more notes, end the song.
		note_next();
		delta_next();
		if (stop) return;

		// Push that note into a channel
		push_note();
	} while (!delta.v); 
	else delta.v--;
}	
	