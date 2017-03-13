#include <msp430.h>
#include <stdint.h>
#include "interpow.h"

/*
 * main.c
 */


void task_1_function (void);
void task_2_function (void);


NewTask(T1, task_1_function)
NewTask(T2, task_2_function)

#pragma PERSISTENT(prog_state)
InitialTask(T1)

#pragma PERSISTENT(GetName(T1, T2, f1))
//NewChannel(T1, T2, f1, UINT, 32)
NewField(T1, T2, f1, UINT, 32)

uint16_t data[32];

int main(void) {

    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    ResumeProgram();

    uint16_t i = __T1T2f1[0];
    __T1T2f1[i+1] = 10+1+i;
    __T1T2f1[0] = i+1;

    ReadField_U16(T1, f1, data);

    data[5] = 127;

    WriteFieldElement_U16(T2, f1, &data[5], 5);

    StartTask(T2);

	return 0;
}


void task_1_function () {
    return;
}


void task_2_function () {
    return;
}
