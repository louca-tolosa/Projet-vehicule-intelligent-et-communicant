#include "bibli_lidar.h"
#include "GPIO.h"
#include "stdio.h"
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD



extern ARM_DRIVER_USART Driver_USART1;


extern void uart_cb(uint32_t event);

void Init_UART_Lidar(void){
	Driver_USART1.Initialize(uart_cb);
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

//void Init_UART0_Lidar(void){
//	Driver_USART1.Initialize(uart_cb);
//	Driver_USART1.PowerControl(ARM_POWER_FULL);
//	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
//							ARM_USART_DATA_BITS_8		|
//							ARM_USART_STOP_BITS_1		|
//							ARM_USART_PARITY_NONE		|
//							ARM_USART_FLOW_CONTROL_NONE,
//							115200);
//	Driver_USART1.Control(ARM_USART_CONTROL_TX,0);
//	Driver_USART1.Control(ARM_USART_CONTROL_RX,0);
//}

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

void laser_on(void)
{
	uint8_t tab_cmd[2];
	tab_cmd[0]= 0xA5 ;
	tab_cmd[1]= 0x20 ;
	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send(tab_cmd, 2);
}

void laser_off(void)
{
	uint8_t tab_stop[2];
	tab_stop[0]= 0xA5 ;
	tab_stop[1]= 0x25 ;
	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send(tab_stop, 2);
}

void reset_lidar(void)
{
	uint8_t tab_reset[2];
	tab_reset[0]=0xA5;
	tab_reset[1]=0x40;
	while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
	Driver_USART1.Send(tab_reset, 2);
}

