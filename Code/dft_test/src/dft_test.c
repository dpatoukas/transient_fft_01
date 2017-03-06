/*******************************************************************************
 *
 * DFT implementation
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
 *   library mentioned in the documentation
 *
 * - thanks to the redefinition of standard functions 'fputc' and 'fputs',
 *   the function 'printf' can be used to send strings using the UART
 *
 * - this version is not complete yet, the input signal has been generated
 *   but no DFT operation is performed
 *
 *******************************************************************************
 */

#define GLOBAL_Q        15          // from -1 to 0.999970
                                    // resolution 0.000030

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
//#include "QmathLib.h"
#include "DSPLib.h"
#include "util.h"


#ifndef M_2PI
#define M_2PI           6.28318530717958647692
#endif

#define F1              1
#define F2              4
#define P1              6.28318530717958647692      // 2*PI*F1
#define P2              25.13274122871834590768     // 2*PI*F2
#define SAMPL_PERIOD    0.00390625                  // 1/SAMPL_FREQ
#define SAMPL_FREQ      256                         // Simulation time:
#define N_SAMPLES       256                         // N_SAMPLES/SAMPL_FREQ


int fputc(int _c, register FILE *_fp);
int fputs(const char *_ptr, register FILE *_fp);


float x[N_SAMPLES];                     // discrete-time input signal
float xRE[N_SAMPLES], xIM[N_SAMPLES];   // DFT of x (real and imaginary parts)
float X[N_SAMPLES];                     // spectrum of x

DSPLIB_DATA(in,N_SAMPLES)
_q15 in[N_SAMPLES];
msp_status status;

//_q      q_t, q_sampl_period;
//_q10    q_a1, q_a2;
float   testF;


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

    uint16_t n;

    //q_sampl_period = _Q((float) SAMPL_PERIOD);
    //q_t = _Q(0.0);
    /*
    for (n=0 ; n<N_SAMPLES ; n++) {
        q_a1 = _Q10mpy(_Q10((float) P1), _QtoQ10(q_t));
        q_a2 = _Qmpy4(q_a1);
        x[n] = sin(_Q10toF(q_a1)) + sin(_Q10toF(q_a2));
        printf("%f ", x[n]);        // remove when optimising
        q_t += q_sampl_period;
        testF = _QtoF(q_t);         // remove when optimising
    }*/
    // VERY GOOD APPROXIMATION OF x

    msp_sinusoid_q15_params sin_init;
    sin_init.length = N_SAMPLES;
    sin_init.amplitude = _Q15(0.5);
    sin_init.cosOmega = _Q15(0.970031253);
    sin_init.sinOmega = _Q15(0.242980179);
    status = msp_sinusoid_q15(&sin_init, in);

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
        // do your stuff...
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
