/* ---------------------------------------------------
   Test 5 : Play some simple chords 
   ---------------------------------------------------*/
near unsigned char score_i=0;
near unsigned char repeat=2;

rom near unsigned char score_note[] = {
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

rom near unsigned char score_delta[] = {
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
	if (!delta.v) do {
		// Get the next note. If no more notes, end the song.
		note.v = score_note[ score_i ];

		// Push that note into a channel
		if (note.v) {
			push_note();
			// Set the delay until the next note
			delta.v = score_delta[ score_i ];
			score_i++;
		} else {
	 		if (!--repeat) stop=1;
			score_i=0;
		}
	} while (!delta.v); 
	else delta.v--;
}	
	

