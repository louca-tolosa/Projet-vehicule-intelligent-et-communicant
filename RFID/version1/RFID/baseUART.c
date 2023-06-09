#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include <LPC17xx.h>   
#include "GPIO.h"
#include <time.h>

extern ARM_DRIVER_USART Driver_USART1;

unsigned char buffer[64];       // buffer array for data receive over serial port
int count = 0;                    // counter for buffer array

void delay(int milliseconds);

#define LED P2_0     // Led is connected to P2.0
#define Num_LED 1

void Init_UART(void){
	Driver_USART1.Initialize(NULL);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							9600);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}

void delay(int milliseconds)
{
    clock_t start_time = clock();
    while (clock() < start_time + milliseconds);
}


int main (void){
	uint8_t tab[50];
	Init_UART();
	Initialise_GPIO();

	while (1){
		//envoi
		while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART1.Send("\n\rHello World!!!",16);
		
		// reception, récupération de la chaine qui se complète au fur et à mesure
		Driver_USART1.Receive(tab,50); // la fonction remplira jusqu'à 50 cases
		while (Driver_USART1.GetRxCount() <1 ) // on attend que 1 case soit pleine
		
		//if(){ //carte détectée
			//allumer LED
			
        /* Turn On all the leds and wait for 100ms */ 
        Allumer_1LED(Num_LED);     // Make all the Port pin as high  
        delay(100);

        Eteindre_1LED(Num_LED);     // Make all the Port pin as low  
        delay(100);
    
		//}
	}	
	return 0;
}
