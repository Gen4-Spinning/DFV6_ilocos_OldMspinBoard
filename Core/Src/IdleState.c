#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "functionDefines.h"
#include "logicDefines.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim7;
extern uint8_t BufferRec[];
extern char BufferTransmit[];
extern int keyPress;
extern int buttonTimer;
extern char out[100];

void IdleState(void){
	int sizeofPacket = 0;
	while(1){
		// start the timer that signals when to send the data.
		if (HAL_TIM_Base_GetState(&htim7) ==  HAL_TIM_STATE_READY){
			HAL_TIM_Base_Start_IT(&htim7); // currently at 1 sec
		}
							
		// every sec, send a Idle screen(if you send it once, if it fails your gone)
		if( (U.TXtransfer == 1)	&& (U.TXcomplete == 1)){
			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_SCREEN_DATA,HMI_IDLE_SCREEN,NO_VAR,NO_VAR,NO_VAR);
			sizeofPacket = HMI_GetIdlePacketString(BufferTransmit,hsb);
			HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
			U.TXcomplete = 0;
			U.TXtransfer = 0;
		}
				
		if (S.state_change == TO_DIAG){
			S.current_state =  DIAGNOSTIC_STATE;
			S.prev_state = IDLE_STATE;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			S.updateBasePackets = 1;
			HAL_TIM_Base_Stop_IT(&htim7);
			break;
		}

		if (S.state_change == TO_SETTINGS){
			S.current_state =  UPDATESETTINGS;
			S.prev_state = IDLE_STATE;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			S.updateBasePackets = 1;
			HAL_TIM_Base_Stop_IT(&htim7);
			break;
		}
			
		keyPress = Pushbutton();
		if ((keyPress) && (buttonTimer > BUTTON_DEBOUNCE)){
			S.state_change = TO_RUN;
			S.current_state =  RUN_STATE;
			S.prev_state = IDLE_STATE;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			S.updateBasePackets = 1; // only when going to RUN, need a flag to set the correct state
			HAL_TIM_Base_Stop_IT(&htim7);
			buttonTimer = 0;  // make button timer 0 everytime theres a keypress
			keyPress = 0;
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

