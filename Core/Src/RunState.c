#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "functionDefines.h"
#include "logicDefines.h"
#include "encoder.h"
#include "Initialize.h"
#include "Logger.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim7;
extern uint8_t BufferRec[];
extern char BufferTransmit[];
//RUN LOGIC variables
extern int allMotorsOn;
extern int pushbuttonPress;
extern int stopSecondaryMotor;
extern int keyPress;

extern int Cap3;
extern int Rpm3;
extern int filter3;
extern int startFlag;
extern float totalProduction;
extern float actualDeliverySpeed;
extern int buttonTimer;

int sliverCut = 0 ;
int sliverBreak = 0;
int lapping = 0;
int sliverCut1 = 0;
extern char out[100];
void RunState(void){
	char sizeofPacket = 0;
	while(1){
		/******************COMMUNICATION **************/
		if (S.updateBasePackets == 1){
			//Setup the HMI Run packet also inside  since it only happens onetime
			S.runMode = HMI_RUN_NORMAL;
			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_SCREEN_DATA,HMI_RUN_SCREEN,S.runMode,HMI_RUN_PACKET_LENGTH,3);
			UpdateRunPacketDF(HMI_PRODUCTION_M_MIN,dsp.deliverySpeed,HMI_PRODUCTION_TOTAL,0.1f,HMI_EMPTY_SPACE,0);
			S.updateBasePackets  = 0;
		}

		// and start the timer that signals when to send the data.
		if (HAL_TIM_Base_GetState(&htim7) ==  HAL_TIM_STATE_READY){
			HAL_TIM_Base_Start_IT(&htim7); // currently at 1 sec
		}
										
		//send Run packet
		if ((U.TXcomplete ==1) && (U.TXtransfer == 1)){
			 sizeofPacket =  UpdateRunPacketStringDF(BufferTransmit,hsb,hrp,actualDeliverySpeed,totalProduction);
			 HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
			 U.TXcomplete = 0;
			 U.TXtransfer = 0;
			}
							
		/***********MAIN MOTOR LOGIC************************/
		if (S.firstSwitchon == 1){
			// Switch on all motors
			ResetMotorVariables();
			ResetEncoderVariables();

			//Lights
			TowerLamp(GREEN_ON);
			TowerLamp(AMBER_OFF);
					
			// other variables
			allMotorsOn = 1; // BOTH MOTORS START
			startFlag = 1;
			sliverCut = 0;

			S.firstSwitchon = 0;
			S.runMode = HMI_RUN_NORMAL; // only one state for DF
		}
											
		TowerLamp(GREEN_OFF);
					
		//************SLIVER CUT SECTION
		sliverCut  = InputSensor1();
		//sliverCut = 0;
		//*******************************
		lapping =  LappingSensor();
		lapping = 0;
		if (lapping == 1){//go to halt
			E.RpmErrorFlag = 1;
			S.errStopReason = ERR_LAPPING;
			S.errStopReasonHMI	 = ERR_LAPPING;
			S.errmotorFault = NO_VAR;
			S.errVal = NO_FLOAT_VAR;
		}
					
		//Check for User KeyPress to take machine into Pause State
		if(buttonTimer >= BUTTON_DEBOUNCE){
			keyPress = Pushbutton();
		}

		//CHECK FOR COMPLETE LENGTH LIMIT
		if (totalProduction > dsp.lengthLimit){
			S.targetProductionReached = 1;
		}


		if ((keyPress==1) || (sliverCut == 1) || (S.targetProductionReached == 1)){
			S.state_change = TO_PAUSE;
			S.current_state =  PAUSE_STATE;
			S.prev_state = RUN_STATE;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			if (sliverCut == 0){
				S.errStopReason = ERR_USER_PAUSE; //not used
				S.errStopReasonHMI = ERR_USER_PAUSE;
				S.errmotorFault = NO_VAR;
				S.errVal = NO_FLOAT_VAR;
			}else if (S.targetProductionReached == 1){
				S.errStopReason = ERR_LENGTH_REACHED;
				S.errStopReasonHMI	 = ERR_LENGTH_REACHED;
				S.errmotorFault = NO_VAR;
				S.errVal = NO_FLOAT_VAR;
			}
			else{
				S.errStopReason = ERR_SLIVER_CUT_ERROR;
				S.errStopReasonHMI = ERR_SLIVER_CUT_ERROR;
				S.errmotorFault = NO_VAR;
				S.errVal = NO_FLOAT_VAR;
			}
			HAL_TIM_Base_Stop_IT(&htim7);
			//update variables for runState changing
			startFlag = 0;
			buttonTimer = 0;
			S.updateBasePackets = 1;
			keyPress = 0;
			break;
		}
					
		/****************************************************/
							
		//Check for RPM ERROR, to go into halt State
		if (E.RpmErrorFlag == 1){
			S.state_change = TO_HALT;
			S.current_state = HALT_STATE;
			S.prev_state = RUN_STATE;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			HAL_TIM_Base_Stop_IT(&htim7);
			S.updateBasePackets = 1;
			break;
		}

		if (S.logNow){
			LogMotorVariables();
			HAL_UART_Transmit(&huart1,(uint8_t*)out,73,100);
			S.logNow = 0;
			S.bufferedVars = 1;
		}
	} // closes while
}



