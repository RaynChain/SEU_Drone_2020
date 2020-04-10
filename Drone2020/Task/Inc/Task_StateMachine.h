#include "System.h"

void StatusMachine_Init(void);
void StatusMachine_Update(void);

typedef enum
{
  STOP = 0,
  SINGLE_ROUND,
	MULTI_ROUND
}SHOOT_STATUS;

extern SHOOT_STATUS Shoot_Status_Value;

