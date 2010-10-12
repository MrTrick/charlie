/* ---------------------------------------------------
   Test 1 : Play the fundamental note, over and over.
   ---------------------------------------------------*/
#define TEST_INIT
void test_init();
void test_init() {
	velocity[0]=0x100;
}

rom near unsigned char score_note[]; //not used
rom near unsigned char score_delta[]; //not used

void check_score() {
	//Wait until the previous note has faded - then start a new one.
	if (!decay[0]) {
		wave_ptr[0] = 0;
		decay[0] = 1;
	}
}	
	

