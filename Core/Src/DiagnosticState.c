#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "eeprom.h"
#include "encoder.h"
#include "functionDefines.h"
#include "logicDefines.h"
#include "main.h" 
#include "Logger.h"
#include <stdlib.h>

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim7;
extern uint8_t BufferRec[];
extern char BufferTransmit[];

//logic varaibles for diagnostic test
extern char MOTORARRAY[4];
extern char MOTORARRAY_HMI[4];

//global variables for testing defined here
int testMode = 0;
long diag_pwm = 0;
int idxMotor_diag = 0;	
int current_pwm_percent = 0;
char compareMotorID = 0;
char finalStatus = 0;
int finalErrorDiag = 0; 
extern uint8_t use_50ms_timer ;
extern char out[100];

void DiagnosticsState(void){
	int sizeofPacket = 0;

	while(1){
		//start the timer
		if (HAL_TIM_Base_GetState(&htim7) ==  HAL_TIM_STATE_READY){
			HAL_TIM_Base_Start_IT(&htim7); // currently at 1 sec
		}
						
		// if youve got the signal that youve got the test vals from the HMI
		if (S.state_change == RUN_DIAG_TEST){
			S.oneSecTimer = 0; //Reset the one sec timer to keep track of how long the test should run
			S.state_change = TO_DIAG; // so that we dont keep restarting the timer
			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_BG_DATA,HMI_DIAG_RESULTS,NO_VAR,19,4);// set the HMIbasepacket values
					
			// start the test mode after setting the variables to the correct vals
			for (int i=0;i< sizeof(MOTORARRAY_HMI);i++){
				compareMotorID = (char) D.motorID;
				if (MOTORARRAY_HMI[i] == compareMotorID){
					idxMotor_diag = i;
					break;
				}
			}
			diag_pwm =  (MAX_PWM * D.targetSignal)/100;//set the open loop val
			M[idxMotor_diag].setRpm = D.targetRPM;//set the target RPM
			testMode = 1;
		}

		if (testMode == 1){
			//when the onesectimergoes more than the test time,stop the motor
			if (S.oneSecTimer > D.testTime){
				if (D.typeofTest == HMI_DIAG_OPEN_LOOP){
					finalStatus = HMI_DIAG_TEST_SUCCESSFUL;
				}
				else{
					finalErrorDiag = M[idxMotor_diag].presentRpm - D.targetRPM ;
					// send test over mesg, send a rpi msg, reset the motor variables to their defaults.
					if (abs(finalErrorDiag) <= 50) {
						finalStatus = HMI_DIAG_TEST_SUCCESSFUL;
					}else{
						finalStatus = HMI_DIAG_TEST_FAIL;
					}
				}
							
				testMode = 0;
				/*****QUICK SHUTDOWN**********/
				AllSignalVoltageLow(); // switch off all motors.
				ResetMotorVariables(); // put back original values
				ResetEncoderVariables();//reset encoder vals
				// send test over mesg, send a rpi msg, reset the motor variables to their defaults.
				sizeofPacket = HMI_Get_DiagOver_PacketString(BufferTransmit,hsb,finalStatus);
						
				HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
				U.TXcomplete = 0;
				U.TXtransfer = 0;
			
				//Reset the Diag Packets
				idxMotor_diag = 0;
				diag_pwm = 0;
			}
			else{
				// send the HMI DIAG MSG
				current_pwm_percent = ((M[idxMotor_diag].pwm * 100)/MAX_PWM);
				sizeofPacket = UpdateDiagPacketString(BufferTransmit,hsb,hdp,D.typeofTest,D.motorID,current_pwm_percent,M[idxMotor_diag].presentRpm);

				if ((U.TXcomplete ==1) && (U.TXtransfer == 1)){ // TxTransfer signals when to send the data
					HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
					U.TXcomplete = 0;
					U.TXtransfer = 0;
					}
				}
			}//if testmode == 1
				
			if (S.state_change == TO_DIAG_SWITCH_OFF){
				// stop the test
				testMode = 0;
				/*****QUICK SHUTDOWN**********/
				AllSignalVoltageLow(); // switch off all motors.
				ResetMotorVariables(); // put back original values
				ResetEncoderVariables(); // Reset encoder vals
				//Reset the Diag Packets
				idxMotor_diag = 0;
				diag_pwm = 0;
			}
						
			if (S.state_change == TO_IDLE){
				S.current_state =  IDLE_STATE;
				S.prev_state = DIAGNOSTIC_STATE;
				S.first_enter = 1; // ALLOW THE RPI TO GET A MESG WHEN U GO BACK FROM DIAGNOSTICS
				S.oneTimeflag = 0;
				HAL_TIM_Base_Stop_IT(&htim7);
				U.TXcomplete = 1;
				//Reset incase youve found an error!
				E.RpmErrorFlag = 0;
				break;
			}

			if (E.RpmErrorFlag == 1){
				//switch of the test
				testMode = 0;
				AllSignalVoltageLow(); // switch off all motors.
				ResetMotorVariables(); // put back original values

				finalStatus = HMI_DIAG_TEST_MOTORERROR;
				sizeofPacket = HMI_Get_DiagOver_PacketString(BufferTransmit,hsb,finalStatus);
				if ((U.TXcomplete ==1) && (U.TXtransfer == 1)){
					HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
					U.TXcomplete = 0;
					U.TXtransfer = 0;
				}
								
				//Reset the Diag Packets
				idxMotor_diag = 0;
				diag_pwm = 0;
			}

			if (S.logNow){
				LogMotorVariables();
				HAL_UART_Transmit(&huart1,(uint8_t*)out,73,100);
				S.logNow = 0;
				S.bufferedVars = 1;
			}
	} //closes while
}


