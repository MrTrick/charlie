

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


rom near unsigned char score_delta[] = {
	//Rhythms:
	0, 10, 10, 10, 10, 10, 10, 20, 10, 10, 20, RET, //1 : 0
	0, 0, 40, 10, 10, RET, //2 : 12
	0, 10, 10, 10, 10, 10, 10, 0, 20, 10, 10, 20, RET, //3 : 18
	0, 10, 10, 10, 20, 10, RET, //4 : 31
	0, 10, 10, 10, 0, 30, RET, //5 : 38
	0, 10, 10, 10, 10, 10, 10, 10, RET, //6 : 45

	//Verse main : 54
	CALL|0, CALL|0, 
	CALL|12, CALL|12, CALL|12, CALL|12,
	CALL|0, CALL|18, 
	CALL|18, CALL|31, RET,
	
	//Song : 65
	CALL|54, CALL|31, CALL|54, CALL|38, CALL|45, RET
};
#define DELTA_START 65


/*rom near unsigned char score_note[] = {
	C2, D2, E2, F2, G2, A2, B2, C3,
	C3, B2, A2, G2, F2, E2, D2, C2, E2, G2,
	0x00
};*/
/*
rom near unsigned char score_delta[] = {
	32, 16, 16, 16, 16, 16, 16, 32,
	32, 16, 16, 16, 16, 16, 16, 0,0,64 //chord
};
*/
