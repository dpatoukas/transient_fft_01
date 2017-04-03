/**
 * DISCRETE FOURIER TRANSFORM UNDER INTERMITTENT POWER OPERATION
 *
 *
 * This DFT is an application of the InterPow library.
 *
 * A dummy periodical input signal is generated and transformed using a DFT
 * algorithm which makes use of the MSP430 DSPLibrary.
 *
 *
 * TODO:
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
#include "DSPLib.h"
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
_q15 x[N_SAMPLES] = {
    0x0000, 0x029C, 0xFFFF, 0x0115, 0x03FF, 0x0114, 0xFFFF, 0x029C,
    0xFFFF, 0xFD62, 0xFFFF, 0xFEEA, 0xFC00, 0xFEEA, 0x0000, 0xFD63,
    0x0000, 0x029D, 0x0000, 0x0115, 0x03FF, 0x0114, 0xFFFF, 0x029B,
    0xFFFF, 0xFD62, 0xFFFF, 0xFEEA, 0xFC00, 0xFEEB, 0x0000, 0xFD64
};

DSPLIB_DATA(coeff,4)
_iq31 coeff[2*N_SAMPLES];

#pragma PERSISTENT(final_out)
int16_t final_out[N_SAMPLES] = {0};     // Test variable

msp_status status;


/******************************************************************************/
// Tasks' functions declaration

// Initialise the parameters used by the program
void T_init_function(void);

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

#pragma PERSISTENT(T_init)
NewTask(T_init, T_init_function, 0)

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
InitialTask(T_init)


/******************************************************************************/
// Definition of fields, preceded by the PERSISTENT declaration
// Order of declaration: All fields per origin task

// T_init Fields
#pragma PERSISTENT(PersField(T_init, T_mac, f_input))
NewField(T_init, T_mac, f_input, INT16, N_SAMPLES)

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

    P1DIR |= 0x01;  // prepare LED1
    P1DIR |= 0x02;  // prepare LED2
    P1OUT  = 0x00;  // turn LEDs off

	while(1) {
	    Resume();
	}

}


void T_init_function()
{
    // Generate two sine waves and store them into x1 and x2
/*
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

    // Scale down signal x to avoid saturation/overflow
    msp_scale_q15_params scaleParams;
    scaleParams.length = N_SAMPLES;
    scaleParams.scale = _Q15(SCALE_FACTOR);
    scaleParams.shift = 0;
    status = msp_scale_q15(&scaleParams, x, x);
*/
    WriteField_16(T_init, T_mac, f_input, x);

    // P1OUT = 0x02;   // turn on GREEN

    StartTask(T_control)
}

void T_control_function()
{
	uint16_t k;
	ReadSelfField_U16(T_control, sf_index, &k)

	if (k < N_SAMPLES) { /* Compute complex coefficients */
		WriteField_U16(T_control, T_cosine, f_index, &k)
		WriteField_U16(T_control, T_sine, f_index, &k)
		WriteField_U16(T_control, T_mac, f_index, &k)

		k++;
		WriteSelfField_U16(T_control, sf_index, &k)
		StartTask(T_cosine)
	}
	else /* Complex coefficients computed */
		StartTask(T_convert)
}

void T_cosine_function()
{
	uint16_t k; // external index, coming from T_control
	uint16_t i; // internal loop index

	ReadField_U16(T_control, T_cosine, f_index, &k)
	ReadSelfField_U16(T_cosine, sf_index, &i)

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

	 ReadField_16(T_init, T_mac, f_input, x)
	 ReadField_U16(T_control, T_mac, f_index, &k)
	 ReadField_16(T_cosine, T_mac, f_output_cos, x1)
	 ReadField_16(T_sine, T_mac, f_output_sin, x2)

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
 	ReadField_32(T_mac, T_convert, f_output_coeff, coeff)

 	msp_iq31_to_q15_params convParams;
    convParams.length = N_SAMPLES;

    status = msp_iq31_to_q15(&convParams, &coeff[0], x1);
    status = msp_iq31_to_q15(&convParams, &coeff[N_SAMPLES], x2);

    WriteField_16(T_convert, T_magnitude, f_real, x1)
    WriteField_16(T_convert, T_magnitude, f_imag, x2)
    StartTask(T_magnitude)
 }

 void T_magnitude_function()
 {
 	uint16_t i; // internal loop index

 	ReadSelfField_U16(T_magnitude, sf_index, &i)

 	if (i<N_SAMPLES) {

 	    ReadField_16(T_convert, T_magnitude, f_real, x1)
 	    ReadField_16(T_convert, T_magnitude, f_imag, x2)

 	    x[i] = _Q15mag(x1[i], x2[i]);
 	    final_out[i] = x[i];

 	    i++;
 	    WriteSelfField_U16(T_magnitude, sf_index, &i)
 	    StartTask(T_magnitude)
 	}
 	else {
 	    if ( (final_out[2] > 100) && (final_out[10] > 100) )    // dummy check
 	        P1OUT = 0x01;   // correct, turn on RED (weird?)
 	    else
 	        P1OUT = 0x02;   // wrong, turn on GREEN (more weird?)
        while (1);
 	}
 }
