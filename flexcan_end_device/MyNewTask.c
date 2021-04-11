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

static uint8_t msg[] = "Counter:x\n\r";

static nwkToMcpsMessage_t *mpPacket;

static uint8_t mMsduHandle;

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

void MyTask_Button(void){
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
			g_counter = g_counter%4;
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
					break;
				default:
					break;
				}
			msg[8] = g_counter + '0';
			g_counter += 1;
			mpPacket = MSG_Alloc(sizeof(nwkToMcpsMessage_t) + 1);

			if(mpPacket != NULL)
			{
				/* Data is available in the SerialManager's receive buffer. Now create an
				MCPS-Data Request message containing the data. */
				mpPacket->msgType = gMcpsDataReq_c;
				mpPacket->msgData.dataReq.pMsdu = (uint8_t*)(&mpPacket->msgData.dataReq.pMsdu) +
												  sizeof(mpPacket->msgData.dataReq.pMsdu);
				/* Create the header using coordinator information gained during
				the scan procedure. Also use the short address we were assigned
				by the coordinator during association. */
				mpPacket->msgData.dataReq.pMsdu = msg;
				mpPacket->msgData.dataReq.dstAddr = 0x0000;
				mpPacket->msgData.dataReq.srcAddr = 0x0001;
				mpPacket->msgData.dataReq.dstPanId = 0x0505;
				mpPacket->msgData.dataReq.srcPanId = 0x0505;
				mpPacket->msgData.dataReq.dstAddrMode = gAddrModeShortAddress_c;
				mpPacket->msgData.dataReq.srcAddrMode = gAddrModeShortAddress_c;
				mpPacket->msgData.dataReq.msduLength = 12;
				/* Request MAC level acknowledgement of the data packet */
				mpPacket->msgData.dataReq.txOptions = gMacTxOptionsAck_c;
				/* Give the data packet a handle. The handle is
				returned in the MCPS-Data Confirm message. */
				mpPacket->msgData.dataReq.msduHandle = mMsduHandle++;
				/* Don't use security */
				mpPacket->msgData.dataReq.securityLevel = gMacSecurityNone_c;

				(void)NWK_MCPS_SapHandler(mpPacket,0);

				/* Prepare for another data buffer */
				mpPacket = NULL;
			}
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


