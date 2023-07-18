#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "Structs.h"
#include "Logger.h"
#include "stdio.h"

int logCounter = 0;
extern UART_HandleTypeDef huart1;

char out[100] ;

int LogVal(int integer,int bufferIndex){
	out[bufferIndex] = integer>>8;
	out[bufferIndex+1] = integer ;	
	return bufferIndex+2;
}

int EndLine(int bufferIndex ){
	out[bufferIndex] = 0x0A;
	out[bufferIndex+1] = 0x0D;
	return bufferIndex+2;
}

/*void LogMotorVariables(void){
		//size of output is 2 *no of variables 
		int bufferIndex = 0;
		bufferIndex = LogVal(65533,bufferIndex); //StartIndicator
		bufferIndex = LogVal(filter1,bufferIndex);
	  bufferIndex = LogVal(filter2,bufferIndex);
		bufferIndex = LogVal(filter3,bufferIndex);
		for (int i=1;i<4;i++){
				bufferIndex = LogVal(M[i].intTarget,bufferIndex);
				bufferIndex = LogVal(M[i].pwm,bufferIndex);
			}
		//other variables
		bufferIndex = LogVal(S.current_state,bufferIndex);
		bufferIndex = LogVal(S.errStopReasonHMI,bufferIndex);
		bufferIndex = EndLine(bufferIndex);
		HAL_UART_Transmit(&huart1,(uint8_t*)out,bufferIndex,100);
}
*/


void LogMotorVariables(void){
		//size of output is 2 *no of variables 	
		sprintf(out,"%02X%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%02d%02d%02X\r",170,logCounter,M[1].intTarget,
		M[1].presentRpm,M[1].pwm,M[1].feedforward,M[1].calcPwm,M[2].intTarget,M[2].presentRpm,M[2].pwm,M[2].feedforward,M[2].calcPwm,M[3].intTarget,M[3].presentRpm,M[3].pwm,M[3].feedforward,
		M[3].calcPwm,S.current_state,S.errStopReasonHMI,255);
		//HAL_UART_Transmit(&huart1,(uint8_t*)out,73,100);
}

void LogSettings(void){
		//size of output is 2 *no of variables CC
		sprintf(out,"%02X%04d%04d%04d%02X\r",204,
		(int)(dsp.deliverySpeed*100),(int)(dsp.tensionDraft*100),dsp.lengthLimit,255);
		HAL_UART_Transmit(&huart1,(uint8_t*)out,18,100);
}


void LogPIDVals(void){
		//size of output is 2 *no of variables BB 
		sprintf(out,"%02X%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%04d%02X\r",187,
		(int)(M[1].Kp*100),(int)(M[1].Ki*100),M[1].startOffsetOrig,(int)(M[2].Kp*100),(int)(M[2].Ki*100),
		M[2].startOffsetOrig,(int)(M[3].Kp*100),(int)(M[3].Ki*100),M[3].startOffsetOrig,(int)(DFs.rampUp*100),
		DFs.rampDown,0,0,255);
		HAL_UART_Transmit(&huart1,(uint8_t*)out,57,100);
}



