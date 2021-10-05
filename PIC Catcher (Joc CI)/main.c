// Enric Condal Asensio, GRUP 12

// 1. El jugador es controla amb les tecles 'a' (per anar cap a l'esquerra) i 'd' (per anar a la dreta)
// 2. Clicant la tecla 'x' obtens 1 vida extra (fins a 99) però això te un preu, la teva puntuació es redueix a la meitat
// 3. El joc acaba quan et quedes amb 0 vides o quan arribes a 999 punts
// 4. Hi ha 4 nivells de dificultat, en el més alt només es pot arribar a guanyar memoritzant les posicions pseudo-aleatòries en que cauen els PICs
// 5. Es recomana anar fent ullades a la Terminal Virtual, doncs hi van apareixen missatges segons les circumstàncies del joc

// Les explicacions i aclaracions dins del codi estan majoritàriament fetes en anglès

#include <xc.h>
#include "config.h"
#include "GLCD.h"
#include "splash.h"
#include "ascii.h"
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 8000000 // Needed for __delay_ms function

// GLOBAL VARIABLES---------------------------------------------------
//Message sent when initializing the game
const char welcome_msg[] = "Welcome, player.\r My name is Enric Condal. \r I hope you'll enjoy this game. \r\rCurrent objective: \r - Grab as much PICs as you can.\r\r\n";

//Messages sent when player catches a PIC
const char catch_msg1[] = " Have you been practising? That was a magnific catch!\r\r\n";
const char catch_msg2[] = " Okey, I'm taking notes about this last move of yours\r\r\n";
const char catch_msg3[] = " Gotta catch'em all! (Nevermind, this may be copyrighted)\r\r\n";
const char catch_msg4[] = " I could use some classes. Are you free next Wednesday?\r\r\n";
const char catch_msg5[] = " I hereby pronounce you Sir Catcher, defender of the PICs\r\r\n";
const char catch_msg6[] = " Newton would be proud of how you're using his laws\r\r\n";
const char catch_msg7[] = " Should I grab myself a chair?\r\r\n";
const char catch_msg8[] = " That was amazing! You've got this!\r\r\n";
const char catch_msg9[] = " May I be in front of our Hall of Fame's next contestant\r\r\n";
int nice_catch_msg = 1;		//used to iterate through all possible messages

//Message sent when player presses 'x' (++n_hearts) for the first time
const char cheat_msg[] = "(x) Hearts increasing? I'll pretend I didn't see that...\r\r\n";
int cheat_used = 0;		//used as a boolean for the first time pressing 'x'

//Message sent when player loses a heart for the first time
const char tip[] = "Be careful. \r The Data Sheet doesn't mention anything about broken PICs. \r\r\n";
int heart_lost = 0;		//used as a boolean for the first heart lost

//Messages sent when an end of game is reached
const char win_msg[] = "The prophecy was true... You really are the chosen one.\n";
const char lose_msg[] = "You've got potential, I can see it.\n";
int end_reached = 0;	//used as a boolean for the first iteration when game has ended

//Text to show in-game
char score[] = "Score:";
char hearts[] = "Hearts:";
char good_job[] = "Good job!";
char amazing[] = "Amazing!";
char you_got[] = "You got ";
char points_msg[] = "points";
char well_played[] = "Well played!";

//Welcome screen(0), playing(1), game lost(2), game won(3)
unsigned int mode = 0;

//Difficulty (from 0 to 3)
unsigned int difficulty = 0;
int last_diff = 0;

//Hearts/Lives-related variables
int n_hearts = 3;
int hearts_updated = 0;

//Points/Score-related variables
int points = 0;
int points_updated = 0;

//Falling PIC variables
int dropping = 0;	//Used as a boolean that answers: "Is there a PIC dropping?"
int pic_y_coord = 65;
int pic_p_coord = 1;

// Player variables
int player_p_coord = 5;
int player_y_coord = 64;
int last_pl_y_coord = 64;	//to check if the player has moved or not since last checking


// THE GAME STARTS-----------------------------------------------------
void game_on() {
	PIE1bits.RCIE = 1;		//Interrupt reception enabled
	PIE1bits.TXIE = 0;
	clearGLCD(0,7,0,127);
	mode = 1;
	//Temps desitjat = n * Fcy * PRESCALER -> 1 = n * 4*(1/8*10^6) * 128
	//n = 15625
	//Timer starts from: 2^16 - 15625 = 0d49911 = 0xC2F7 
	TMR0H = 0xC2;
	TMR0L = 0xF7;
	TMR0ON = 1;
}

// RSI High Priority---------------------------------------------------
void interrupt rsi(void) {
	if (TMR0IE && TMR0IF && mode == 0) {
			TMR0IF = 0;
			TMR0ON = 0;
			TMR0IE = 1;
			game_on();
	}
	
	//Sets the rate at which PICs are dropped and drops them at that speed
	if (TMR0IE && TMR0IF && mode == 1) {
	//Temps desitjat de caiguda/pàgina = n * Fcy * PRESCALER -> r = n * 4*(1/8*10^6) * 128
	//Timer0 starts from: 2^16 - n = TMR0 value
		if (difficulty == 0) {				//Easy (1 second/page)
			T0PS2 = 1;	//Prescaler 1:128
			T0PS1 = 1;
			T0PS0 = 0;
			TMR0H = 0xC2;
			TMR0L = 0xF7;
			TMR0IF = 0;
		} 
		else if (difficulty == 1) {			//Normal, recommended (0.8 seconds/page)
			T0PS2 = 1;	//Prescaler 1:128
			T0PS1 = 1;
			T0PS0 = 0;
			TMR0H = 0xCF;
			TMR0L = 0x2C;
			TMR0IF = 0;
		} 
		else if (difficulty == 2){			//Hard (0.6 seconds/page)
			T0PS2 = 1;	//Prescaler 1:64
			T0PS1 = 0;
			T0PS0 = 0;
			TMR0H = 0xB6;
			TMR0L = 0xC2;
			TMR0IF = 0;
		} 
		else {								//Insane (0.4 seconds/page)
			T0PS2 = 1;	//Prescaler 1:64
			T0PS1 = 0;
			T0PS0 = 0;
			TMR0H = 0xCF;
			TMR0L = 0x2C;
			TMR0IF = 0;
		}
		if (pic_p_coord < 7) ++pic_p_coord;
		else {
			pic_p_coord = 1;
			pic_y_coord = rand()%126;
			dropping = 0;
		}
	}
	
	//Checks for 'a', 'd' or 'x' key pressed
	if (PIE1bits.RCIE && PIR1bits.RCIF) {
		while (PIR1bits.RCIF) {
			char rx_buffer = RCREG;
			if (rx_buffer == 'x') {			
				if (n_hearts < 99) {
					if (!cheat_used) {	//If first time pressing 'x'
						cheat_used = 1;
						int msg_length = sizeof(cheat_msg);
						for(int i = 0; i < msg_length; ++i) {
							while (!TX1IF) {};
								TXREG1 = cheat_msg[i];
						}
					}
					++n_hearts;
					hearts_updated = 1;
					//Score reduces in half when n_hearts increases
					points = points/2;
					points_updated = 1;
				}
			} 
			else if (rx_buffer == 'd') {
				if (player_y_coord < 120) ++player_y_coord;
			} 
			else if (rx_buffer == 'a') {
				if (player_y_coord > 1) --player_y_coord;
			}
		}
	}
}

// INITIALIZE PORTS AND BASIC PIC RESOURCES----------------------------
void InitPIC() {
//Ports
	PORTB = 0x00;	//Valors inicials dels ports a 0
	PORTC = 0x00;
	PORTD = 0x00;
	TRISA = 0x01;	//Pin 1 del Port A d'entrada
	TRISB = 0x00;	//Ports B de sortida (GLCD)
	TRISD = 0x00;	//Port D de sortida (GLCD)
	TRISC = 0xFF;	//Port C d'entrada (Terminal, per evitar intervencions no desitjades amb el registre LAT)
	ANSELA = 1;		//Port A analògic
	ANSELC = 0;		//Port C digital

//T0CON
	T08BIT = 0;		//Timer0 configured as an 8-bit timer
	T0CS = 0;		//Clock source is Internal instruction cycle clock
	T0SE = 0;		//Increment on low-to-high transition
	PSA = 0;		//Prescaler is assigned
	T0PS2 = 1;		//Prescaler 1:128
	T0PS1 = 1;
	T0PS0 = 0;
   
//Timer Registers
	//Temps desitjat (en s) = n * Fcy * PRESCALER -> 3 = n * 4*(1/8*10^6) * 128
	//n = 46875
	//Timer starts from: 2^16 - 46875 = 0d18661 = 0x48E5 
	TMR0H = 0x48;
	TMR0L = 0xE5;

//RCON
	IPEN = 1;		//Enable priority levels on interrupts
	
//INTCON
	TMR0IF = 0;				//TMR0 register not overflowed yet
	GIEH = 1;				//Enable all high priority interrupts
	GIEL = 0;				//Enable all low priority interrupts
	TMR0IE = 1;				//Enable the TMR0 overflow interrupt
	INTCONbits.PEIE = 1; 	//Enable Peripheral interrupt
	INTCONbits.GIE = 1;		//Enable all unmasked global interrupts

//INTCON2
	TMR0IP = 1;		//TMR0 Overflow Interrupt Priority bit set to High

//TXSTA1
	TXSTA1bits.TXEN = 1;	//Enables the transmitter circuitry of the EUSART
	TXSTA1bits.SYNC = 0;	//Configures de EUSART for asynchronous operation
	TXSTA1bits.BRGH = 1;	//High speed Baud Rate
	TXSTA1bits.TX9 = 0;		//8-bit transmission selected
	TXSTA1bits.TRMT = 0;	//Transmit Shift Register when TSR is full

//BAUDCON1
	BAUDCON1bits.BRG16 = 1; //16-bit Baud Rate Generator is used
	SPBRG = 16;				//Register used by BRG16

//RCSTA1
	RCSTA1bits.RX9 = 0;		//8-bit recepction selected
	RCSTA1bits.SPEN = 0;	//Disables the EUSART (enabled lines later)
	RCSTA1bits.CREN = 1;	//Receiver enabled
    RCSTA1bits.OERR = 1;	//Overrun error (will be cleared later)

//PIE1
	PIE1bits.RC1IE = 1;		//Enables the EUSART1 receive interrupt
	RCSTA1bits.SPEN = 1;	//Enables the EUSART and configures TXx/CKx I/O pin as serial port pins

//IPR1
	//IPR1bits.RCIP = 1;	//USART Receive Interrupt Priority bit
	//IPR1bits.TXIP = 1;	//USART Transmit Interrupt Priority bit

//ADCON0
	ADCON0bits.CHS = 0;		//Analog Channel 0 selected
	ADCON0bits.GO = 0;		//AD conversion completed/not in progress
	ADCON0bits.ADON = 1;	//ADC is enabled

//ADCON1
	ADCON1bits.TRIGSEL = 1;		//CTMU module selected
	ADCON1bits.PVCFG = 0b11;	//A/D Vref+ connected to internal signal
	ADCON1bits.NVCFG = 0b11;	//A/D Vref- connected to internal signal

//ADCON2
	ADCON2bits.ADFM = 1;		//A/D Conversion result format is right justified
	ADCON2bits.ACQT = 0b010;	//[4 TAD] Acquisition time for A/D
	ADCON2bits.ADCS = 0b001;	//[Fosc/8] Base clock for A/D (fsampling > 2*fsignalmaxfreq)
}

// INITIALIZE GLCD-----------------------------------------------------
void StartGLCD() {
	GLCDinit();		   		   // Inicialitzem la pantalla
	clearGLCD(0,7,0,127);      // Esborrem pantalla
	setStartLine(0);           // Definim linia d'inici
}

// WRITE TEXT POINTED BY s TO (page, y) COORDS-------------------------
void writeTxt(byte page, byte y, char * s) {
	int i=0;
	while (*s!='\n' && *s!='\0') {
		putchGLCD(page, y+i, *(s++));
		i++;
	};
}	

// DRAW HEARTS BASED ON THE n_hearts GLOBAL VARIABLE-------------------
void draw_hearts() {
	writeTxt(0, 13, hearts);
	//n_hearts + '0';
	if (hearts_updated) {
		clearGLCD(0,0,101,127);
		hearts_updated = 0;
	}
	if (n_hearts < 4) {
		for(int pos = 0; pos < n_hearts; ++pos) {
			for(int i = 0; i < 7; ++i) {
				writeByte(0, 101+i+pos*10, *(Heart+i));
			}
		}
	} else {	//If more than 3 hearts, the amount will be shown as "?x4"
		if (n_hearts == 4) clearGLCD(0,0,121,127);
		for(int i = 0; i < 7; ++i) {
			writeByte(0, 101+i, *(Heart+i));
		}
		putchGLCD(0, 22, 'x');
		writeNum(0, 23, n_hearts);
	}
}

// WRITE CURRENT POINTS BASED ON THE points GLOBAL VARIABLE------------
void update_score() {
	if (points_updated) {
		clearGLCD(0,0,20,90);
		points_updated = 0;
	}
	writeNum(0, 7, points);
}

// DRAW MAP AND TEXT (BOTH STATIC/NON-MOVING OBJECTS)------------------
void draw_map_and_text() {
	writeTxt(0, 0, score);
	for(int i = 0; i < 128; ++i) {
		writeByte(7, i, 0xFF);
	}
}

// DRAW MAIN CHARACTER-------------------------------------------------
void draw_player() {
	if (last_pl_y_coord != player_y_coord) {
		last_pl_y_coord = player_y_coord;
		clearGLCD(5,6,0,127);
	}
	for(int p = 0; p < 2; ++p) {
			for(int i = 0; i < 8; ++i) {
				writeByte(player_p_coord+p, player_y_coord+i, *(Rift+i+p*8));
			}
		}
}

// PLAYER HAS CAUGHT A PIC, SEND A MESSAGE THROUGH TERMINAL------------
void send_nice_catch_msg() {
	//First, points are printed
	while (!TX1IF) {};
	TXREG1 = '(';
	if (points > 99) {
		while (!TX1IF) {};
		TXREG1 = points/100 + '0';
		while (!TX1IF) {};
		TXREG1 = (points/10)%10 + '0';
		while (!TX1IF) {};
		TXREG1 = points%10 + '0';
	} else if (points > 9) {
		while (!TX1IF) {};
		TXREG1 = points/10 + '0';
		while (!TX1IF) {};
		TXREG1 = points%10 + '0';
	} else {
		while (!TX1IF) {};
		TXREG1 = points + '0';
	}
	while (!TX1IF) {};
	TXREG1 = ')';
	
	//Next, a motivational message from all the ones available is printed
	int msg_length = 0;
	if (nice_catch_msg == 1) msg_length = sizeof(catch_msg1);
	else if (nice_catch_msg == 2) msg_length = sizeof(catch_msg2);
	else if (nice_catch_msg == 3) msg_length = sizeof(catch_msg3);
	else if (nice_catch_msg == 4) msg_length = sizeof(catch_msg4);
	else if (nice_catch_msg == 5) msg_length = sizeof(catch_msg5);
	else if (nice_catch_msg == 6) msg_length = sizeof(catch_msg6);
	else if (nice_catch_msg == 7) msg_length = sizeof(catch_msg7);
	else if (nice_catch_msg == 8) msg_length = sizeof(catch_msg8);
	else if (nice_catch_msg == 9) msg_length = sizeof(catch_msg9);

	for(int i = 0; i < msg_length; ++i) {
		while (!TX1IF) {};	//checked by polling
		if (nice_catch_msg == 1) TXREG1 = catch_msg1[i];
		else if (nice_catch_msg == 2) TXREG1 = catch_msg2[i];
		else if (nice_catch_msg == 3) TXREG1 = catch_msg3[i];
		else if (nice_catch_msg == 4) TXREG1 = catch_msg4[i];
		else if (nice_catch_msg == 5) TXREG1 = catch_msg5[i];
		else if (nice_catch_msg == 6) TXREG1 = catch_msg6[i];
		else if (nice_catch_msg == 7) TXREG1 = catch_msg7[i];
		else if (nice_catch_msg == 8) TXREG1 = catch_msg8[i];
		else if (nice_catch_msg == 9) TXREG1 = catch_msg9[i];
	}
	if (nice_catch_msg == 9) nice_catch_msg = 1;
	else ++nice_catch_msg;
}

// CHECK IF A DROPPING PIC HAS TOUCHED GROUND OR HAS BEEN CAUGHT------- 
void pic_status_checker() {
	if (pic_p_coord == 6) {	//PIC touches ground, one heart less
		--n_hearts;
		if (!heart_lost) {
			heart_lost = 1;
			int msg_length = sizeof(tip);
			for(int i = 0; i < msg_length; ++i) {
				while (!TX1IF) {};	//checked by polling
				TXREG1 = tip[i];
			}
		}
		clearGLCD(pic_p_coord, pic_p_coord, pic_y_coord, pic_y_coord+5);
		hearts_updated = 1;
		dropping = 1;
		if (n_hearts == 0) {
			clearGLCD(0,7,0,127);
			mode = 2;				//Game ended by losing hearts
		}
	} else if ((pic_p_coord == 5 || pic_p_coord == 6) && (player_y_coord <= pic_y_coord+6 && player_y_coord+8 >= pic_y_coord)) {	//Player grabs PIC, one more point
		++points;
		clearGLCD(5,6,0,127);
		send_nice_catch_msg();
		points_updated = 1;
		dropping = 1;
		if (points == 999) {
			clearGLCD(0,7,0,127);  
			mode = 3;				//Game ended by winning points
		}
	}
	
}

//IF A PIC HAS BEEN DROPPED, DRAW IT AND CHECK ITS POSITION------------
void drop_pics() {
	if (!dropping) {	//If no PICs dropping, then drop one
		if (pic_p_coord > 1) clearGLCD(pic_p_coord-1, pic_p_coord, pic_y_coord, pic_y_coord+5);
		for(int i = 0; i < 6; ++i) {
				writeByte(pic_p_coord, pic_y_coord+i, *(Pic+i));
		}
		pic_status_checker();
	}
}

// CHECK POTENTIOMETER AND SET DIFFICULTY ACCORDING TO ITS VALUE-------
void set_difficulty() {
	ADCON0bits.GO = 1;		//Start of an A/D conversion cycle
	while (ADCON0bits.GO) {};	//Espera activa
	int value = ADRES;		//ADRESH*256 + ADRESL;
	if (last_diff != value) {
		last_diff = value;
		//un valor de 1023 correspon a dificultat nv. 3
		//un valor de 0 correspon a dificultat nv. 0
		//amb això en ment, he ajustat els rangs tal que:
		if (value < 256) difficulty = 0;
		else if (value < 512) difficulty = 1;
		else if (value < 1000) difficulty = 2;	
		else difficulty = 3;
	}
}

// END OF THE GAME: THE PLAYER HAS REACHED 0 HEARTS--------------------
void game_over() {
	if (!end_reached) {
		int msg_length = sizeof(lose_msg);
		for(int i = 0; i < msg_length; ++i) {
			while (!TX1IF) {};	//checked by polling
			TXREG1 = lose_msg[i];
		}
		PIE1bits.RCIE = 0;		//Interrupt reception disabled
		ADCON0bits.ADON = 0;	//ADC is disabled
		TMR0ON = 0;				//Timer0 disabled
		end_reached = 1;
	}
	writeTxt(3, 8, good_job);
	writeTxt(5, 4, you_got);
	writeNum(5, 12, points);
	writeTxt(5, 15, points_msg);
}

// END OF THE GAME: THE PLAYER HAS ACHIEVED 999 POINTS-----------------
void game_completed() {
	if (!end_reached) {
		int msg_length = sizeof(win_msg);
		for(int i = 0; i < msg_length; ++i) {
			while (!TX1IF) {};	//checked by polling
			TXREG1 = win_msg[i];
		}
		PIE1bits.RCIE = 0;		//Interrupt reception disabled
		ADCON0bits.ADON = 0;	//ADC is disabled
		TMR0ON = 0;				//Timer0 disabled
		end_reached = 1;
	}
	writeTxt(3, 9, amazing);
	writeTxt(5, 4, you_got);
	writeNum(5, 12, points);
	writeTxt(5, 16, points_msg);
}

//START OF THE GAME----------------------------------------------------
void start_game() {
			draw_map_and_text();
			draw_hearts();
			update_score();
			draw_player();
			set_difficulty();
			drop_pics();
}

// SEND WELCOME MESSAGE------------------------------------------------
void send_welcome_msg() {
	int msg_length = sizeof(welcome_msg);
	for(int i = 0; i < msg_length; ++i) {
		while (!TX1IF) {};	//checked by polling
		TXREG1 = welcome_msg[i];
	}
}

// SHOW WELCOME SCREEN-------------------------------------------------
void welcome_screen() {
	for(int p = 0; p < 8; ++p) {
		for (int y = 0; y < 128; ++y) {
			writeByte(p, y, bitmap[p*128+y]);
		}
	}
}

// MAIN PROGRAM--------------------------------------------------------
void main(void) {
	InitPIC();
	StartGLCD();
	
	//before gameplay starts, mode = 0;
	welcome_screen();
	send_welcome_msg();
	
	while (1) {	
		switch(mode) {
			case 1:		//Playing
			start_game();
			break;

			case 2:		//Game Over
			game_over();
			break;

			case 3:		//Hall of Fame
			game_completed();
			break;
		}
	}
}