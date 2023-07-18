#ifndef __FUNCTIONDEFINES_H
#define __FUNCTIONDEFINES_H

void ApplyPwms(void);
void AllSignalVoltageLow(void);
void TimerLow(uint8_t index);
void MotorDrive(char index);	
void ResetMotorVariables(void);
void ResetStartOffsetVars(void);
void UpdateMotorVoltageDiag(uint8_t motorIndex,int pwmValue);
void UpdateMotorPID_Diag(uint8_t motorIndex);
	
void MotorTimer(char index);
void StepMotorPID(uint8_t motorIndex);
void StepMotorPID1(uint8_t motorIndex);
void StepMotorPID_w_DCOffset(uint8_t motorIndex,int offset);
void UpdateMotorPID(uint8_t motorIndex);
void UpdateMotorPID_w_DCOffset(uint8_t motorIndex,int dc_offset);
void UpdateMotorPiecing(uint8_t motorIndex);
void UpdateMotorVoltage(uint8_t motorIndex,int pwmValue);
void AllTimerOn(void);
void ResetSecondaryMotor(void);
char Pushbutton(void);
void CheckSteadyStateReached(uint8_t motorIndex);
void ResetMotorSteadyStates(void);
char LappingSensor(void);
void SetFFmultiplier(void);

void LedOn(char index);
void LedOff(char index);
void LedToggle(char index); 


char InputSensor1(void);
char InputSensor2(void);
char InputSensor3(void);

void TowerLamp(char index);

void Motor(char index);


char InputVoltageSense(void);

void RunMachine(void);

void updateTargets(uint8_t motorIndex,uint8_t rampUp);
void followTargets(uint8_t motorIndex);

#endif 
