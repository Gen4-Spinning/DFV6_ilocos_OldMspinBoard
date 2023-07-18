
#include "encoder.h"
#include "main.h"
#include "logicDefines.h"
#include "Initialize.h"
#include "functionDefines.h"

int Cap1 = 0;
int Cap2 = 0;
int Cap3 = 0;
int Cap4 = 0;
int Cap5 = 0;
int Cap6 = 0;
int Cap7 = 0;
int Cap8 = 0;
int Cap9 = 0;
int Cap10 = 0;

int Rpm1 = 0;
int Rpm2 = 0;
int Rpm3 = 0;
int Rpm4 = 0;
int Rpm5 = 0;
int Rpm6 = 0;
int Rpm7 = 0;
int Rpm8 = 0;
int Rpm9 = 0;
int Rpm10 = 0;

int filter1 = 0;
int filter2 = 0;
int filter3 = 0;
int filter4 = 0;
int filter5 = 0;
int filter6 = 0;
int filter7 = 0;
int filter8 = 0;
int filter9 = 0;
int filter10 = 0;


uint32_t sumCap1 = 0;
uint32_t sumCap2 = 0;
uint32_t sumCap3 = 0;
uint32_t sumCap4 = 0;

uint16_t cap1_Avg = 0;
uint16_t cap2_Avg = 0;
uint16_t cap3_Avg = 0;
uint16_t cap4_Avg = 0;

uint8_t Avgidx = 0;
uint8_t averagingStarted = 0;

void ResetSecondaryEncoderVariables(void)
{
	
		Cap3 = 0;
		Cap4 = 0;
		Cap5 = 0;
		Cap6 = 0;
		Cap7 = 0;
		Cap8 = 0;
		Cap9 = 0;
		Cap10 = 0;

		Rpm3 = 0;
		Rpm4 = 0;
		Rpm5 = 0;
		Rpm6 = 0;
		Rpm7 = 0;
		Rpm8 = 0;
		Rpm9 = 0;
		Rpm10 = 0;

		filter3 = 0;
		filter4 = 0;
		filter5 = 0;
		filter6 = 0;
		filter7 = 0;
		filter8 = 0;
		filter9 = 0;
		filter10 = 0;
}

void ResetEncoderVariables(void)
{
		Cap1 = 0;
		Cap2 = 0;
		Cap3 = 0;
		Cap4 = 0;
		Cap5 = 0;
		Cap6 = 0;

		Rpm1 = 0;
		Rpm2 = 0;
		Rpm3 = 0;
		Rpm4 = 0;
		Rpm5 = 0;
		Rpm6 = 0;

		filter1 = 0;
		filter2 = 0;
		filter3 = 0;
		filter4 = 0;
		filter5 = 0;
		filter6 = 0;

		sumCap1 = 0;
		sumCap2 = 0;
		cap1_Avg = 0;
		cap2_Avg = 0;
		Avgidx = 0;
		averagingStarted = 0;
}


void UpdateRPM(void){

	Rpm1 = (Cap1*10.0*60.0)/16.0; // front roller motor has 16 ppr
	Cap1 = 0;

	Rpm2 = (Cap2*10.0*60.0)/8.0; // 8 ppr
	Cap2 = 0;

	Rpm3 = (Cap3*10.0*60.0)/8.0; // 8 ppr
	Cap3 = 0;
}

int getRPM(uint8_t motorIndex){
	if (motorIndex == MOTOR1)
	{
		return Rpm1;
	}
	if (motorIndex == MOTOR2)
	{
		return Rpm2;
	}
	if (motorIndex == MOTOR3)
	{
		return Rpm3;
	}
	if (motorIndex == MOTOR4)
	{
		return Rpm4;
	}
	if (motorIndex == MOTOR5)
	{
		return Rpm5;
	}
	if (motorIndex == MOTOR6)
	{
		return Rpm6;
	}
	return 0;
}
























