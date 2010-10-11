/* ---------------------------------------------------
   Test 5 : Recursive melodies and repetitive use
   ---------------------------------------------------*/
#define TEST_INIT
void test_init();
void note_next();
void delta_next();

struct {
	near unsigned char v, i, d;
	near unsigned char stack[2];	
} note = {0,0,0}, delta = {0,0,0};

void test_init() {
	note.i = 36; //set start points
	delta.i = 18;
}


#define SUB 0x80

rom near unsigned char cap_note[] = {
	F3, C4,	F3, C4, F3, C4,	D3, F3, C4,	F3, C4,	D3, F3, C4,	D3, F3, C4,	0xFF,//18
	E3, B3, E3, B3, E3, B3, C3, E3, B3, E3, B3, C3, E3, B3, C3, E3, B3, 0xFF,//18
	SUB|0, SUB|18, SUB|0, SUB|18, SUB|0, SUB|18, SUB|0, SUB|18, 0xFF

	//F3, C4,	F3, C4, F3, C4,	F3, C4,	F3, C4,	F3, C4,	F3, C4,	F3, C4,	0xFF,//17
	//E3, B3, E3, B3, E3, B3, E3, B3, E3, B3, E3, B3, E3, B3, E3, B3, 0xFF,//17
	//SUB|0, SUB|17, SUB|0, SUB|17, SUB|0, SUB|17, SUB|0, SUB|17, 0xFF
};
/*rom near unsigned char cap_note[] = { 0x85, 0x85, 0x8B, 0x85 , 0xFF, 0x01, 0x02, 0x03, 0x04 , 0x05 , 0xFF, 0x06, 0x07, 0x08, 0x09 , 0x0A, 0xFF };*/
	
rom near unsigned char cap_delta[] = {
	0, 0x20, 0, 0x20, 0, 0x20, 0x10, 0, 0x20, 0, 0x20, 0x10, 0, 0x10, 0x10, 0, 0x20, 0xFF, //18

	//0, 0x20, 0, 0x20, 0, 0x30, 0, 0x20, 0, 0x20, 0, 0x10, 0, 0x20, 0, 0x20, 0xFF,//17
	SUB, SUB, SUB, SUB, SUB, SUB, SUB, SUB, 0xFF,
};

#define A note
#define A_table cap_note
void note_next(void) {
	do {
		A.v = A_table[ A.i++ ];				// Get the next element
		if (A.v == 0xff) { 					// Returning from a subsection?
			if (!A.d) { stop=1; return; }
			A.i = A.stack[--A.d];
		} else if (A.v&0x80) { 				// Going into a sub-section?
			A.stack[A.d++] = A.i;
			A.i = A.v&0x7F;
		} else return;						// Neither, this is the desired element
	} while (1);
}
#undef A 
#undef A_table
#define A delta
#define A_table cap_delta


void delta_next(void) {
	do {
		A.v = A_table[ A.i++ ];				// Get the next element
		if (A.v == 0xff) { 					// Returning from a subsection?
			if (!A.d) { stop=1; return; }
			A.i = A.stack[--A.d];
		} else if (A.v&0x80) { 				// Going into a sub-section?
			A.stack[A.d++] = A.i;
			A.i = A.v&0x7F;
		} else return;						// Neither, this is the desired element
	} while (1);
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
	