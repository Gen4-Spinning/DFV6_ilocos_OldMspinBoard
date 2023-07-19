#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "eeprom.h"
#include "functionDefines.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim7;
extern uint8_t BufferRec[];
extern char BufferTransmit[];
extern char MOTORARRAY_HMI[4];
extern struct Motor M[4];

void SettingsState(void){
	int sizeofPacket = 0;
	while(1){
		if (S.state_change == TO_UPDATE_SETTINGS){
			S.state_change = TO_SETTINGS; // so that u update settings only once
			//then write the new values into the eeprom
			WriteDrawFrameSettingsIntoEeprom();
			// ACK PACKET
			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_BG_DATA,FROM_HMI_CHANGE_PROCESS_PARAMS,FROM_MC_ACK_UPDATED_SETTINGS,FROM_MC_ACK_PKT_LEN,NO_VAR);
			sizeofPacket = HMI_GetSettingsACKPacketString(BufferTransmit,hsb);
			HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
			U.TXcomplete = 0;
			U.TXtransfer = 0;
		}

		if (S.state_change == TO_UPDATE_PIDSETTINGS){
			S.state_change = TO_SETTINGS; // so that u update settings only once

			//figure out which motor we ve got settings for
			int motorIdx = 0;
			for (int i=0;i< sizeof(MOTORARRAY_HMI);i++){
				if (MOTORARRAY_HMI[i] == (char)P.updateOptionID) {
					motorIdx = i;
					break;
				}
			}
					
			if (motorIdx != 0){
				M[motorIdx].Kp = ((float)P.updateAttr1)/100.0f;
				M[motorIdx].Ki = ((float)P.updateAttr2)/100.0f;
				M[motorIdx].startOffsetOrig = P.updateAttr3;
				M[motorIdx].ff_multiplier = ((float)P.updateAttr4)/100.0f;
				//then write the new values into the eeprom
				WritePIDSettingsIntoEeprom(motorIdx);
			}
			else{// wasnt a motor Update setting, but one of the other DF start/stop var update
				if ((char)P.updateOptionID == HMI_PID_START_VARS){
					 DFs.rampUp = P.updateAttr1;
					 DFs.rampDown = P.updateAttr2;
					 //then write the new values into the eeprom
					 WriteDF_StartSettingsIntoEeprom();
				 }
			 }
						
			// ACK PACKET
			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_BG_DATA,HMI_PID_INFO,FROM_MC_ACK_SAVED_SETTINGS,FROM_MC_ACK_PKT_LEN,NO_VAR);
			sizeofPacket = HMI_GetSettingsACKPacketString(BufferTransmit,hsb);
			HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
			U.TXcomplete = 0;
			U.TXtransfer = 0;

		}

		// From settings allow to go into DIAG
		if (S.state_change == TO_DIAG){
			S.current_state =  DIAGNOSTIC_STATE;
			S.prev_state = UPDATESETTINGS;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			S.updateBasePackets = 1;
			HAL_TIM_Base_Stop_IT(&htim7);
			break;
		}
								
		if (S.state_change == TO_IDLE){
			if (S.prev_state == PAUSE_STATE){
				S.current_state = PAUSE_STATE;
			}else{
				S.current_state = IDLE_STATE;
			}
			//S.current_state =  IDLE_STATE;
			S.prev_state = UPDATESETTINGS;
			HAL_TIM_Base_Stop_IT(&htim7);
			S.updateBasePackets = 1;
			break;
			}
		}
	} // close while
}

