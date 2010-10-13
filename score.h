

rom near unsigned char score_note[] = {
	//Verse - main : 0
	F4, F2, A2, C3, F3, G4, A4, G4, F4, E4, D4, //1
	E4, F2, A2, C3, F3, F4, G4, F4, E4, D4, C4, //1
	D4, C4,	A2, E4, F4, //2
	D4, B3, Gs2, E4, F4, //2
	G4, As3, G2, A4, As4, //2
	Fs4, C4, D3, G4, A4, //2
	As4, G2, As2, D3, G3, C5, D5, C5, As4, A4, G4, //1
	As4, As2, Cs3, F3, As3, C5, Cs5, C5, Ds3, As4, A4, G4, //3
	A4, F2, A2, C3, F3, As4, A4, As4, A2, A4, G4, F4, //3
	G4, G2, As2, D3, G3, D4, //4
	RET,
	//Verse1 - coda : 84
	A4, C3, E3, G3, As3, G4, //4
	RET,
	//Verse2 - coda : 91
	A4, C3, E3, G3, As3, G4, //5
	F4, F2, A2, C3, F3, A3, C4, F4, //6
	RET,
	//Song : 106
	CALL|0, CALL|84, CALL|0, CALL|91, RET
};
#define NOTE_START 106

#define Tempo 10
#define Qu 1*Tempo	//Quaver
#define Cr 2*Tempo	//Crotchet
#define dC 3*Tempo	//Dotted Crotchet
#define Mi 4*Tempo	//Minim
#define ch 0 		//chord-part (no delay)
//(These deltas cannot be higher than 0x79)

rom near unsigned char score_delta[] = {
	//Rhythms:
	ch, Qu, Qu, Qu,  Qu, Qu, Qu, Cr,  Qu, Qu, Cr, RET, 		//1 : 0
	ch, ch, Mi, Qu,  Qu, RET, 								//2 : 12
	ch, Qu, Qu, Qu,  Qu, Qu, Qu, ch,  Cr, Qu, Qu, Cr,  RET,	//3 : 18
	ch, Qu, Qu, Qu,  Cr, Qu, RET, 	 						//4 : 31
	ch, Qu, Qu, Qu,  ch, dC, RET, 							//5 : 38
	ch, Qu, Qu, Qu,  Qu, Qu, Qu, Qu,  RET, 					//6 : 45

	//Verse main : 54
	CALL|0, CALL|0, 
	CALL|12, CALL|12, CALL|12, CALL|12,
	CALL|0, CALL|18, 
	CALL|18, CALL|31, RET,
	
	//Song : 65
	CALL|54, CALL|31, CALL|54, CALL|38, CALL|45, RET
};
#define DELTA_START 65

