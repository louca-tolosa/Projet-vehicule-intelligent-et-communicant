#include "bibli_lidar.h"
#include "GPIO.h"
#include "stdio.h"

void pwm(int dc)
{
	LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040;       // enable PWM1

			
	LPC_PWM1 -> MCR |= (2<<0);			// Compteur relancé quand MR0 repasse à 0
	
	LPC_PWM1->PR = 0; // prescaler
	LPC_PWM1->MR0 = 999; // freqs du PWM vaut 25kHz
	
	
	LPC_PINCON->PINSEL4 = LPC_PINCON->PINSEL4| 0x00000010;	// Validation des sorties PWM1.3 voir la table 84
	LPC_PWM1->LER |= 0x0000000F;
	LPC_PWM1->PCR = LPC_PWM1->PCR | 0x00000800;	//Autorise la sortie PWM3 bit 11 – voir table 452
	
	LPC_PWM1->MR3 = 10*dc;      //On règle le duty cycle à dc% de PMW1.3 sur la broche P2.2
	
	LPC_PWM1 -> TCR = 1;
}