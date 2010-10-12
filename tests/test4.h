/* ---------------------------------------------------
   Test 4 : Play a major scale repeatedly 
   ---------------------------------------------------*/
near unsigned char score_i=0;
near unsigned char repeat=2;

rom near unsigned char score_note[] = {
	C4, D4, E4, F4, G4, A4, B4, C5, 
	//C5, E5, G5, C6, G5, E5, C5, 
	0x00
};

rom near unsigned char score_delta[]; //not used here

void check_score() {
	if (!delta.v) {
		delta.v = 0x80;
		note.v = score_note[score_i++];
		if (!note.v) { 
			score_i=0;
	 		if (!--repeat) stop=1;
		}
		else push_note();
	}
	else delta.v--;
}	
	

