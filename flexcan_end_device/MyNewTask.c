/*
 * MyNewTask.c
 *
 *  Created on: 5 abr. 2021
 *      Author: alber
 */

#include "MyNewTask.h"

osaEventId_t mMyEvents;
/* Global Variable to store our TimerID */
tmrTimerID_t myTimerID = gTmrInvalidTimerID_c;

/* Handler ID for task */
osaTaskId_t gMyTaskHandler_ID;

/* Local variable to store the current state of the LEDs */
static uint8_t ledsState = 0;

static uint8_t g_counter = 0;

/* OSA Task Definition*/
OSA_TASK_DEFINE(My_Task, gMyTaskPriority_c, 1, gMyTaskStackSize_c, FALSE );

/* Function to initthe task */
void MyTask_Init(void){
	mMyEvents = OSA_EventCreate(TRUE);
	/* The instance of the MAC is passed at task creaton*/
	gMyTaskHandler_ID = OSA_TaskCreate(OSA_TASK(My_Task), NULL);
}

static void myTaskTimerCallback(void*param){
	OSA_EventSet(mMyEvents, gMyNewTaskEvent2_c);
}

void MyTaskTimer_Start(void){
	OSA_EventSet(mMyEvents, gMyNewTaskEvent1_c);
}

void MyTaskTimer_Stop(void){
	OSA_EventSet(mMyEvents, gMyNewTaskEvent3_c);
}

/* Main custom task */
void My_Task(osaTaskParam_t argument){
	osaEventFlags_t customEvent;
	myTimerID = TMR_AllocateTimer();
	while(1){
		OSA_EventWait(mMyEvents, osaEventFlagsAll_c, FALSE, osaWaitForever_c, &customEvent);
		if( !gUseRtos_c && !customEvent){
			break;
		}

		/* Depending on the received event */
		switch(customEvent){
		case gMyNewTaskEvent1_c:
			TMR_StartIntervalTimer(myTimerID, /*myTimerID*/
					TIMEOUT, /* Timer's Timeout */
					myTaskTimerCallback, /* pointer to myTaskTimerCallback function */
					NULL
					);
			TurnOffLeds(); /* Ensure all LEDs are turned off */
			break;
		case gMyNewTaskEvent2_c: /* Event called from myTaskTimerCallback */
				switch(g_counter){
				case 0:
					LED_TurnOffAllLeds();
					Led2On();
					break;
				case 1:
					LED_TurnOffAllLeds();
					Led3On();
					break;
				case 2:
					LED_TurnOffAllLeds();
					Led4On();
					break;
				case 3:
					LED_TurnOffAllLeds();
					LED_TurnOnAllLeds();
					g_counter = -1;
					break;
				default:
					break;
				}
				g_counter += 1;
				break;
		case gMyNewTaskEvent3_c: /* Event to stop the timer */
			ledsState = 0;
			TurnOffLeds();
			TMR_StopTimer(myTimerID);
			break;
		default:
			break;
		}
	}
}


