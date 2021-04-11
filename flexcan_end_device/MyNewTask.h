/*
 * MyNewTask.h
 *
 *  Created on: 5 abr. 2021
 *      Author: alber
 */

#ifndef MYNEWTASK_H_
#define MYNEWTASK_H_

/* Fwk*/
#include "TimersManager.h"
#include "FunctionLib.h"
#include "LED.h"
/* KSDK */
#include "fsl_common.h"
#include "EmbeddedTypes.h"
#include "fsl_os_abstraction.h"
#include "MacInterface.h"
#include "Messaging.h"

/* Define the available Task's Events */
#define gMyNewTaskEvent1_c (1 << 0)
#define gMyNewTaskEvent2_c (1 << 1)
#define gMyNewTaskEvent3_c (1 << 2)
#define TIMEOUT 3*1000

#define gMyTaskPriority_c 3
#define gMyTaskStackSize_c 400

void My_Task(osaTaskParam_t argument);
void MyTaskTimer_Start(void);
void MyTask_Button(void);
void MyTask_Init(void);

#endif /* MYNEWTASK_H_ */
