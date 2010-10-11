/* ---------------------------------------------------
   Test 5 : Play some simple chords 
   ---------------------------------------------------*/
#define TEST_INIT
void test_init();
void test_init() {
	i=3;
}

rom near unsigned char chord_note[] = {
	C4, 
	C4, E4, 
	C4, E4, G4, 
	C4, E4, G4, C5, 
	C4, 
	C4, F4, 
	C4, F4, A4, 
	D4, F4, G4, B4, 
	0x00
};

rom near unsigned char chord_delta[] = {
	0x80, 
	0, 0x80, 
	0, 0, 0x80, 
	0, 0, 0, 0xFF, 
	0x80, 
	0, 0x80, 
	0, 0, 0x80, 
	0, 0, 0, 0xFF, 
};

void check_score() {
	//Is it time for the next note?
	if (!delta) do {
		// Get the next note. If no more notes, end the song.
		note = chord_note[ score_i ];

		// Push that note into a channel
		if (note) {
			push_note();
			// Set the delay until the next note
			delta = chord_delta[ score_i ];
			score_i++;
		} else {
	 		if (!--i) stop=1;
			score_i=0;
		}
	} while (!delta); 
	else delta--;
}	
	

