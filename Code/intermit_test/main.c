#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "interpow.h"

/*
 * main.c
 */


void task_1_function (void);
void task_2_function (void);
void task_3_function (void);


/******************************************************************************/
// Define all the tasks here, preceded by the PERSISTENT declaration

#pragma PERSISTENT(T1)              // argument: task name
NewTask(T1, task_1_function, 1)

#pragma PERSISTENT(T2)              // argument: task name
NewTask(T2, task_2_function, 0)

#pragma PERSISTENT(T3)              // argument: task name
NewTask(T3, task_3_function, 0)

/******************************************************************************/
// Inform the program about the task to execute on the first start of the
// system, preceded by the PERSISTENT declaration

#pragma PERSISTENT(PersState)       // argument: PersState (FIXED)
InitialTask(T1)

/******************************************************************************/
// Define all fields here, preceded by the PERSISTENT declaration

#pragma PERSISTENT(PersField(T1, T2, int_field))
NewField(T1, T2, int_field, UINT16, 32)

#pragma PERSISTENT(PersField(T2, T3, end_field))
NewField(T2, T3, end_field, UINT16, 32)

#pragma PERSISTENT(__T1T1self_field_data_short_0)
#pragma PERSISTENT(__T1T1self_field_data_short_1)
NewSelfField(T1, self_field_data_short, UINT16, 4, SELF_FIELD_CODE_1)

#pragma PERSISTENT(__T1T1self_field_data_long_0)
#pragma PERSISTENT(__T1T1self_field_data_long_1)
NewSelfField(T1, self_field_data_long, UINT16, 32, SELF_FIELD_CODE_2)

/******************************************************************************/


int main(void) {

    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    Resume();

	return 0;
}


void task_1_function ()
{
    uint16_t internal_data[4];
    uint16_t transition_data[32];

    ReadSelfField_U16(T1, self_field_data_short, internal_data);
    ReadSelfField_U16(T1, self_field_data_long, transition_data);

    (!internal_data[0]) ? (internal_data[0]++) : (internal_data[0] <<= 1);
    internal_data[1] = (internal_data[0] << 1);
    internal_data[2] = (internal_data[1] << 1);
    internal_data[3] = (internal_data[2] << 1);

    WriteSelfField_U16(T1, self_field_data_short, internal_data);

    uint8_t offset = 0;
    while ((internal_data[0] >> offset) != 1)
        offset++;

    offset = offset << 2;

    uint8_t i;
    for (i=0; i<4; i++)
        transition_data[i+offset] = internal_data[i];

    WriteSelfField_U16(T1, self_field_data_long, transition_data);

    if (internal_data[3] < 1024) {
        StartTask(T1);
    }
    else {
        WriteField_U16(T1, T2, int_field, transition_data);
        StartTask(T2);
    }

    return;
}


void task_2_function ()
{
    __no_operation();

    uint16_t internal_data[32];
    ReadField_U16(T1, T2, int_field, internal_data);

    uint8_t i;
    for (i=0; i<32; i++)
        internal_data[i] += (1024-internal_data[i]);

    WriteField_U16(T2, T3, end_field, internal_data);
    StartTask(T3);

    return;
}

void task_3_function ()
{
    uint16_t internal_data[32];
    ReadField_U16(T2, T3, end_field, internal_data);

    uint8_t errors = 0;
    uint8_t i;
    for (i=0; i<32; i++)
        if (internal_data[i] != 1024)
            errors++;

    if (!errors) {
        PM5CTL0 &= ~LOCKLPM5;
        P1DIR |= 0x01;
        volatile uint16_t count;
        while (1) {
            count = 10000;
            while (--count);
            P1OUT ^= 0x01;
        }
    }

    StartTask(T3);
    return;
}
