/*******************************************************************************
 *
 * DFT implementation (using DSPLib libraries)
 *
 * @author: Carlo Delle Donne       stud. number 4624718
 * @author: Dimitrios Patoukas      stud. number xxxxxxx
 * @author: Thijmen Ketel           stud. number yyyyyyy
 *
 *
 * REMARKS:
 *
 * - the standard library <math.h> contains optimised implementation of
 *   mathematical functions, it actually corresponds to the "msp430_math.h"
 *   library mentioned x the documentation
 *
 * - thanks to the redefinition of standard functions 'fputc' and 'fputs',
 *   the function 'printf' can be used to send strings using the UART
 *
 * - this version is not complete yet, the input signal has been generated
 *   but no DFT operation is performed
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "DSPLib.h"
#include "util.h"


#define F1              5
#define F2              10
#define SAMPL_FREQ      256                         // Simulation time:
#define N_SAMPLES       256                         // N_SAMPLES/SAMPL_FREQ

#define PI              3.1415926536


int fputc(int _c, register FILE *_fp);
int fputs(const char *_ptr, register FILE *_fp);


DSPLIB_DATA(x1,N_SAMPLES)
_q15 x1[N_SAMPLES];

DSPLIB_DATA(x2,N_SAMPLES)
_q15 x2[N_SAMPLES];

DSPLIB_DATA(x,N_SAMPLES)
_q15 x[N_SAMPLES];                      // Input signal x = x1 + x2

msp_status status;

float X_RE[N_SAMPLES], X_IM[N_SAMPLES]; // DFT of x (real and imaginary parts)
float X[N_SAMPLES];                     // spectrum of x


void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;       // Stop WDT

    Clock_config();

    if (UART_config() == STATUS_FAIL)
        return;

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings

    PM5CTL0 &= ~LOCKLPM5;           // as done by PMM_unlockLPM5()

    __enable_interrupt();

    printf("All set up!\n\r");      // remove when optimising

    // Generate two sine waves and store them into x1 and x2
    // for cosOmega use the value of cos(2*pi*frequency/SAMPL_FREQ)
    // for sinOmega use the value of sin(2*pi*frequency/SAMPL_FREQ)

    msp_sinusoid_q15_params sinParams;
    sinParams.length = N_SAMPLES;
    sinParams.amplitude = _Q15(0.5);

    sinParams.cosOmega = _Q15(cosf(2*PI*F1/SAMPL_FREQ));        // Frequency
    sinParams.sinOmega = _Q15(sinf(2*PI*F1/SAMPL_FREQ));        // F1 = 5 Hz
    status = msp_sinusoid_q15(&sinParams, x1);

    sinParams.cosOmega = _Q15(cosf(2*PI*F2/SAMPL_FREQ));        // Frequency
    sinParams.sinOmega = _Q15(sinf(2*PI*F2/SAMPL_FREQ));        // F2 = 10 Hz
    status = msp_sinusoid_q15(&sinParams, x2);

    // Input signal x is the sum of two sine waves
    msp_add_q15_params addParams;
    addParams.length = N_SAMPLES;
    status = msp_add_q15(&addParams, x1, x2, x);

    __no_operation();               // breakpoint here to check results

    while(1);

}

//******************************************************************************
//
// This is the USCI_A0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        // do your stuff, e.g.
        // RXData = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
        break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    }
}


/**
 * Redefinition of standard function to use 'prinft' for UART
 */
int fputc(int _c, register FILE *_fp)
{
    while(!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = (unsigned char) _c;
    //while (!UCTXIFG);

    return((unsigned char)_c);
}


/**
 * Redefinition of standard function to use 'prinft' for UART
 */
int fputs(const char *_ptr, register FILE *_fp)
{
    unsigned int i, len;

    len = strlen(_ptr);

    for(i=0 ; i<len ; i++)
    {
      while(!(UCA0IFG & UCTXIFG));
      UCA0TXBUF = (unsigned char) _ptr[i];
    }

    return len;
}
