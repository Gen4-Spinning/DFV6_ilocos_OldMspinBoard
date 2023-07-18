#ifndef __ENCODER_H
#define __ENCODER_H

#include "stdio.h"

void UpdateRpm(void);
void ResetEncoderVariables(void);
int FilterRpm(char motorIndex);
void ResetSecondaryEncoderVariables(void);

void UpdateRPM(void);
int getRPM(uint8_t motorIndex);
void UpdateRpm_NewDrives(void);

#endif /* __ENCODER_H */
