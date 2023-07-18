#ifndef __EEPROM_H
#define __EEPROM_H

#define EEPROM_ADDRESS 0xAC


#define DF_DELIVERY_SPEED_ADDR 0X20
#define DF_TENSION_DRAFT_ADDR 0X22
#define DF_LENGTHLIMIT_ADDR 0X26

//All INTS
#define FRONTROLLER_KP 0X30
#define FRONTROLLER_KI 0X32
#define FRONTROLLER_START_OFFSET 0X34
#define BACKROLLER_KP 0X36
#define BACKROLLER_KI 0X38
#define BACKROLLER_START_OFFSET 0X3A
#define CREEL_KP 0X3C
#define CREEL_KI 0X40
#define CREEL_START_OFFSET 0X42

#define RAMPUP 0X44 // all ints
#define RAMPDOWN 0X46
#define START_VARS_C3 0X48

#define STOP_RPM_TRIGGER 0X5A

#define FRONTROLLER_FF_MLTPLR 0x60
#define BACKROLLER_FF_MLTPLR 0x62
#define CREEL_FF_MLTPLR 0x64


#define EXTRA  0x80

void EepromWriteInt(unsigned position,unsigned int data);
unsigned int EepromReadInt(unsigned position);
void EepromWriteFloat(unsigned position,float data);
unsigned int EepromReadFloat(unsigned position);
void WriteDrawFrameSettingsIntoEeprom(void);
void ReadDrawFrameSettingsFromEeprom(void);

//PID settings Fns
void ReadAllPIDSettingsFromEeprom(void);
void WriteAllPIDSettingsIntoEeprom(void);
void ReadPIDSettingsFromEeprom(int motorIndex);
void WritePIDSettingsIntoEeprom(int motorIndex);
int CheckSettingsReadings(void);
int CheckPIDSettings(void);
void WriteDefaultPIDSettingsIntoEepromAndStruct(void);
void LoadFactoryDefaultSettings(void);

void ReadDF_StartStopSettingsFromEeprom(void);
void WriteDF_StartSettingsIntoEeprom(void);
void WriteDF_StopSettingsIntoEeprom(void);
int CheckDF_StartStop_Settings(void);
void WriteDefaultDFStartStopSettingsIntoEepromAndStruct(void);

#endif /* __ENCODER_H */

