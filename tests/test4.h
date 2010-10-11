/* ---------------------------------------------------
   Test 4 : Play a major scale repeatedly 
   ---------------------------------------------------*/
#define TEST_INIT
void test_init();
void test_init() {
	i=4;
}

rom near unsigned char scale_note[] = {
	C4, D4, E4, F4, G4, A4, B4, C5, 
	//C5, E5, G5, C6, G5, E5, C5, 
	0x00
};


void check_score() {
	if (!delta) {
		delta = 0x80;
		note = scale_note[score_i++];
		if (!note) { 
			score_i=0;
	 		if (!--i) stop=1;
		}
		else push_note();
	}
	else delta--;
}	
	

