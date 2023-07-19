
//Constants common to Logic, and the two Communication channels

//GENERAL
#define OFF 0
#define ON 1
#define UP 1
#define DOWN 2

//current machine SETTINGs
#define CURRENT_MACHINE DRAWFRAME
#define CURRENT_MACHINE_ID 1

//MACHINES
#define CARDING 1
#define DRAWFRAME 2
#define FLYER 3

//STATES
#define OFF_STATE 0
#define IDLE_STATE 1
#define DIAGNOSTIC_STATE 2
#define RUN_STATE 3
#define PAUSE_STATE 4
#define HALT_STATE 5
#define UPDATESETTINGS 6 // NOT SURE IF WE NEED THIS

//change states
#define TO_RUN 0
#define TO_DIAG 1
#define TO_SETTINGS 2
#define TO_PAUSE 3
#define TO_HALT 4
#define TO_UPDATE_SETTINGS 5
#define TO_IDLE 6
#define TO_UPDATE_PIDSETTINGS 9
#define INTERNAL_TO_IDLE 0x0A // this is a code to use inside the embedded coded wherever you want a falg taht says go to idle, like in 
#define TO_DIAG_SWITCH_OFF 0x0B 

//MOTORS
#define DF_FRONTROLLER 0x30 // FOR HMI we have to send 1E (hex(30))
#define DF_BACK_ROLLER 0x31
#define DF_CREEL 0x32

//EXTRA VARS FOR PID OPTIONS APART FROM MOTORS
#define PID_START_VARS 0x33
#define PID_STOP_VARS 0x34

//DIAG CODES. HMI AND RPI have different test type codes. TO FIX in next round
#define DIAG_ATTR_MOTORID 0x01
#define DIAG_ATTR_SIGNAL_IP_PERCENT 0x02
#define DIAG_ATTR_TARGET_RPM 0x03
#define DIAG_ATTR_TEST_TIME 0x04
#define DIAG_ATTR_TEST_RESULT 0x05

// ERROR CODES
#define ERR_RPM_ERROR 0x02
#define ERR_MOTOR_VOLTAGE_ERROR 0x03
#define ERR_DRIVER_VOLTAGE_ERROR 0x04
#define ERR_USER_PAUSE 0x08
#define ERR_SLIVER_CUT_ERROR 0x09
#define ERR_LENGTH_REACHED 0x0C
#define ERR_LAPPING 0x0E

//DATATYPES
#define FLOAT 4
#define INT 2
#define CHAR 1

//MISC
#define NOTHING 255
#define NO_VAR 0x00
#define NO_FLOAT_VAR (float)0.01
#define FRONTROLLER_OD_MM 40	
#define FR_CIRCUMFERENCE 125.66
#define BR_CIRCUMFERENCE 94.25

//DEFAULTS
#define DEFAULT_DELIVERYSPEED 120
#define DEFAULT_DRAFT 8.8	
#define DEFAULT_LENGTHLIMIT 1000

#define BUTTON_DEBOUNCE 2 // sec

#define BREAK_DRAFT_BR_SR 1.5
#define BACKROLLER_GEAR_RATIO 15.5
