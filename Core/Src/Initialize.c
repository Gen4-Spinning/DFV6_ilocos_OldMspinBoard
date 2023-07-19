#include "Structs.h"
#include "CommonConstants.h"
#include "Initialize.h"
#include "logicDefines.h"
#include "functionDefines.h"
#include "HMI_Constants.h"
#include "CommonConstants.h"
#include <string.h>
//Load the structs here and make them extern in the file you want.


void InitializePIDUpdateStruct(){
P.requestedPIDoptionIdx = 0;
P.updateAttr1 = 0;
P.updateAttr2 = 0;
P.updateAttr3 = 0;
P.updateOptionID = 0;
P.updatePIDVals = 0;	
}

void InitializeDF_StartStop_Struct(){
DFs.rampUp = 0;
DFs.rampDown = 0;
}

void InitializeStateStruct()
{
	S.current_state = IDLE_STATE;
	S.prev_state = OFF_STATE;
	S.state_change = 0;
	S.first_enter = 1;
	S.oneTimeflag = 0;
	S.errStopReason = NO_VAR;
	S.errmotorFault = NO_VAR;
	S.errStopReasonHMI = NO_VAR;
	S.errVal = NO_VAR;
	S.firstSwitchon = 1;
	S.runMode = HMI_RUN_NORMAL;
	S.updateBasePackets = 0;
	S.oneSecTimer = 0;
	S.keyState = ENABLE_D;
	S.loggingEnabled = 0;
	S.logType = 0;
	S.targetProductionReached = 0;
}

void InitializeDrawFrameSettings()
{
    dsp.deliverySpeed = 250;
    dsp.tensionDraft = 5.0;
    dsp.lengthLimit = 1245;
}

void InitializeUartStruct()
{
	U.TXcomplete =0;
	U.TXtransfer =0;
}
void InitializeDiagnosticsStruct()
{
	D.motorID = 0;
	D.motorID = 0;
	D.targetSignal=0;
	D.testTime = 0;
	D.typeofTest = 0;
	
}


//Can only run Once at the beginning.Cant run elseWHERE! cos the KP Ki kd 
//start offset values from the eeprom will get overwritten
void MotorStructInit(void)	
{
		strcpy(M[MOTOR1].name,"___FR") ;
		M[MOTOR1].presentRpm = 0;
		M[MOTOR1].setRpm = dsp.deliverySpeed/((3.141593f*40)/1000);
		M[MOTOR1].rampRpm = 20;
		M[MOTOR1].piecingRpm = 300;
		M[MOTOR1].error = 0;
		M[MOTOR1].integralError = 0;
		M[MOTOR1].intTarget = 0;
		M[MOTOR1].ff_multiplier = 0;
		M[MOTOR1].feedforward = 0;
		M[MOTOR1].pwm = 180;
		M[MOTOR1].Ki = 0.45;
		M[MOTOR1].Kd = 0;
		M[MOTOR1].Kp = 0.35;
		M[MOTOR1].startOffset = 0;
		M[MOTOR1].startOffsetOrig = 0;
		M[MOTOR1].steadyState = 0;
	 	M[MOTOR1].overloadDelta = 100; // delta rpm of 300
		M[MOTOR1].overloadTime = 50; // for 5 sec at 0.1 sec interrupt
		M[MOTOR1].overloadCount = 0;
		M[MOTOR1].antiWindup = 0;
		M[MOTOR1].calcPwm = 0;
		
		strcpy(M[MOTOR2].name,"___BR") ;
		M[MOTOR2].presentRpm = 0;
		M[MOTOR2].setRpm = getBR_target(M[MOTOR1].setRpm);
		M[MOTOR2].rampRpm = (M[MOTOR1].rampRpm/dsp.tensionDraft)*15.5f;
		M[MOTOR2].piecingRpm = 300;
		M[MOTOR2].error = 0;
		M[MOTOR2].integralError = 0;
		M[MOTOR2].intTarget = 0;
		M[MOTOR2].ff_multiplier = 0;
		M[MOTOR2].feedforward = 0;
		M[MOTOR2].pwm = 0;
		M[MOTOR2].Ki = 0.45; //0.85 changed in Philly1
		M[MOTOR2].Kd = 0;
		M[MOTOR2].Kp = 0.35; //Changed from 0.17 in Philly 1
		M[MOTOR2].startOffset = 0;
		M[MOTOR2].startOffsetOrig = 0;
		M[MOTOR2].steadyState = 0;
	    M[MOTOR2].overloadDelta = 100; // delta rpm of 300
		M[MOTOR2].overloadTime = 50; // for 5 sec at 0.1 sec interrupt
		M[MOTOR2].overloadCount = 0;
		M[MOTOR2].antiWindup = 0;
		M[MOTOR2].calcPwm = 0;

		strcpy(M[MOTOR3].name,"CREEL") ;
		M[MOTOR3].presentRpm = 0;
		M[MOTOR3].setRpm = (M[MOTOR2].setRpm*0.529568f);
		M[MOTOR3].rampRpm = (M[MOTOR2].rampRpm*0.51948f);
		M[MOTOR3].piecingRpm = 300;
		M[MOTOR3].error = 0;
		M[MOTOR3].ff_multiplier = 0;
		M[MOTOR3].feedforward = 0;
		M[MOTOR3].integralError = 0;
		M[MOTOR3].intTarget = 0;
		M[MOTOR3].pwm = 0;
		M[MOTOR3].Ki = 0.85;
		M[MOTOR3].Kd = 0;
		M[MOTOR3].Kp = 0.17;
		M[MOTOR3].startOffset = 0;
		M[MOTOR3].startOffsetOrig = 0;
		M[MOTOR3].steadyState = 0;
		M[MOTOR3].overloadDelta = 100; // delta rpm of 300
		M[MOTOR3].overloadTime = 50; // for 5 sec at 0.1 sec interrupt
		M[MOTOR3].overloadCount = 0;
		M[MOTOR3].antiWindup = 0;
		M[MOTOR3].calcPwm = 0;
}
