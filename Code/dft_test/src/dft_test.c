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


#define F1              2
#define F2              4
#define SAMPL_FREQ      32                         // Simulation time:
#define N_SAMPLES       32                         // N_SAMPLES/SAMPL_FREQ
//#define SAMPL_PER       0.00390625
#define SAMPL_PER       0.03125

#define PI              3.1415926536


int fputc(int _c, register FILE *_fp);
int fputs(const char *_ptr, register FILE *_fp);


DSPLIB_DATA(x1,4)
_q15 x1[N_SAMPLES];

DSPLIB_DATA(x2,4)
_q15 x2[N_SAMPLES];

DSPLIB_DATA(x,4)
_q15 x[N_SAMPLES];                      // Input signal x = x1 + x2

//DSPLIB_DATA(X_RE,4)
//_q15 X_RE[N_SAMPLES];                   // Real part of X = DFT(x)

//DSPLIB_DATA(X_IM,4)
//_q15 X_IM[N_SAMPLES];                   // Imaginary part of X = DFT(x)

//DSPLIB_DATA(X,4)
//_q15 X[N_SAMPLES];                      // Magnitude of X = DFT(x)

DSPLIB_DATA(temp,4)
_q15 temp[2*N_SAMPLES];

msp_status status;

//float X_RE[N_SAMPLES], X_IM[N_SAMPLES]; // DFT of x (real and imaginary parts)
//float X[N_SAMPLES];                     // spectrum of x


void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;       // Stop WDT

    Clock_config();

    if (UART_config() == STATUS_FAIL)
        return;

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings

    PM5CTL0 &= ~LOCKLPM5;           // as done by PMM_unlockLPM5()

    P1DIR |= 0x01;
    P1OUT = 0x00;                   // Turn off LED1

    __enable_interrupt();

    printf("All set up!\n\r");      // remove when optimising

    // Generate two sine waves and store them into x1 and x2

    msp_sinusoid_q15_params sinParams;
    sinParams.length = N_SAMPLES;
    sinParams.amplitude = _Q15(0.5);

    sinParams.cosOmega = _Q15(cosf(2*PI*F1/SAMPL_FREQ));
    sinParams.sinOmega = _Q15(sinf(2*PI*F1/SAMPL_FREQ));
    status = msp_sinusoid_q15(&sinParams, x1);

    sinParams.cosOmega = _Q15(cosf(2*PI*F2/SAMPL_FREQ));
    sinParams.sinOmega = _Q15(sinf(2*PI*F2/SAMPL_FREQ));
    status = msp_sinusoid_q15(&sinParams, x2);

    // Input signal x is the sum of two sine waves
    msp_add_q15_params addParams;
    addParams.length = N_SAMPLES;
    status = msp_add_q15(&addParams, x1, x2, x);

    // Scale down signal x
    msp_scale_q15_params scaleParams;
    scaleParams.length = N_SAMPLES;
    scaleParams.scale = _Q15(SAMPL_PER);
    scaleParams.shift = 0;
    status = msp_scale_q15(&scaleParams, x, x);

    // Perform the DFT on signal x
    /**
     *  for k = 0:N_SAMPLES
     *      X_RE[k] = X_IM[k] = 0;
     *      for n = 0:N_SAMPLES
     *          X_RE[k] = X_RE[k] + x[n]*cos(2*M_PI*n*k/N_SAMPLES);
     *          X_IM[k] = X_IM[k] - x[n]*sin(2*M_PI*n*k/N_SAMPLES);
     *      end for
     *      X[k] = sqrt(X_RE[k]*X_RE[k] + X_IM[k]*X_IM[k]);
     *  end for
     */

    uint16_t k, n;
    msp_mpy_q15_params mpyParams;
    mpyParams.length = N_SAMPLES;

    for (k=0; k<N_SAMPLES; k++) {

        for (n=0; n<N_SAMPLES; n++)                     // x1 will contain a
            x1[n] = _Q15(cosf(n*k*2*PI/N_SAMPLES));     // cos wave at f = k

        for (n=0; n<N_SAMPLES; n++)                     // x2 will contain a
            x2[n] = _Q15(-sinf(n*k*2*PI/N_SAMPLES));    // sin wave at f = k

        status = msp_mpy_q15(&mpyParams, x, x1, x1);    // x1 = x * cos(...)
        status = msp_mpy_q15(&mpyParams, x, x2, x2);    // x2 = x * -sin(...)

        temp[k] = temp[k+N_SAMPLES] = 0;
        for (n=0; n<N_SAMPLES; n++) {
            temp[k] += x1[n];                           // Real part
            temp[k+N_SAMPLES] += x2[n];                 // Imaginary part
        }
    }

    // Final result x = sqrt(x_RE * x_RE + x_IM * x_IM)
    status = msp_mpy_q15(&mpyParams, &temp[0], &temp[0], x1);
    status = msp_mpy_q15(&mpyParams, &temp[N_SAMPLES], &temp[N_SAMPLES], x2);
    status = msp_add_q15(&addParams, x1, x2, x);
    for (k=0; k<N_SAMPLES; k++)
        x[k] = sqrtf(x[k]);

    __no_operation();               // breakpoint here to check results

    P1OUT |= 0x01;                  // Turn on LED1

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
