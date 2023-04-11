#include "Driver_USART.h"               // ::CMSIS Driver:USART

extern ARM_DRIVER_USART Driver_USART1;

#include "LPC17xx.h"                    // Device header
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

void tache1(void const *argument); //prototype
osThreadId ID_tache1;
osThreadDef (tache1, osPriorityNormal, 1, 0); //1 instance,taille pile par défaut


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

int main (void){
	
	
	Init_UART();
	osKernelInitialize ();
	
	ID_tache1 = osThreadCreate ( osThread ( tache1 ), NULL ) ; //changer le nul si plusieurs instances

	
	osDelay(osWaitForever) ;
	return 0;
}


void tache1(void const *argument) {
	char tab_W[64];
	char tab_R[64];
	
//	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
//	Driver_USART1.Send(tab_W,16);
		
	Driver_USART1.Receive(tab_R,12);
	while (Driver_USART1.GetRxCount() <1) ; // on attend que 1 case soit pleine
	
	// ID tag : 0D0093641BE1 (en ASCII)
	if((tab_R[0]==0x30) & (tab_R[1]==0x44) & (tab_R[2]==0x30) & (tab_R[3]==0x30) & (tab_R[4]==0x39) & (tab_R[5]==0x33) & (tab_R[6]==0x36) & (tab_R[7]==0x34) & (tab_R[8]==0x31) & (tab_R[9]==0x42) & (tab_R[10]==0x45) & (tab_R[11]==0x31)){
		
		
	}
}
