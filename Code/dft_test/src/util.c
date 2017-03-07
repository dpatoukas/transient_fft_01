/*
 * util.c
 *
 *  Created on: Mar 5, 2017
 *      Author: Carlo
 */

#include "util.h"


void Clock_config()
{
    // LFXT Setup
    //Set PJ.4 and PJ.5 as Primary Module Function Input.
    PJDIR  |= 0x00;                 // Direction -> IN
    PJSEL0 |= (BIT4 | BIT5);        // Pin 4 and 5 set for
                                    // Primary Module Function

    // Use Calibration values for 1MHz Clock DCO
    CSCTL0  = CSKEY;
    CSCTL1 |= (DCORSEL & 0) | (DCOFSEL & DCOFSEL_0);
    CSCTL2 |= SELS_3;               // set DCO as source of SMCLK
    CSCTL3 |= DIVS__1;              // set '1' as SMCLK divider

    //Set external clock frequency to 32.768 KHz
    CS_setExternalClockSource(32768,0);

    // ACLK setting
    CSCTL2 |= SELA__LFXTCLK;        // set LFXT as source of ACLK
    CSCTL3 |= DIVA__1;              // set '1' as ACLK divider

    // MCLK setting
    // TODO: try to increase the MCLK frequency
    CSCTL2 |= SELM__DCOCLK;         // set DCO as source of MCLK
    CSCTL3 |= DIVM__1;              // set '1' as MCLK divider

    // Start XT1 with no time out
    CS_turnOnLFXT(CS_LFXT_DRIVE_0);
}


bool UART_config()
{
    bool retVal;

    // Configure UART for Tx/Rx with a BaudRate of 9600
    EUSCI_A_UART_initParam param = {0};
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_ACLK;
    param.clockPrescalar = 3;
    param.firstModReg = 0;
    param.secondModReg = 146;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;

    retVal = EUSCI_A_UART_init(EUSCI_A0_BASE, &param);

    if(retVal == STATUS_FAIL)
        return retVal;

    // Configure UART pins
    // Set P2.0 and P2.1 as Secondary Module Function Input.

                                    // Direction does not matter
    P2SEL1 |= (BIT0 | BIT1);        // Pin 0 and 1 set for
                                    // Secondary Module Function

    EUSCI_A_UART_enable(EUSCI_A0_BASE);

    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE,
                                EUSCI_A_UART_RECEIVE_INTERRUPT);

    // Enable USCI_A0 RX interrupt
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE,
                                 EUSCI_A_UART_RECEIVE_INTERRUPT);

    return STATUS_SUCCESS;
}
