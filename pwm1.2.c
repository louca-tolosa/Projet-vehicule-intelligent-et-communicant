/* -----------------------------------------------------------------------------
 * Copyright (c) 2022-2023 
 * 
 * 
 * 
 * $Date:        14. March 2023
 * $Revision:    V1.0
 *
 * Project:      PWM for Vehicle 
 * -------------------------------------------------------------------------- */

#include "LPC17xx.h"                    // Device header
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
  

int level=0;

void mouvement(void);
void direction(void);

void TIMER0_IRQHandler (void);

int main (void)
{

	mouvement();
	direction();
	
	NVIC_SetPriority(TIMER0_IRQn,0);
	NVIC_EnableIRQ(TIMER0_IRQn);		

	while(1)
	{
				
	}
}


void mouvement(void)
{
	LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040; 				// Enable PWM1
	
	LPC_PWM1->MCR = LPC_PWM1->MCR | 0x00000002;					// Compteur relancé quand MR0 repasse à 0
	
	LPC_PWM1->PR = 0; 																	// Prescaler
	LPC_PWM1->MR0 = 1249; 															// MR0
	
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7| 3<<18; 	// Validation des sorties PWM 1.2 
	
	LPC_PWM1->LER = LPC_PWM1->LER | 0x0000000F;					// Compteurs des PWMs relancés quand MR passe à 0 "bit 0 = MR0" 

	LPC_PWM1->PCR = LPC_PWM1->PCR | 1<<10; 							// Autorise les sorties PWM/2 bits 10 
	
	LPC_PWM1->MR2 =600;																	// Choix du rapport cyclique

	LPC_PWM1->TCR = 1; 																	// Validation de timer et reset counter 
	
	LPC_GPIO0->FIODIR = LPC_GPIO0->FIODIR | (1<<16)| (1<<17)| (1<<18)| (1<<19);

	LPC_GPIO0->FIOPIN = LPC_GPIO0->FIOPIN | (0<<16)| (1<<17)| (1<<18)| (1<<19); //Reculer
	LPC_GPIO0->FIOPIN = LPC_GPIO0->FIOPIN | (1<<16)| (0<<17)| (1<<18)| (1<<19); //Avancer
}

void direction(void)
{
	LPC_GPIO3->FIODIR = LPC_GPIO3->FIODIR | (1<<26);
	
	LPC_TIM0->PR=0;
	LPC_TIM0->MR0=25000-1;
	
	LPC_TIM0->MCR = LPC_TIM0->MCR | (3<<0);
	
	LPC_TIM0->TCR = 1;
}

void TIMER0_IRQHandler (void)
{
	LPC_TIM0->IR= LPC_TIM0->IR| (1<<0);								 	// Baisse flag
	
	LPC_GPIO3->FIOPIN = LPC_GPIO3->FIOPIN ^ (1<<26);		// sortie P3.25
	
	/*if(level==1)
	{
		LPC_TIM0->MR0=474925-1;
		level=0;
	}
	else
	{
		LPC_TIM0->MR0=25000-1;
		level=1;
	}	*/																								// A droite
	
	if(level==1)
	{
		LPC_TIM0->MR0=462688-1;
		level=0;
	}
	else
	{
		LPC_TIM0->MR0=37500-1;
		level=1;
	}																										// A gauche
}
