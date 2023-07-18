
#include "eeprom.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c3;
extern int EEPROM_KP_ADDRESSES[4];
extern int EEPROM_KI_ADDRESSES[4];
extern int EEPROM_START_OFFSET_ADDRESSES[4];
extern int EEPROM_FEEDFORWARD_ADDRESSES[4];

void EepromWriteInt(unsigned position,unsigned int data)
{
	int count = 0;
	if (data>255)
	{
		while(data>255)
		{
			data = data-255;
			count++;
		}
	}
	
	if(count>0)
	{
		HAL_I2C_Mem_Write(&hi2c3,EEPROM_ADDRESS, position,0xFF,(uint8_t*)&count,1,1);
		HAL_Delay(5);
		HAL_I2C_Mem_Write(&hi2c3,EEPROM_ADDRESS, position+1,0xFF,(uint8_t*)&data,1,1);
		HAL_Delay(5);
	}
	else
	{	
		HAL_I2C_Mem_Write(&hi2c3,EEPROM_ADDRESS, position,0xFF,(uint8_t*)&count,1,1);
		HAL_Delay(5);
		HAL_I2C_Mem_Write(&hi2c3,EEPROM_ADDRESS, position+1,0xFF,(uint8_t*)&data,1,1);
		HAL_Delay(5);
	}

}

unsigned int EepromReadInt(unsigned position)
{
	int count =0;
	unsigned int data = 0;
	HAL_I2C_Mem_Read(&hi2c3,EEPROM_ADDRESS,position,0xFF,(uint8_t*)&count,1,1);
	HAL_I2C_Mem_Read(&hi2c3,EEPROM_ADDRESS,position+1,0xFF,(uint8_t*)&data,1,1);
	data = data+(count*255);
	return data;
}


void EepromWriteFloat(unsigned position,float data)
{
	unsigned int data_in = 0;
	int count3 = 0;
	int count2 = 0;
	int count1 = 0;
	data_in =  *(unsigned int*)&data;
	if (data_in>16777216)
	{
		while(data_in>16777216)
		{
			data_in = data_in-16777216;
			count3++;
		}
	}
		if (data_in>65536)
	{
		while(data_in>65536)
		{
			data_in = data_in-65536;
			count2++;
		}
	}
	if (data_in>255)
	{
		while(data_in>255)
		{
			data_in = data_in-255;
			count1++;
		}
	}
	
	EepromWriteInt(position,count3);
	EepromWriteInt(position+2,count2);
	EepromWriteInt(position+4,count1);
	EepromWriteInt(position+6,data_in);
}


unsigned int EepromReadFloat(unsigned position)
{
	long data = 0;
	unsigned int count3_in = 0;
	unsigned int count2_in = 0;
	unsigned int count1_in = 0;
	unsigned int count0_in = 0;
	count3_in = EepromReadInt(position);
	count2_in = EepromReadInt(position+2);
	count1_in = EepromReadInt(position+4);
	count0_in = EepromReadInt(position+6);
	data = count0_in+(count1_in*255)+(count2_in*65536)+(count3_in*16777216);
	return data;  //data has to be converted to float use this *(float*)&data
}


void ReadDrawFrameSettingsFromEeprom(void)
{
	long data_out = 0;
	dsp.deliverySpeed = EepromReadInt(DF_DELIVERY_SPEED_ADDR);
	data_out = EepromReadInt(DF_TENSION_DRAFT_ADDR);
	dsp.tensionDraft= ((float)data_out)/(float)100;
	dsp.lengthLimit = EepromReadInt(DF_LENGTHLIMIT_ADDR);
}

void WriteDrawFrameSettingsIntoEeprom(void)
{

	EepromWriteInt(DF_DELIVERY_SPEED_ADDR,dsp.deliverySpeed);
	EepromWriteInt(DF_TENSION_DRAFT_ADDR,(int)(dsp.tensionDraft*100));
	EepromWriteInt(DF_LENGTHLIMIT_ADDR,dsp.lengthLimit);
}

void WritePIDSettingsIntoEeprom(int motorIndex)
{
	EepromWriteInt(EEPROM_KI_ADDRESSES[motorIndex],(int)(M[motorIndex].Ki*100));
	EepromWriteInt(EEPROM_KP_ADDRESSES[motorIndex],(int)(M[motorIndex].Kp*100));
	EepromWriteInt(EEPROM_START_OFFSET_ADDRESSES[motorIndex],M[motorIndex].startOffsetOrig);
	EepromWriteInt(EEPROM_FEEDFORWARD_ADDRESSES[motorIndex],(int)(M[motorIndex].ff_multiplier * 100));

}

int CheckPIDSettings(void){
	for (int i=1;i< 4;i++){
		if (M[i].Ki > 8.0f){
			return 0;
		}
		if (M[i].Kp > 8.0f){
			return 0;
		}
		if (M[i].startOffsetOrig > 700){
			return 0;
		}
		if (M[i].ff_multiplier > 6){
			return 0;
		}
	}
	return 1;
}


int CheckSettingsReadings(void){
		//typically when something goes wrong with the eeprom you get a value that is very high..
		//to allow for changes place to place without changing this code, we just set the thresholds to  2* maxRange.
		// dont expect in any place the nos to go higher than this..NEED TO PUT LOWER BOUNDS FOR EVERYTHING
		if ((dsp.deliverySpeed > 1500 ) || (dsp.deliverySpeed < 10)){
			return 0;
		}
		if (dsp.tensionDraft > 16.0f){
			return 0;
		}
		if (dsp.lengthLimit > 10000){
			return 0;
		}
	return 1;
}



void ReadPIDSettingsFromEeprom(int motorIndex)
{
	long data_out = 0;
	data_out= EepromReadInt(EEPROM_KI_ADDRESSES[motorIndex]);
	M[motorIndex].Ki = ((float)data_out)/(float)100;
	data_out= EepromReadInt(EEPROM_KP_ADDRESSES[motorIndex]);
	M[motorIndex].Kp = ((float)data_out)/(float)100;
	M[motorIndex].startOffsetOrig = EepromReadInt(EEPROM_START_OFFSET_ADDRESSES[motorIndex]);
	data_out= EepromReadInt(EEPROM_FEEDFORWARD_ADDRESSES[motorIndex]);
	M[motorIndex].ff_multiplier = ((float)data_out)/(float)100;
}

void ReadDF_StartStopSettingsFromEeprom(void){
	DFs.rampUp = EepromReadInt(RAMPUP);
	DFs.rampDown = EepromReadInt(RAMPDOWN);
}


int CheckDF_StartStop_Settings(void){
		if (DFs.rampUp > 20.0){
			return 0;
		}
		if (DFs.rampDown > 20){
			return 0;
		}
	return 1;
}

void WriteDefaultDFStartStopSettingsIntoEepromAndStruct(void){
	DFs.rampUp = 5;
	DFs.rampDown = 5;
	WriteDF_StartSettingsIntoEeprom();
}

void WriteDF_StartSettingsIntoEeprom(void)
{
	EepromWriteInt(RAMPUP,DFs.rampUp);
	EepromWriteInt(RAMPDOWN,DFs.rampDown);
}



void WriteAllPIDSettingsIntoEeprom(void){
	for (int i=1;i< 4;i++){
		WritePIDSettingsIntoEeprom(i);
	}
}

void ReadAllPIDSettingsFromEeprom(void){
	for (int i=1;i< 4;i++){
		ReadPIDSettingsFromEeprom(i);
	}
}

void WriteDefaultPIDSettingsIntoEepromAndStruct(void){
	for (int i=1;i< 4;i++){
		M[i].Kp = 0.5;
		M[i].Ki = 0.1;
		M[i].startOffsetOrig = 50;
		M[i].ff_multiplier = 0.5;
		WritePIDSettingsIntoEeprom(i);
	}
}



void LoadFactoryDefaultSettings(void){
			dsp.deliverySpeed = DEFAULT_DELIVERYSPEED;
			dsp.tensionDraft = DEFAULT_DRAFT;
			dsp.lengthLimit = DEFAULT_LENGTHLIMIT;
}
