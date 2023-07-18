#ifndef __LOGGER_H
#define __LOGGER_H

int LogVal(int integer,int bufferIndex);
void LogMotorVariables(void);
void LogSettings(void);
void LogPIDVals(void);
int EndLine(int bufferIndex );

#endif
