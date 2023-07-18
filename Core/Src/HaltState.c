#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "functionDefines.h"
#include "logicDefines.h"
#include "encoder.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim6;
extern uint8_t BufferRec[];
extern char BufferTransmit[];
extern char out[100];

//CANT GET OUT OF HALT STATE WITHOUT RESTARTING MACHINE, (APP WILL RESTART TO IDLE WHEN IT GETS IDLE MSGS)
void HaltState(void)
{ 
	int sizeofPacket = 0;
	while(1)
			{				
						// and start the timer that signals when to send the data.
						if (HAL_TIM_Base_GetState(&htim7) ==  HAL_TIM_STATE_READY) // if switched off
							{
								HAL_TIM_Base_Start_IT(&htim7); // currently at 1 sec								
							}
							
						/*******MOTOR LOGIC *********/
						AllSignalVoltageLow();
						MotorDrive(DISABLE_D);
						TowerLamp(GREEN_ON);
						TowerLamp(RED_OFF);
						TowerLamp(AMBER_ON);
						if(S.errStopReason == ERR_LENGTH_REACHED)
						{
						TowerLamp(GREEN_OFF);
						TowerLamp(RED_OFF);
						TowerLamp(AMBER_OFF);
						}
						//Stop the UpdateTImer
						HAL_TIM_Base_Stop_IT(&htim6);

						/******************************/
							
						/*********COMM LOGIC CONTINUED********/
							
						if (S.updateBasePackets == 1)
						{	// keep it here because this will run once and we want this buffer packet to get created only once.
							// prepare the Stop packet Structure
						  UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_SCREEN_DATA,HMI_STOP_SCREEN,HMI_STOP_HALT,HMI_STOP_PACKET_LENGTH,3);
						  if ((S.errStopReasonHMI == ERR_LENGTH_REACHED ) || (S.errStopReasonHMI == ERR_LAPPING ))
							{ UpdateStopPacket(HMI_STOP_REASON_CODE,NO_VAR,NO_VAR);	
							}
							else{
							UpdateStopPacket(HMI_STOP_REASON_CODE,HMI_MOTOR_FAULT_CODE,HMI_ERROR_VAL_CODE);								 
							}							
							S.updateBasePackets = 0 ;							
						}
						
					//send Stop packet
						if ((U.TXcomplete ==1) && (U.TXtransfer == 1)) // TxTransfer signals when to send the data
						{
							 sizeofPacket = UpdateStopPacketString(BufferTransmit,hsb,hsp,S.errStopReasonHMI,S.errmotorFault,S.errVal);
							 HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
							 U.TXcomplete = 0; 
							 U.TXtransfer = 0;
						}
						
						if (S.logNow){
							LogMotorVariables();
							HAL_UART_Transmit(&huart1,(uint8_t*)out,73,100);
							S.logNow = 0;
							S.bufferedVars = 1;
						}

						
						//TOGGLE A LED
						LedToggle(LED2);
						HAL_Delay(1000);
						
			}
}

