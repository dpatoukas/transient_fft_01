/*
 * util.c
 *
 *  Created on: Apr 3, 2017
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


void UART_config()
{
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

    EUSCI_A_UART_init(EUSCI_A0_BASE, &param);

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
}


void ADC_config()
{
    // Pin P1.3 set for Ternary Module Function (which includes A3)
    P1SEL0 |= BIT3;
    P1SEL1 |= BIT3;

    // Clear ENC bit to allow register settings
    ADC12CTL0 &= ~ADC12ENC;

    // Sample-and-hold source select
    //
    // 000 -> ADC12SC bit (default)
    // ADC12CTL1 &= ~(ADC12SHS0 | ADC12SHS1 | ADC12SHS2);

    // Clock source select
    //
    // source: ACLK
    // pre-divider: 1
    // divider: 1
    ADC12CTL1 |= ADC12SSEL_1 | ADC12PDIV_0 | ADC12DIV_0;

    // sampling period select for MEM0: 256 clock cycles (*)
    // multiple sample and conversion: enabled
    // ADC module ON
    ADC12CTL0 |= ADC12SHT0_4 | ADC12MSC | ADC12ON;
    // (*) freq = ACLK / (ADC12PDIV_0 * ADC12DIV_0 * ADC12SHT0_4)
    //          = 32768 / (1 * 1 * 64)
    //          = 4 Hz

    // conversion sequence mode: repeat-single-channel
    // pulse-mode select: SAMPCON signal is sourced from the sampling timer
    ADC12CTL1 |= ADC12CONSEQ_2 | ADC12SHP;

    // resolution: 12 bit
    // data format: right-aligned, unsigned
    ADC12CTL2 |= ADC12RES__12BIT | ADC12DF_0;

    // conversion start address: MEM0
    ADC12CTL3 |= ADC12CSTARTADD_0;

    // MEM0 control register
    // reference select: VR+ = AVCC (3V), VR- = AVSS (0V)
    // input channel select: A3
    ADC12MCTL0 |= ADC12VRSEL_0 | ADC12INCH_3;

    // Clear interrupt for MEM0
    ADC12IFGR0 &= ~ADC12IFG0;

    // Enable interrupt for (only) MEM0
    ADC12IER0 = ADC12IE0;

    // Trigger first conversion (Enable conversion and Start conversion)
    ADC12CTL0 |= ADC12ENC | ADC12SC;
}
