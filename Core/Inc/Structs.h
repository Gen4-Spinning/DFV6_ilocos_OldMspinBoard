#ifndef __STRUCTS_H
#define __STRUCTS_H

struct Uart
{
	volatile char TXcomplete ; //Flag that says whether a uart transmission is complete. is set to 1 inside the TX finished interrupt
										// so make it 0 after every transmission call
	volatile char TXtransfer; // Flag used in run mode and diagnosic mode where there is continuous data to link a timer with the logic code.
};

struct PIDUpdate
{int requestedPIDoptionIdx ;
 int updateOptionID ;
 int updateAttr1;
 int updateAttr2 ;
 int updateAttr3;
 int updateAttr4;
 int updatePIDVals ;
	
};

struct DF_StartStopSettings
{	int rampUp;
	int rampDown;
};

struct State 
{	
	char current_state; //current state
	char prev_state;    // prev state
	char state_change;	// bool to actually force a state change
	char first_enter;		// flag to tell when u enter a state for the first time
	char oneTimeflag;   // bool flag which can be used for any thing  
	char errStopReason;  //stop reason,which is usually the faulty motor 
	char errStopReasonHMI ;// stop reason with hmi code
	char errmotorFault; // reason why the motor stopped
	float errVal;				// err value associated with the reason the mtoor stopped
	char firstSwitchon; // state that says when we first switch on the machine
	char runMode;				// the current run state, normal, ramp or piecing.(FOR HMI)
	char updateBasePackets; // flag when u change run state (FOR HMI)
	int oneSecTimer;
	int keyState;
	int logType;
	int logNow;
	int bufferedVars;
	int loggingEnabled;
};

struct DrawFrameSettings
{
    float deliverySpeed;
    float tensionDraft;
    int lengthLimit;
};

struct Diagnostics
{
    int typeofTest;
    int motorID;
    int targetRPM;
    long targetSignal;
    int testTime;
};

struct Motor 
{	
	char name[5] ;
	unsigned int setRpm;	
	unsigned int presentRpm;
	float rampRpm;
	unsigned int piecingRpm;
	signed int intTarget;
	signed int error;
	signed int integralError;
	unsigned int feedforward;
	float ff_multiplier;
	signed int preError;
	unsigned int pwm;
	float Kp;
	float Ki;
	float Kd;
	int startOffsetOrig; //startOffset gets overwritten in the code. so to keep a non changing version
	int startOffset;
	int steadyState;
	int overloadDelta;
	int overloadTime;
	int overloadCount;
	int antiWindup;
	signed int calcPwm;
};

// TO keep structs for logic and for communication different
struct Error 
{
	unsigned int RpmErrorFlag;
};

extern struct PIDUpdate P;
extern struct Motor M[4];
extern struct State S;
extern struct DrawFrameSettings dsp;
extern struct Uart U;
extern struct Error E;
extern struct Diagnostics D;
extern struct DF_StartStopSettings DFs;
#endif
