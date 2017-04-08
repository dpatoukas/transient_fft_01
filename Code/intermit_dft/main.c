/**
 * DISCRETE FOURIER TRANSFORM UNDER INTERMITTENT POWER OPERATION
 *
 *
 * This DFT is an application of the InterPow library.
 *
 *
 * TODO:
 *
 * - Fix SAMPL_FREQ and co.
 *
 * - Make the cosine/sine tasks parametric
 *
 * - Measure the execution cycles of each task using msp_benchmarkStart() and
 *   msp_benchmarkStop()
 */


#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "QmathLib.h"
#include "DSPLib.h"                     // include DSPLib.h after QmathLib.h
#include "interpow.h"

#define F1              2               // must be less than SAMPL_FREQ/2
#define F2              10              // otherwise Nyquist might get angry
#define N_SAMPLES       32
#define SAMPL_FREQ      N_SAMPLES
#define SCALE_FACTOR    1.0/N_SAMPLES
#define MAX_AMPLITUDE   0x7FFF          // sinusoid max amplitude
#define LOOP_BURST      2

#define PI              3.1415926536


/******************************************************************************/
// Fixed-point variables declaration

DSPLIB_DATA(x1,4)
_q15 x1[N_SAMPLES];

DSPLIB_DATA(x2,4)
_q15 x2[N_SAMPLES];

DSPLIB_DATA(x,4)
/*
_q15 x[N_SAMPLES] = {
    0x0000, 0x029C, 0xFFFF, 0x0115, 0x03FF, 0x0114, 0xFFFF, 0x029C,
    0xFFFF, 0xFD62, 0xFFFF, 0xFEEA, 0xFC00, 0xFEEA, 0x0000, 0xFD63,
    0x0000, 0x029D, 0x0000, 0x0115, 0x03FF, 0x0114, 0xFFFF, 0x029B,
    0xFFFF, 0xFD62, 0xFFFF, 0xFEEA, 0xFC00, 0xFEEB, 0x0000, 0xFD64
};
*/
_q15 x[N_SAMPLES] = {
    871, 873, 868, 873, 873, 868, 870, 874,
    871, 868, 875, 875, 867, 872, 873, 867,
    871, 875, 869, 870, 875, 869, 869, 874,
    875, 870, 877, 875, 869, 869, 870, 867
};

DSPLIB_DATA(coeff,4)
_iq31 coeff[2*N_SAMPLES];

#pragma PERSISTENT(final_out)
int16_t final_out[N_SAMPLES] = {0};     // Test variable

msp_status status;


/******************************************************************************/
// Tasks' functions declaration

// Set ADC and sample
void T_sampling_function(void);

// Main DFT task that controls the execution of the DFT
void T_control_function(void);

// Cosine calculation task
void T_cosine_function(void);

// Sine calculation task
void T_sine_function(void);

// Multiply and accumulate task
void T_mac_function(void);

// Conversion task
void T_convert_function(void);

// Magnitude task
void T_magnitude_function(void);

/******************************************************************************/
// Definition of tasks, preceded by the PERSISTENT declaration

#pragma PERSISTENT(T_sampling)
NewTask(T_sampling, T_sampling_function, 0)

#pragma PERSISTENT(T_control)
NewTask(T_control, T_control_function, 1)

#pragma PERSISTENT(T_cosine)
NewTask(T_cosine, T_cosine_function, 1)

#pragma PERSISTENT(T_sine)
NewTask(T_sine, T_sine_function, 1)

#pragma PERSISTENT(T_mac)
NewTask(T_mac, T_mac_function, 0)

#pragma PERSISTENT(T_convert)
NewTask(T_convert, T_convert_function, 0)

#pragma PERSISTENT(T_magnitude)
NewTask(T_magnitude, T_magnitude_function, 1)

/******************************************************************************/
// Inform the program about the task to execute on the first start of the
// system, preceded by the PERSISTENT declaration

#pragma PERSISTENT(PersState)
InitialTask(T_sampling)


/******************************************************************************/
// Definition of fields, preceded by the PERSISTENT declaration
// Order of declaration: All fields per origin task

// T_sampling Fields
#pragma PERSISTENT(PersField(T_sampling, T_mac, f_input))
NewField(T_sampling, T_mac, f_input, INT16, N_SAMPLES)

// T_control Fields
/*self-field*/
#pragma PERSISTENT(PersSField0(T_control, sf_index))
#pragma PERSISTENT(PersSField1(T_control, sf_index))
NewSelfField(T_control, sf_index, UINT16, 1, SELF_FIELD_CODE_1)

#pragma PERSISTENT(PersField(T_control, T_cosine, f_index))
NewField(T_control, T_cosine, f_index, UINT16, 1)

#pragma PERSISTENT(PersField(T_control, T_sine, f_index))
NewField(T_control, T_sine, f_index, UINT16, 1)

#pragma PERSISTENT(PersField(T_control, T_mac, f_index))
NewField(T_control, T_mac, f_index, UINT16, 1)

//T_cosine
/*self-field*/
#pragma PERSISTENT(PersSField0(T_cosine, sf_index))
#pragma PERSISTENT(PersSField1(T_cosine, sf_index))
NewSelfField(T_cosine, sf_index, UINT16, 1, SELF_FIELD_CODE_1)

#pragma PERSISTENT(PersField(T_cosine, T_mac, f_output_cos))
NewField(T_cosine, T_mac, f_output_cos, INT16, N_SAMPLES)

//T_sine
/*self-field*/
#pragma PERSISTENT(PersSField0(T_sine, sf_index))
#pragma PERSISTENT(PersSField1(T_sine, sf_index))
NewSelfField(T_sine, sf_index, UINT16, 1, SELF_FIELD_CODE_1)

#pragma PERSISTENT(PersField(T_sine, T_mac, f_output_sin))
NewField(T_sine, T_mac, f_output_sin, INT16, N_SAMPLES)

//T_mac
#pragma PERSISTENT(PersField(T_mac, T_convert, f_output_coeff))
NewField(T_mac, T_convert, f_output_coeff, INT32, 2*N_SAMPLES)

//T_convert
#pragma PERSISTENT(PersField(T_convert, T_magnitude, f_real))
NewField(T_convert, T_magnitude, f_real, INT16, N_SAMPLES)

#pragma PERSISTENT(PersField(T_convert, T_magnitude, f_imag))
NewField(T_convert, T_magnitude, f_imag, INT16, N_SAMPLES)

//T_magnitude
/*self-field*/
#pragma PERSISTENT(PersSField0(T_magnitude, sf_index))
#pragma PERSISTENT(PersSField1(T_magnitude, sf_index))
NewSelfField(T_magnitude, sf_index, UINT16, 1, SELF_FIELD_CODE_1)

#pragma PERSISTENT(PersSField0(T_magnitude, sf_final_out))
#pragma PERSISTENT(PersSField1(T_magnitude, sf_final_out))
NewSelfField(T_magnitude, sf_final_out, INT16, N_SAMPLES, SELF_FIELD_CODE_2)

// NOTE: self-fields belonging to the same self-channel must have different
//       self-field codes, e.g.
//       - first self-field: SELF_FIELD_CODE_1
//       - second self-field: SELF_FIELD_CODE_2

/******************************************************************************/



int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	PM5CTL0 &= ~LOCKLPM5;

	P1OUT  = 0x00;                  // turn LEDs off
    P1DIR |= BIT0 | BIT1 | BIT2;    // prepare LED1, LED2 and P1.2 for output

	while(1) {
	    Resume();
	}

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
    // source: ADC12OSC (MODOSC), 1 MHz
    // pre-divider: 1
    // divider: 1
    ADC12CTL1 |= ADC12SSEL_0 | ADC12PDIV_0 | ADC12DIV_0;

    // sampling period select for MEM0: 64 clock cycles (*)
    // multiple sample and conversion: enabled
    // ADC module ON
    ADC12CTL0 |= ADC12SHT0_4 | ADC12MSC | ADC12ON;
    // (*) freq = MODOSC / (ADC12PDIV_0 * ADC12DIV_0 * ADC12SHT0_4)
    //          = 1000000 / (1 * 1 * 64)
    //          = 15625 Hz

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

uint8_t counter;
int16_t input[N_SAMPLES];
void T_sampling_function()
{
    __disable_interrupt();

    // configure ADC
    ADC_config();

    counter = 0;
    __enable_interrupt();

    while(__get_SR_register() & GIE);

    // disable and turn off the ADC to save energy
    ADC12CTL0 &= ~ADC12ENC;
    ADC12CTL0 &= ~ADC12ON;

    // Dummy input
    // WriteField_16(T_init, T_mac, f_input, x);

    // Real sampled input
    WriteField_16(T_sampling, T_mac, f_input, input);

    StartTask(T_control);
}

void T_control_function()
{
	uint16_t k;
	ReadSelfField_U16(T_control, sf_index, &k);

	if (k < N_SAMPLES) { /* Compute complex coefficients */
		WriteField_U16(T_control, T_cosine, f_index, &k);
		WriteField_U16(T_control, T_sine, f_index, &k);
		WriteField_U16(T_control, T_mac, f_index, &k);

		k++;
		WriteSelfField_U16(T_control, sf_index, &k);
		StartTask(T_cosine);
	}
	else /* Complex coefficients computed */
		StartTask(T_convert);
}

void T_cosine_function()
{
	uint16_t k; // external index, coming from T_control
	uint16_t i; // internal loop index

	ReadField_U16(T_control, T_cosine, f_index, &k);
	ReadSelfField_U16(T_cosine, sf_index, &i);

	//for (i=0; i<N_SAMPLES; i++)
		x1[i] = _Q15(cosf(i*k*2*PI/N_SAMPLES));

	if (i < N_SAMPLES) {
	    WriteFieldElement_16(T_cosine, T_mac, f_output_cos, &x1[i], i);

	    i++;
	    WriteSelfField_U16(T_cosine, sf_index, &i);
	    StartTask(T_cosine);
	}
	else {
	    i = 0;
	    WriteSelfField_U16(T_cosine, sf_index, &i);
	    StartTask(T_sine);
	}
	//WriteSelfField_U16(T_cosine,sf_index,&n)
	//WriteField_16(T_cosine, T_mac, f_output_cos, x1)
	//StartTask(T_sine)
}

void T_sine_function()
{
	uint16_t k; // external index, coming from T_control
	uint16_t i; // internal loop index

	ReadField_U16(T_control, T_sine, f_index, &k);
	ReadSelfField_U16(T_sine, sf_index, &i);

	//for (i=0; i<N_SAMPLES; i++)
        x2[i] = _Q15(-sinf(i*k*2*PI/N_SAMPLES));

    if (i < N_SAMPLES) {
        WriteFieldElement_16(T_sine, T_mac, f_output_sin, &x2[i], i);

        i++;
        WriteSelfField_U16(T_sine, sf_index, &i);
        StartTask(T_sine);
    }
    else {
        i = 0;
        WriteSelfField_U16(T_sine, sf_index, &i);
        StartTask(T_mac);
    }

	//WriteSelfField_U16(T_sine,sf_index,&n)
	//WriteField_16(T_sine, T_mac, f_output_sin, x2)
	//StartTask(T_mac)
}

void T_mac_function()
{
	 uint16_t k; // external index, coming from T_control

	 //ReadField_16(T_init, T_mac, f_input, x);
	 ReadField_16(T_sampling, T_mac, f_input, x);
	 ReadField_U16(T_control, T_mac, f_index, &k);
	 ReadField_16(T_cosine, T_mac, f_output_cos, x1);
	 ReadField_16(T_sine, T_mac, f_output_sin, x2);

	 msp_mac_q15_params macParams;
     macParams.length = N_SAMPLES;

	 msp_mac_q15(&macParams, x, x1, &coeff[k]);
     msp_mac_q15(&macParams, x, x2, &coeff[k+N_SAMPLES]);

     // WriteField_32(T_mac, T_convert, f_output_coeff, coeff);
     WriteFieldElement_32(T_mac, T_convert, f_output_coeff, &coeff[k], k);
     WriteFieldElement_32(T_mac, T_convert, f_output_coeff, &coeff[k+N_SAMPLES], k+N_SAMPLES);
     StartTask(T_control);
}


void T_convert_function()
{
 	ReadField_32(T_mac, T_convert, f_output_coeff, coeff);

 	msp_iq31_to_q15_params convParams;
    convParams.length = N_SAMPLES;

    status = msp_iq31_to_q15(&convParams, &coeff[0], x1);
    status = msp_iq31_to_q15(&convParams, &coeff[N_SAMPLES], x2);

    WriteField_16(T_convert, T_magnitude, f_real, x1);
    WriteField_16(T_convert, T_magnitude, f_imag, x2);
    StartTask(T_magnitude);
}

void T_magnitude_function()
{
 	uint16_t i; // internal loop index

 	ReadSelfField_U16(T_magnitude, sf_index, &i);

 	if (i<N_SAMPLES) {

 	    ReadField_16(T_convert, T_magnitude, f_real, x1);
 	    ReadField_16(T_convert, T_magnitude, f_imag, x2);

 	    x[i] = _Q15mag(x1[i], x2[i]);
 	    final_out[i] = x[i];

 	    i++;
 	    WriteSelfField_U16(T_magnitude, sf_index, &i);
 	    StartTask(T_magnitude);
 	}
 	else {
 	    if ( (final_out[2] > 100) && (final_out[10] > 100) )    // dummy check
 	        P1OUT = BIT1 | BIT2;    // correct, turn on GREEN and assert P1.2
 	    else
 	        P1OUT = BIT0;           // wrong, turn on RED
        while (1);
 	}
}


/*
 * ADC Interrupt handler
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ADC12_VECTOR)))
#endif
void ADC12_ISR(void)
{
    // static uint16_t i = 0;
    switch(__even_in_range(ADC12IV,12))
    {
    case  0: break;                         // Vector  0:  No interrupt
    case  2: break;                         // Vector  2:  ADC12BMEMx Overflow
    case  4: break;                         // Vector  4:  Conversion time overflow
    case  6: break;                         // Vector  6:  ADC12BHI
    case  8: break;                         // Vector  8:  ADC12BLO
    case 10: break;                         // Vector 10:  ADC12BIN
    case 12:                                // Vector 12:  ADC12BMEM0 Interrupt
        // Read ADC12MEM0 value
        if (counter < N_SAMPLES)
            input[counter++] = ADC12MEM0;
        else
            __bic_SR_register_on_exit(GIE);
        break;                              // Clear CPUOFF bit from 0(SR)
    case 14: break;                         // Vector 14:  ADC12BMEM1
    case 16: break;                         // Vector 16:  ADC12BMEM2
    case 18: break;                         // Vector 18:  ADC12BMEM3
    case 20: break;                         // Vector 20:  ADC12BMEM4
    case 22: break;                         // Vector 22:  ADC12BMEM5
    case 24: break;                         // Vector 24:  ADC12BMEM6
    case 26: break;                         // Vector 26:  ADC12BMEM7
    case 28: break;                         // Vector 28:  ADC12BMEM8
    case 30: break;                         // Vector 30:  ADC12BMEM9
    case 32: break;                         // Vector 32:  ADC12BMEM10
    case 34: break;                         // Vector 34:  ADC12BMEM11
    case 36: break;                         // Vector 36:  ADC12BMEM12
    case 38: break;                         // Vector 38:  ADC12BMEM13
    case 40: break;                         // Vector 40:  ADC12BMEM14
    case 42: break;                         // Vector 42:  ADC12BMEM15
    case 44: break;                         // Vector 44:  ADC12BMEM16
    case 46: break;                         // Vector 46:  ADC12BMEM17
    case 48: break;                         // Vector 48:  ADC12BMEM18
    case 50: break;                         // Vector 50:  ADC12BMEM19
    case 52: break;                         // Vector 52:  ADC12BMEM20
    case 54: break;                         // Vector 54:  ADC12BMEM21
    case 56: break;                         // Vector 56:  ADC12BMEM22
    case 58: break;                         // Vector 58:  ADC12BMEM23
    case 60: break;                         // Vector 60:  ADC12BMEM24
    case 62: break;                         // Vector 62:  ADC12BMEM25
    case 64: break;                         // Vector 64:  ADC12BMEM26
    case 66: break;                         // Vector 66:  ADC12BMEM27
    case 68: break;                         // Vector 68:  ADC12BMEM28
    case 70: break;                         // Vector 70:  ADC12BMEM29
    case 72: break;                         // Vector 72:  ADC12BMEM30
    case 74: break;                         // Vector 74:  ADC12BMEM31
    case 76: break;                         // Vector 76:  ADC12BRDY
    default: break;
    }
}
