/*	Author: terry
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
unsigned char tmp1;
unsigned char tmp2;
enum BL_states {BL_start, BL_off, BL_on} BL_state;
void BL_tick(){
	switch(BL_state){
		case BL_start:
			BL_state = BL_off;
			break;
		case BL_off:
			BL_state = BL_on;
			break;
		case BL_on:
			BL_state = BL_off;
			break;
		default:
			BL_state = BL_start;
			break;
	}
	switch(BL_state){
		case BL_start:
			tmp1 = 0x00;
			break;	
                case BL_off:
                        tmp1 = 0x00;
                        break;
                case BL_on:
                        tmp1 = 0x08;
                        break;
                default:
                        break;
        }
}

enum TL_states {TL_start, t0, t1, t2} TL_state;
void TL_tick(){
	switch(TL_state){
		case TL_start:
			TL_state = t0;
			break;
		case t0:
			TL_state = t1;
			break;
		case t1:
			TL_state = t2;
			break;
		case t2:
			TL_state = t0;
			break;
		default:
			TL_state = TL_start;
			break;
	}
	switch(TL_state){
		case TL_start:
			tmp2 = 0x00;
			break;
		case t0:
			tmp2 = 0x01;
			break;
		case t1:
			tmp2 = 0x02;
			break;
		case t2:
			tmp2 = 0x04;
			break;
		default:
			break;
	}
}

enum output_states {output_start, output_light} output_state;
void output_tick(){
	switch(output_state){
		case output_start:
			output_state = output_light;
			break;
		case output_light:
			output_state = output_light;
			break;
		default:
			output_state = output_start;
			break;
	}
	switch(output_state){
		case output_start:
			PORTB = 0x00;
			break;
		case output_light:
			PORTB = tmp1 | tmp2;
			break;
		default:
			break;
	}
}
int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00; 
    unsigned long BL_elapsedTime = 1000;
    unsigned long TL_elapsedTime = 1000;
    unsigned long output_elapsedTime = 1000;
    const unsigned long timerPeriod = 100;
    PORTB = 0x00;
    TimerSet(timerPeriod);
    TimerOn();
    BL_state = BL_start;
    TL_state = TL_start;
    /* Insert your solution below */
    while (1) {
    	if(BL_elapsedTime >= 1000){
		BL_tick();
		BL_elapsedTime = 0;
	}
	if(TL_elapsedTime >= 1000){
                TL_tick();
                TL_elapsedTime = 0;
        }
	if(output_elapsedTime >= 1000){
                output_tick();
                output_elapsedTime = 0;
        }
	while(!TimerFlag){}
	TimerFlag = 0;
	BL_elapsedTime += timerPeriod;
	TL_elapsedTime += timerPeriod;
	output_elapsedTime += timerPeriod;
    }
    return 1;
}
