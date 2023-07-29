
#include "main.h"
#include "stm32f4xx_hal.h"
#include "logicDefines.h"
#include "encoder.h" 
#include "Initialize.h"
#include "functionDefines.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include <stdlib.h>
#include <math.h>

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

extern int filter1;
extern char MOTORARRAY[];
extern char MOTORARRAY_HMI[];

extern int startFlag;

void AllTimerOn(void)
{
		MotorTimer(MOTOR1_TIMER_ON);
		MotorTimer(MOTOR2_TIMER_ON);
		MotorTimer(MOTOR3_TIMER_ON);
		MotorTimer(MOTOR4_TIMER_ON);
		MotorTimer(MOTOR5_TIMER_ON);
		MotorTimer(MOTOR6_TIMER_ON);
		MotorTimer(MOTOR7_TIMER_ON);
}

//not needed in DrawFrame since we re not changing startOffset like in flyer
void ResetMotorVariables(void)
{				//WE dont want to reset the Kp,Ki startOFfset 
	
				M[MOTOR1].pwm = 0	;
				M[MOTOR2].pwm = 0;
				M[MOTOR3].pwm = 0;
				
				M[MOTOR1].intTarget = 0;
				M[MOTOR2].intTarget = 0;
				M[MOTOR3].intTarget = 0;			

				M[MOTOR1].error = 0;
				M[MOTOR2].error = 0;
				M[MOTOR3].error = 0;

				M[MOTOR1].preError = 0;
				M[MOTOR2].preError = 0;
				M[MOTOR3].preError = 0;

			
				M[MOTOR1].integralError = 0;
				M[MOTOR2].integralError = 0;
				M[MOTOR3].integralError = 0;

}


void updateTargets(uint8_t motorIndex,uint8_t rampUp)
{
	M[motorIndex].presentRpm = getRPM(motorIndex);
	if(rampUp == 1){
		if (M[motorIndex].intTarget != M[motorIndex].setRpm){
			M[motorIndex].intTarget = M[motorIndex].intTarget + M[motorIndex].rampRpm;
			if (M[motorIndex].intTarget >  M[motorIndex].setRpm){
					M[motorIndex].intTarget =  M[motorIndex].setRpm;
				}
		}
	}

	if(rampUp == 0){
		if (M[motorIndex].intTarget != 0){
			M[motorIndex].intTarget = M[motorIndex].intTarget - M[motorIndex].rampRpm;
			if (M[motorIndex].intTarget <=  M[motorIndex].rampRpm){
					M[motorIndex].intTarget =  0;
				}
		}
	}
}


void followTargets(uint8_t motorIndex)
{
	// needs to be here so that when we run diagnostics we can print these values.
	M[motorIndex].presentRpm = getRPM(motorIndex);
	M[motorIndex].error = M[motorIndex].intTarget - M[motorIndex].presentRpm;
	if (fabs(M[motorIndex].error) >= 300) // if the error is large
		{ M[motorIndex].overloadCount++;
			if(M[motorIndex].overloadCount >= 20){
				E.RpmErrorFlag = 1;
				S.errStopReason = MOTORARRAY[motorIndex];
				S.errStopReasonHMI = MOTORARRAY_HMI[motorIndex];
				S.errmotorFault = ERR_RPM_ERROR;
				S.errVal = NO_FLOAT_VAR;
				M[motorIndex].overloadCount = 0;
			}
		}else{
			M[motorIndex].overloadCount = 0;
		}
	if (M[motorIndex].antiWindup == 0){
		M[motorIndex].integralError = M[motorIndex].integralError + M[motorIndex].error;
	}


	M[motorIndex].calcPwm = M[motorIndex].Kp*M[motorIndex].error + M[motorIndex].Ki*M[motorIndex].integralError;
	M[motorIndex].feedforward = (uint16_t)(M[motorIndex].intTarget*M[motorIndex].ff_multiplier) ;
	M[motorIndex].pwm = M[motorIndex].startOffset + M[motorIndex].calcPwm + M[motorIndex].feedforward;

	if (M[motorIndex].pwm > MAX_PWM){
		M[motorIndex].antiWindup = 1;
		M[motorIndex].pwm  = MAX_PWM;
	}else{
		M[motorIndex].antiWindup = 0;
	}

	if (M[motorIndex].pwm < M[motorIndex].startOffset){
		M[motorIndex].pwm = M[motorIndex].startOffset;
	}
}


void UpdateMotorPID(uint8_t motorIndex)
{
	unsigned int calcPwm = 0;
	M[motorIndex].presentRpm = FilterRpm(motorIndex);

	if((startFlag == 1) && (M[motorIndex].intTarget <= M[motorIndex].setRpm))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget + M[motorIndex].rampRpm;
	}
	
	if((startFlag == 1) && (M[motorIndex].intTarget > M[motorIndex].setRpm))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget - M[motorIndex].rampRpm;
	}
	
	if((startFlag == 0)&&((M[motorIndex].intTarget > 0)))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget - M[motorIndex].rampRpm;
	}
	M[motorIndex].error = M[motorIndex].intTarget - M[motorIndex].presentRpm;
//	M[motorIndex].error = M[motorIndex].intTarget - filter1;
	
	/*if (__fabs(M[motorIndex].error) >= 300)
	{
		E.RpmErrorFlag = 1;
		S.errStopReason = MOTORARRAY[motorIndex];
		S.errStopReasonHMI = MOTORARRAY_HMI[motorIndex];
		S.errmotorFault = ERR_RPM_ERROR;	
		S.errVal = NO_FLOAT_VAR;
	}*/
	
	//0.1 sec per call, so 10 per sec, and 50 in 5 sec
	if (M[motorIndex].steadyState == 1){  // only look for error when your in the steadystate region 
		if (abs(M[motorIndex].error) >= M[motorIndex].overloadDelta){
						M[motorIndex].overloadCount ++ ; // only this is an array, maybe we can put this in the motor struct also
						if (M[motorIndex].overloadCount  > 20){
							E.RpmErrorFlag = 1;
							S.errStopReason = MOTORARRAY[motorIndex];
							S.errmotorFault = ERR_RPM_ERROR;	
							S.errVal = NO_FLOAT_VAR;
							M[motorIndex].overloadCount  = 0; // you can set it to zero because E.rpm error has been set
						}
					}
				else{
				M[motorIndex].overloadCount  = 0;
			}
		}
	//NO CORRECTION DURING RAMPUP RAMP DOWN
		
	M[motorIndex].integralError = (M[motorIndex].integralError + M[motorIndex].error);
	
	calcPwm = M[motorIndex].Kp*M[motorIndex].error + M[motorIndex].Ki*M[motorIndex].integralError;
	
	M[motorIndex].preError = M[motorIndex].error;
	if (calcPwm <= MAX_PWM)
	{
		M[motorIndex].pwm = calcPwm;
	}
}

void UpdateMotorPID_Diag(uint8_t motorIndex)
{
	unsigned int calcPwm = 0;
	int rampRPM = 20;
	M[motorIndex].presentRpm = getRPM(motorIndex);

	if((startFlag == 1) && (M[motorIndex].intTarget <= M[motorIndex].setRpm))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget + rampRPM;
		if (M[motorIndex].intTarget > M[motorIndex].setRpm){
			M[motorIndex].intTarget = M[motorIndex].setRpm;
		}
	}
	
	//not used in Diag
	if((startFlag == 0)&&((M[motorIndex].intTarget > 0)))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget - M[motorIndex].rampRpm;
	}
	
	M[motorIndex].error = M[motorIndex].intTarget - M[motorIndex].presentRpm;
	
	if (abs(M[motorIndex].error) >= 300)
	{
		E.RpmErrorFlag = 1;
		S.errStopReason = MOTORARRAY[motorIndex];
		S.errStopReasonHMI = MOTORARRAY_HMI[motorIndex];
		S.errmotorFault = ERR_RPM_ERROR;	
		S.errVal = NO_FLOAT_VAR;
	}
			
	M[motorIndex].integralError = (M[motorIndex].integralError + M[motorIndex].error);
	
	calcPwm = M[motorIndex].Kp*M[motorIndex].error + M[motorIndex].Ki*M[motorIndex].integralError;
	
	M[motorIndex].preError = M[motorIndex].error;
	if (calcPwm <= MAX_PWM)
	{
		M[motorIndex].pwm = calcPwm;
	}
}


void UpdateMotorVoltage(uint8_t motorIndex,int pwmValue)
{
	char deltaPwm;
/*	if(startFlag == 1)
	{
		deltaPwm = 10;
	}*/
	deltaPwm = pwmValue;
	
	if(startFlag == 0)
	{
		deltaPwm = 20;
	}
	M[motorIndex].presentRpm = FilterRpm(motorIndex);
	if (M[motorIndex].pwm < pwmValue)
	{
		if (M[motorIndex].pwm < MAX_PWM)
		{
				M[motorIndex].pwm = M[motorIndex].pwm + deltaPwm;
		}
	}
	if (M[motorIndex].pwm > pwmValue)
	{
		if (M[motorIndex].pwm > MIN_PWM)
		{
			M[motorIndex].pwm = M[motorIndex].pwm - deltaPwm;
		}
	}
}

//for diag
void UpdateMotorVoltageDiag(uint8_t motorIndex,int pwmValue)
{
	char deltaPwm;

	deltaPwm = 10;
	
	M[motorIndex].presentRpm = getRPM(motorIndex);
	if (M[motorIndex].pwm < pwmValue)
	{
		if (M[motorIndex].pwm < MAX_PWM)
		{
				M[motorIndex].pwm = M[motorIndex].pwm + deltaPwm;
		}
	}
	if (M[motorIndex].pwm > pwmValue)
	{
		if (M[motorIndex].pwm > MIN_PWM)
		{
			M[motorIndex].pwm = M[motorIndex].pwm - deltaPwm;
		}
	}
}


//this machine has ramp down also. so keep checking
void CheckSteadyStateReached(uint8_t motorIndex){
		if (abs(M[motorIndex].intTarget  -  M[motorIndex].setRpm)< 50 ){  // LAter fix the red line near the abs
				M[motorIndex].steadyState = 1;
			}
	}

void ResetStartOffsetVars(void){
	for (int i=1;i<4; i++){
		M[i].startOffset = M[i].startOffsetOrig;
	}
}

void SetFFmultiplier(void){
	for (int i=1;i<4; i++){
		M[i].ff_multiplier = (M[i].startOffsetOrig/100.0);
	}
}

void ResetMotorSteadyStates(){
		M[MOTOR1].steadyState = 0;
		M[MOTOR2].steadyState = 0;
		M[MOTOR3].steadyState = 0;
}
void UpdateMotorPID_w_DCOffset(uint8_t motorIndex,int dc_offset)
{
	unsigned int calcPwm = 0;
	M[motorIndex].presentRpm = FilterRpm(motorIndex);
	
	if((startFlag == 1) && ((M[motorIndex].intTarget <= M[motorIndex].setRpm)))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget + M[motorIndex].rampRpm;
		if (M[motorIndex].intTarget >  M[motorIndex].setRpm){
				M[motorIndex].intTarget =  M[motorIndex].setRpm;
			}	
	}
		
	if((startFlag == 0)&&((M[motorIndex].intTarget > 0)))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget - M[motorIndex].rampRpm;
		if (M[motorIndex].intTarget <=  M[motorIndex].rampRpm){
				M[motorIndex].intTarget =  0;
			}
	}
	
	M[motorIndex].error = M[motorIndex].intTarget - M[motorIndex].presentRpm;
//	M[motorIndex].error = M[motorIndex].intTarget - filter1;
	
	
	/*if (__fabs(M[motorIndex].error) >= 300)
	{ M[motorIndex].overloadCount++;
		if( M[motorIndex].overloadCount >= 20){
			E.RpmErrorFlag = 1;
			S.errStopReason = MOTORARRAY[motorIndex];
			S.errStopReasonHMI = MOTORARRAY_HMI[motorIndex];
			S.errmotorFault = ERR_RPM_ERROR;	
			S.errVal = NO_FLOAT_VAR;
			 M[motorIndex].overloadCount = 0;
		}
	}else{
		M[motorIndex].overloadCount = 0;
	}*/

	if (M[motorIndex].steadyState == 1){  // only look for error when your in the steadystate region 
		if (abs(M[motorIndex].error) >= M[motorIndex].overloadDelta){
						 M[motorIndex].overloadCount ++ ; // only this is an array, maybe we can put this in the motor struct also
						if ( M[motorIndex].overloadCount > M[motorIndex].overloadTime){
							E.RpmErrorFlag = 1;
							S.errStopReason = MOTORARRAY[motorIndex];
							S.errStopReasonHMI = MOTORARRAY_HMI[motorIndex];
							S.errmotorFault = ERR_RPM_ERROR;	
							S.errVal = NO_FLOAT_VAR;
							 M[motorIndex].overloadCount = 0; // you can set it to zero because E.rpm error has been set
						}
					}
				else{
				 M[motorIndex].overloadCount = 0;
			}
		}
	
	M[motorIndex].integralError = (M[motorIndex].integralError + M[motorIndex].error);
	
	calcPwm = M[motorIndex].Kp*M[motorIndex].error + M[motorIndex].Ki*M[motorIndex].integralError;
	calcPwm = calcPwm  + dc_offset;
	
	M[motorIndex].preError = M[motorIndex].error;
	
	if ((calcPwm <= MAX_PWM) && (calcPwm > dc_offset))
	{
		M[motorIndex].pwm = calcPwm;
	}
	if(calcPwm <= dc_offset)
	{
		M[motorIndex].pwm = dc_offset;
	}
	
	
}


void StepMotorPID_w_DCOffset(uint8_t motorIndex,int offset)
{
	unsigned int calcPwm = 0;

	M[motorIndex].presentRpm = FilterRpm(motorIndex);
	M[motorIndex].intTarget = M[motorIndex].setRpm;

	
	M[motorIndex].error = M[motorIndex].intTarget - M[motorIndex].presentRpm;
	
	/*if (__fabs(M[motorIndex].error) >= 300)
	{
		E.RpmErrorFlag = 1;
		S.errStopReason = MOTORARRAY[motorIndex];
		S.errStopReasonHMI = MOTORARRAY_HMI[motorIndex];
		S.errmotorFault = ERR_RPM_ERROR;	
		S.errVal = NO_FLOAT_VAR;	
	}*/
	
		//0.1 sec per call, so 10 per sec, and 50 in 5 sec
	if (M[motorIndex].steadyState == 1){  // only look for error when your in the steadystate region 
		if (abs(M[motorIndex].error) >= M[motorIndex].overloadDelta){
						M[motorIndex].overloadCount++ ; // only this is an array, maybe we can put this in the motor struct also
						if (M[motorIndex].overloadCount > 40){
							E.RpmErrorFlag = 1;
							S.errStopReason = MOTORARRAY[motorIndex];
							S.errStopReasonHMI = MOTORARRAY_HMI[motorIndex];
							S.errmotorFault = ERR_RPM_ERROR;	
							S.errVal = NO_FLOAT_VAR;
							M[motorIndex].overloadCount = 0; // you can set it to zero because E.rpm error has been set
						}
					}
				else{
			M[motorIndex].overloadCount= 0;
			}
		}
	
	M[motorIndex].integralError = (M[motorIndex].integralError + M[motorIndex].error);
	
	calcPwm = M[motorIndex].Kp*M[motorIndex].error + M[motorIndex].Ki*M[motorIndex].integralError ;
	calcPwm = calcPwm + offset;

	M[motorIndex].preError = M[motorIndex].error;
  

	if ((calcPwm <= MAX_PWM) && (calcPwm > offset))
	{
		M[motorIndex].pwm = calcPwm;
	}
	if(calcPwm <= offset)
	{
		M[motorIndex].pwm = offset;
	}
}


void PauseMachine(void)
{
	uint8_t i = 0;
	HAL_Delay(100);
	for(i=3;i<6;i++)
		{
			M[i].pwm = 0;
		}
	TowerLamp(GREEN_ON);
}

void ResetSecondaryMotor(void)
{
				M[MOTOR1].pwm = 370;
				M[MOTOR2].pwm = 0;
				M[MOTOR3].pwm = 0;

				
				M[MOTOR1].intTarget = 150;
				M[MOTOR2].intTarget = 0;
				M[MOTOR3].intTarget = 0;

			
				M[MOTOR1].error = 0;
				M[MOTOR2].error = 0;
				M[MOTOR3].error = 0;

				
				M[MOTOR1].preError = 0;
				M[MOTOR2].preError = 0;
				M[MOTOR3].preError = 0;

				
				M[MOTOR1].integralError = 0;
				M[MOTOR2].integralError = 0;
				M[MOTOR3].integralError = 0;
}



void MotorTimer(char index)
{
		switch(index)
	{
		case MOTOR1_TIMER_ON:
				HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
		break;
		
		case MOTOR1_TIMER_OFF:
				HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_4);
		break;
		
		case MOTOR2_TIMER_ON:
				HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
		break;
		
		case MOTOR2_TIMER_OFF:
				HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_3);
		break;
		
		case MOTOR3_TIMER_ON:
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
		break;
		
		case MOTOR3_TIMER_OFF:
				HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
		break;
		
		case MOTOR4_TIMER_ON:
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
		break;
		
		case MOTOR4_TIMER_OFF:
				HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_2);
		break;
		
		case MOTOR5_TIMER_ON:
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
		break;
		
		case MOTOR5_TIMER_OFF:
				HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_3);
		break;
		
		case MOTOR6_TIMER_ON:
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
		break;
		
		case MOTOR6_TIMER_OFF:
				HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_4);
		break;
		
		case MOTOR7_TIMER_ON:
				HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
		break;
		
		case MOTOR7_TIMER_OFF:
				HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_4);
		break;
	}
}

void TimerLow(uint8_t index)
{
	switch(index)
	{
		case T2_CH4:
			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4,0);
		break;
		
		case T2_CH3:
			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3,0);
		break;
		
		case T1_CH4:
			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4,0);
		break;
		
		case T1_CH3:
			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3,0);
		break;
		
		case T1_CH2:
			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2,0);
		break;
		
		case T1_CH1:
			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1,0);
		break;
		
		case T3_CH4:
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4,0);
		break;
	}
}
void AllSignalVoltageLow(void)
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4,0);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3,0);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4,0);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3,0);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2,0);
	__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1,0);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4,0);
	
}
void ApplyPwms(void)
{
	if ((M[MOTOR1].pwm <= MAX_PWM ) && (M[MOTOR1].pwm >= MIN_PWM))
	{
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4,M[MOTOR1].pwm);
	}
	

	if ((M[MOTOR2].pwm <= MAX_PWM ) && (M[MOTOR2].pwm >= MIN_PWM))
	{
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3,M[MOTOR2].pwm);
	}
	
	if ((M[MOTOR3].pwm <= MAX_PWM ) && (M[MOTOR3].pwm >= MIN_PWM))
	{
		__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4,M[MOTOR3].pwm);
	}
 	
}


float req_FR_SR_draft;
float FR_surfaceSpeed;
float req_SR_surfaceSpeed;
float req_BR_surfaceSpeed;
float req_BR_rpm;
uint16_t BR_motorRPM;
uint16_t getBR_target(uint16_t FRmotorRPM){
	req_FR_SR_draft = dsp.tensionDraft/BREAK_DRAFT_BR_SR;
	FR_surfaceSpeed = FRmotorRPM * FR_CIRCUMFERENCE/60.0;
	req_SR_surfaceSpeed = FR_surfaceSpeed/req_FR_SR_draft;
	req_BR_surfaceSpeed = req_SR_surfaceSpeed/BREAK_DRAFT_BR_SR;
	req_BR_rpm = req_BR_surfaceSpeed/BR_CIRCUMFERENCE * 60;
	BR_motorRPM = (uint16_t)req_BR_rpm*BACKROLLER_GEAR_RATIO;
	return BR_motorRPM;
}




float req_creel_surfaceSpeed = 0;
float req_creel_pulleyRPM = 0;
float effectiveDia = 0;
float effectiveCirc = 0;
float creel_motorRPM = 0;
uint16_t getCreel_target(void){
	req_creel_surfaceSpeed = req_BR_surfaceSpeed;
	effectiveDia = CREEL_DIA + SLIVER_WIDTH;
	effectiveCirc = 3.14 * effectiveDia;
	req_creel_pulleyRPM = req_creel_surfaceSpeed/effectiveCirc * 60;
	creel_motorRPM = (uint16_t)req_creel_pulleyRPM*CREEL_GEARBOX_RATIO;
	return creel_motorRPM;
}
