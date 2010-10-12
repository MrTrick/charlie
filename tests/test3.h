/* ---------------------------------------------------
   Test 3 : Play increasing notes, by chromatics 
   ---------------------------------------------------*/
#define TEST_INIT
void test_init();
void test_init() {
	note.v = B3;
}
rom near unsigned char score_note[]; //not used
rom near unsigned char score_delta[]; //not used

void check_score() {
	//Wait until the previous note has faded - then start a new one.
	if (!decay[0]) {
		wave_ptr[0] = 0;
		velocity[0] = W_table[note.v];
		decay[0] = 1;
		note.v++;
		if (note.v==As7) stop=1;
	}
}	
	

