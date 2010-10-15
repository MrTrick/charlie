// 
// 1. For each note, the desired pitch (frequency) F is known.
// 2. One period of the note waveform is stored as a set of P_COUNT points.
// 3. Pitch is a function of the speed (W) over which those points are 
//    traversed.
// 4. The output level is only adjusted at a _fixed_ rate, SAMPLING_FREQ 
//    times per second.
// 5. If W=1, and the waveform advances by W at a rate of SAMPLING_FREQ,
//    the pitch of that output tone will be SAMPLING_FREQ/P_COUNT : the 
//    "fundamental frequency" (For 32000 khz / 128 points, F_fun is 250 Hz)
// 6. For notes higher than F_fun, W>1, For notes lower than F_fun, W<1.
//    
// In most cases, W is not a whole number, so it is stored here as a 16-bit 
// [whole|fractional] word. eg 2.34 => [0x02,floor(0.34*256)]
// The waveform is advanced only by the upper 'whole' byte.

// The macro calcW calculates how fast the table pointer moves, and stores in a [whole|fractional] word.
#define UPSCALE 8.0 //Make the notes higher so the little speaker can play them better
#define calcW(F) (short)( (F)*UPSCALE*256.0*(float)P_COUNT/(float)SAMPLING_FREQ + 0.5 )
#define NOTES_COUNT 72
rom near unsigned short W_table[NOTES_COUNT] = {
//	B				C				C#				D				D#				E				F				F#				G				G#				A				A#
	calcW(61.74),	calcW(65.41),	calcW(69.3),	calcW(73.42),	calcW(77.78),	calcW(82.41),	calcW(87.31),	calcW(92.50),	calcW(98.00),	calcW(103.83),	calcW(110.00),	calcW(116.54),
	calcW(123.47),	calcW(130.81),	calcW(138.59),	calcW(146.83),	calcW(155.56),	calcW(164.81),	calcW(174.61),	calcW(185.00),	calcW(196.00),	calcW(207.65),	calcW(220.00),	calcW(233.08),	
	calcW(246.94),	calcW(261.63),	calcW(277.18),	calcW(293.66),	calcW(311.13),	calcW(329.63),	calcW(349.23),	calcW(369.99),	calcW(392.00),	calcW(415.30),	calcW(440.00),	calcW(466.16),	
	calcW(493.88),	calcW(523.25),	calcW(554.37),	calcW(587.33),	calcW(622.25),	calcW(659.26),	calcW(698.46),	calcW(739.99),	calcW(783.99),	calcW(830.61),	calcW(880.00),	calcW(932.33),	
	calcW(987.77),	calcW(1046.50),	calcW(1108.73),	calcW(1174.66),	calcW(1244.51),	calcW(1318.51),	calcW(1396.91),	calcW(1479.98),	calcW(1567.98),	calcW(1661.22),	calcW(1760.00),	calcW(1864.66),	
	calcW(1975.53),	calcW(2093.00),	calcW(2217.46),	calcW(2349.32),	calcW(2489.02),	calcW(2637.02),	calcW(2793.83),	calcW(2959.96),	calcW(3135.96),	calcW(3322.44),	calcW(3520.00),	calcW(3729.31),	
};

//Define Note name -> note number mappings
#define B1		0
#define C2		1
#define Cs2		2
#define D2		3
#define Ds2		4
#define E2		5
#define F2		6
#define Fs2		7
#define G2		8
#define Gs2		9
#define A2		10
#define As2		11
#define B2		12
#define C3		13
#define Cs3		14
#define D3		15
#define Ds3		16
#define E3		17
#define F3		18
#define Fs3		19
#define G3		20
#define Gs3		21
#define A3		22
#define As3		23
#define B3		24
#define C4		25
#define Cs4		26
#define D4		27
#define Ds4		28
#define E4		29
#define F4		30
#define Fs4		31
#define G4		32
#define Gs4		33
#define A4		34
#define As4		35
#define B4		36
#define C5		37
#define Cs5		38
#define D5		39
#define Ds5		40
#define E5		41
#define F5		42
#define Fs5		43
#define G5		44
#define Gs5		45
#define A5		46
#define As5		47
#define B5		48
#define C6		49
#define Cs6		50
#define D6		51
#define Ds6		52
#define E6		53
#define F6		54
#define Fs6		55
#define G6		56
#define Gs6		57
#define A6		58
#define As6		59
#define B6		60
#define C7		61
#define Cs7		62
#define D7		63
#define Ds7		64
#define E7		65
#define F7		66
#define Fs7		67
#define G7		68
#define Gs7		69
#define A7		70
#define As7		71