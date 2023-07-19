#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "logicDefines.h"
#include "functionDefines.h"
#include "encoder.h"
#include "Initialize.h"
#include "Logger.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim7;
extern uint8_t BufferRec[];
extern char BufferTransmit[];
extern int keyPress;
extern int startFlag;
extern int pushbuttonPress;
extern int allMotorsOn;
extern int sliverBreak;
extern int sliverCut;
extern int stopSecondaryMotor;
extern int keyPress;
extern int buttonTimer;
extern char out[100];
extern float totalProduction;

void PauseState(void){
	char sizeofPacket = 0;
	while(1){
		// and start the timer that signals when to send the data.
		if (HAL_TIM_Base_GetState(&htim7) ==  HAL_TIM_STATE_READY){
			HAL_TIM_Base_Start_IT(&htim7); // currently at 1 sec
		}
							
		if (S.updateBasePackets == 1){
			// keep it here because this will run once and we want this buffer packet to get created only once.
			// also prepare the Run packet Structure
			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_SCREEN_DATA,HMI_STOP_SCREEN,HMI_STOP_PAUSE,HMI_STOP_PACKET_LENGTH,3);
			UpdateStopPacket(HMI_STOP_REASON_CODE,NO_VAR,NO_VAR);
		  S.updateBasePackets = 0;
		}
						
		//send Pause packet
		if ((U.TXcomplete ==1) && (U.TXtransfer == 1)){
			sizeofPacket = UpdateStopPacketString(BufferTransmit,hsb,hsp,S.errStopReasonHMI,S.errmotorFault,S.errVal);
			 HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
			 U.TXcomplete = 0;
			 U.TXtransfer = 0;
		}

		/******************MOTOR LOGIC*******************/
		if(S.targetProductionReached){
			TowerLamp(GREEN_OFF); // everything ON
			TowerLamp(RED_OFF);
			TowerLamp(AMBER_OFF);
		}else{
			TowerLamp(AMBER_OFF);
		}
		/*************************************************/

		//Wait for a keypress and go back to piecing mode
		if(buttonTimer >= BUTTON_DEBOUNCE){
			keyPress = Pushbutton();
		}

		sliverCut = 0;
		//TO CHANGE FOR DRAWFRAME
		if (keyPress == 1){
			S.state_change = TO_RUN;
			S.current_state =  RUN_STATE;
			S.prev_state = PAUSE_STATE;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			S.updateBasePackets = 1;
			S.errStopReason = NO_VAR;
			S.errStopReasonHMI = NO_VAR;
			S.errmotorFault = NO_VAR;
			S.errVal = NO_VAR;
			HAL_TIM_Base_Stop_IT(&htim7);

			if (S.targetProductionReached == 1){
				S.targetProductionReached = 0;
				totalProduction = 0;
			}
			ResetEncoderVariables();
			ResetMotorVariables();

			TowerLamp(AMBER_ON);
			startFlag = 1;
			allMotorsOn = 1;
			M[MOTOR1].setRpm = dsp.deliverySpeed/((3.141593f*40)/1000);
			HAL_Delay(500);

			S.updateBasePackets = 1;
			S.runMode = HMI_RUN_NORMAL;
			sliverCut = 0;
			buttonTimer = 0;
			keyPress = 0;
			break;
		}

		//allow changing settings in pause state also. sueful while tuning and changing PID without homing each time.
		if (S.state_change == TO_SETTINGS){
			S.current_state =  UPDATESETTINGS;
			S.prev_state = PAUSE_STATE;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			S.updateBasePackets = 1;
			HAL_TIM_Base_Stop_IT(&htim7);
			break;
		}

		//Check for RPM ERROR, to go into halt State
		if (E.RpmErrorFlag == 1){
			S.state_change = TO_HALT;
			S.current_state = HALT_STATE;
			S.prev_state = RUN_STATE;
			S.updateBasePackets = 1;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			S.updateBasePackets = 1;
			HAL_TIM_Base_Stop_IT(&htim7);
			break;
		}

		if (S.logNow){
			LogMotorVariables();
			HAL_UART_Transmit(&huart1,(uint8_t*)out,73,100);
			S.logNow = 0;
			S.bufferedVars = 1;
		}

	}
}

