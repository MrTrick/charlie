/* ---------------------------------------------------
   Test 0 : Play a 100Hz square wave.
   ---------------------------------------------------*/
rom near unsigned char score_note[]; //not used
rom near unsigned char score_delta[]; //not used

void check_score() { }

#define TEST_INIT
void test_init();
void test_init() {
	//ClosePWM1();
	TRISBbits.TRISB3 = 0; //Make output
	
	while(1) {
		Delay10KTCYx(1);	
		CCPR1L = 0xFF;
		//PORTBbits.RB3 = 1;
		Delay10KTCYx(1);	
		CCPR1L = 0x00;
		//PORTBbits.RB3 = 0;
		#ifndef DEBUG_WITHOUT_BUTTONS
			if (!button) break;
		#endif
	}
}	
	