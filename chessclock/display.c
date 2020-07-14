/*
 * display.c
 *
 * Created: 04/07/2020 13:46:47
 *  Author: Joshua
 */ 

#include "display.h"

volatile uint8_t displayBuffer[8];

volatile uint8_t blinkOsc[8];
volatile uint8_t blinkMask[8];

uint8_t blinkTimer;
const uint8_t blinkThreshold=24;

const uint8_t sevenSeg[37] = // TODO: PROGMEM
{
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2 etc.
	0b01001111,
	0b01100110,
	0b01101101,
	0b01111101,
	0b00000111,
	0b01111111,
	0b01100111,
	0b01110111, // A
	0b01111100, // b
	0b00111001, // C
	0b01011110, // d
	0b01111001, // E
	0b01110001, // F
	0b00111101, // G
	0b01110110, // H
	0b00010001, // i
	0b00001110, // J
	0b01110101, // k
	0b00111000, // L
	0b01010101, // m
	0b01010100, // n
	0b01011100, // o
	0b01110011, // p
	0b01100111, // q
	0b01010000, // r
	0b00101101, // S
	0b01111000, // t
	0b00011100, // u
	0b00101010, // V
	0b01101010, // W
	0b01001001, // X
	0b01101110, // y
	0b00011011, // Z
	0b01010011  // ?
};

static void tx_spi(uint8_t data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
}

void init_display(void)
{
	SPI_DDR |= 1<<SCK | 1<<MOSI | 1<<CS; 
	SPI_PORT |= 1<<CS;                  
	SPCR = 1<<SPE | 1<<MSTR | 1<<SPR1; // master mode, /64 prescaler
	
	DDRD |= 1<<PD5; // OC0B	
	
	TCCR0A = 1<<COM0B1 | 1<<WGM01 | 1<<WGM00; // Fast PWM, TOP = OCR0A
	TCCR0B = 1<<WGM02 | 1<<CS02;              // /256 prescaler
	TIMSK0 = 1<<OCIE0A;                       // compare interrupt
	OCR0A = 48;					              // ~640Hz at 8MHz
	OCR0B = 10;                               // Display brightness PWM
	
	PORTB &= ~(1<<CS); 
	tx_spi(0x0C);		// shutdown
	tx_spi(0x01);		//
	PORTB |= 1<<CS;
	
	PORTB &= ~(1<<CS); 
	tx_spi(0x0B);		// scanmode
	tx_spi(0x07);		//
	PORTB |= 1<<CS;
	
	PORTB &= ~(1<<CS); 
	tx_spi(0x0A);	    // intensity
	tx_spi(0x0B);		//
	PORTB |= 1<<CS;
	
	PORTB &= ~(1<<CS); 
	tx_spi(0x09);		// decode
	tx_spi(0xFF);		//
	PORTB |= 1<<CS;	
	
	PORTB &= ~(1<<CS); 
	tx_spi(0x0F);		// test mode
	tx_spi(0x00);		// off
	PORTB |= 1<<CS;
	
	for (uint8_t i = 0; i < 8; i++)
	{
		blinkOsc[i] = 0x00;
		blinkMask[i] = 0x00;
	}
}

void do_blink(void)
{
	blinkTimer++;
	if (blinkTimer > blinkThreshold)
	{
		blinkTimer = 0;
		for (uint8_t i = 0; i < 8; i++)
		{			
			blinkOsc[i] ^= 0x7F; // exclude DP from blink
		}
	}
}

ISR(TIMER0_COMPA_vect)
{
	static uint8_t i;
	
	PORTB &= ~(1<<CS);
	tx_spi(i+1);
	//tx_spi(displayBuffer[i] & !(blinkMask[i] & blinkOsc[i])); // for SR multiplexed display
	tx_spi(displayBuffer[i] | (blinkMask[i] & blinkOsc[i]));
	PORTB |= 1<<CS;
		
	i++;
	i &= 0x07;	
}




