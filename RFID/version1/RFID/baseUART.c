#include "Driver_USART.h"               // ::CMSIS Driver:USART

extern ARM_DRIVER_USART Driver_USART1;

void Init_UART(void){
	Driver_USART1.Initialize(NULL);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							115200);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}

int main (void){
	uint8_t tab[50];
	Init_UART();

	while (1){
		while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART1.Send("\n\rHello World!!!",16);
		
		Driver_USART1.Receive(tab,12);
		// récupération de la chaine qui se complète au fur et à mesure
		Driver_USART1.Receive(tab,50); // la fonction remplira jusqu'à 50 cases
		while (Driver_USART1.GetRxCount() <1 ) ; // on attend que 1 case soit pleine
	}	
	return 0;
}
