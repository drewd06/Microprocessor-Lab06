/*
 * Lab06.c
 *
 * Created: 10/4/2022 9:16:57 AM
 * Initial Coder: jfhutton
 * Current Coder: Drew Dimino
 * Modified:      10/06/2026
 *
 * This lab uses hardware LEDs wired to PORTA, an interrupt from the joystick center
 * button, and a timer interrupt to build a game loop program that can display 
 * different patterns on the LEDs.
 *
 * While there are many "down and dirty" ways to get this coding done, try to 
 * remember your coding and data structures classes.  Things like ENUM, Arrays, Functions
 * could help make for more elegant coding.
 *
 */ 

#include <avr/io.h>              // Needed for AVR IO defines
#include <avr/interrupt.h>       // Needed for AVR interupt devines

#define LEDS            PORTA    // alias PORTA

	
// global variables for communication between ISRs and main
const unsigned char TCNT0_COUNT_SET = 0x8E;// Count for 1ms loop (Provided by Prof Hutton)
// <TBD> Your global variables should go here

typedef enum {Mode0,Mode1,Mode2,Mode3} Mode;
volatile Mode currMode = Mode0;
volatile int Tick = 0;

void inMode0();
void inMode1();
void inMode2();
void inMode3();

int main(void){
	
	// Variables for main
	// <TBD> Your local main variables should go here
	
	// State machine initialization.
	currMode = Mode0;
	Tick = 0;
	
	// Initialization for LEDs
	// Set direction for A ports.
	// (Prof Note:  This is similar to our two line assembly commands.)
	
	DDRA = 0xFF;  // Set the Direction for all PORTA pints to be outputs
	LEDS = 0xFF;  // Set the PORTA for all pins to be high (i.e. OFF)
	//LEDS = 0x00;  // TEST - Set all PORTA pins to be low (i.e. ON)
	
	// Initialization for Timer Interrupt
	// With a 7.3MHz crystal we have a 0.137us period
	// To build a 1ms timer "tick" we
	// - set the pre-scaler to 1/64 (8.77us)
	// - set the TCNT1 to count 114 counts (0-114=0x8E)
	// - enable the interrupt on overflow
	// (Prof Note: these are the commands you need, BUT they are commented
	//  out to start.  Be sure you fully understand them (book, datasheet, etc)
	//  before you enable them...  You also have to have the proper ISR routine
	//  ready for this to work.)
	// (Prof Note Two:  You will need to show your own calculations in the 
	//  Lab Report to verify the TCNT0_COUNT_SET value!)
	
	TCCR0 = (1<<CS02);
	TCNT0 = TCNT0_COUNT_SET;
	TIMSK = (1<<TOIE0);
	
	// Port Initialization
	// Using the LED PORTA initialization above and your Lab05 code, 
	// Configure the joystick button.
	// (Prof Note: you will need to add another jumper wire, or 
	// change the ones you have, to get the center button working!
	
	// <TBD>  Student code here
	DDRB &= ~(1 << DDB4);   // Set pin 4 as input
	PORTB |= (1 << PORTB4);  // Enable pull-up resistor
	
	// Interrupt Enable Block
	// Using your Lab05 code, you will need to update these
	// lines from 0x00 to have the appropriate mask.
	EICRA = 0x03; 
	EIMSK = 0x01;  
	// Enable Global Interrupts
	sei();
	
	// Main Loop
	while (1) {
		// Your game loop will go here.
		// <TBD>  Game Loop
		if(Tick >= 50){
			Tick = 0;
			
			 switch (currMode) {
				 case Mode0:
				 inMode0();
				 break;
				 case Mode1:
				 inMode1();
				 break;
				 case Mode2:
				 inMode2();
				 break;
				 case Mode3:
				 inMode3();
				 break;
				 default:
				 currMode = Mode0;
				 break;
			 }
			 
		}
		
	}
}

// This is the required format of an ISR routine for a 
// Timer.  Your ISR code should go inside
// (Prof Note:  When you define this correctly, the JUMP TABLE
//  will be properly updated by the compiler.  (Thank you 
ISR(TIMER0_OVF_vect){

	// <TBD>  ISR code for timer interrupt.
	TCNT0 = TCNT0_COUNT_SET;
	Tick++;
	
}

// This is the required format of an ISR routine for a
// Hardware Pin Interupt.  Format is "PIN#_vect" where 
// '#' is the number of the interupt...  PD0->0, PD1->1, etc
ISR(INT0_vect){
	
	// <TBD>  
	switch (currMode) {
		case Mode0:
		currMode = Mode1;
		break;
		case Mode1:
		currMode = Mode2;
		break;
		case Mode2:
		currMode = Mode3;
		break;
		case Mode3:
		currMode = Mode0;
		break;
		default:
		currMode = Mode0;
		break;
	}
	
}

void inMode0() {
	static unsigned char index = 0;
	LEDS = ~(1 << index);
	index = (index + 1) % 8;
}

void inMode1() {
	static unsigned char index = 7;
	LEDS = ~(1 << index);
	if (index == 0) {
		
		index = 7;
	} 
	
	else {
		index--;
	}
}

void inMode2() {
	static unsigned char index = 0;
	static int direction = 1;

	LEDS = ~(1 << index);

	if (direction == 1) {
		
		if (index == 7) {
			
			direction = -1;
			index = 6; 
			} 
		
		else {
			index++;
		}
		
	} 
		
	else {
		
		if (index == 0) {
			direction = 1;
			index = 1; 
		} 
		
		else {
		index--;
		}
		
	}
}

void inMode3() {
	static unsigned char index = 0;
	LEDS = ~(1 << index);
	index = (index + 2) % 8;
}