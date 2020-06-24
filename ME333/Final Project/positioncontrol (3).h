// Ethan Piper
// ME333
// positioncontrol.h

#ifndef positioncontrol__H__
#define positioncontrol__H__
#define EINTMAX_POSITION 100

void positioncontrol_init(void);

float PID_Controller(float, float);

#endif