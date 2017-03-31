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

DSPLIB_DATA(x1,4)
_q15 x1[N_SAMPLES];

DSPLIB_DATA(x2,4)
_q15 x2[N_SAMPLES];

DSPLIB_DATA(x,4)
_q15 x[N_SAMPLES];                      // Input signal x = x1 + x2

DSPLIB_DATA(coeff,4)
_iq31 coeff[2*N_SAMPLES];               // Complex DFT coefficients

// DSPLIB_DATA(temp,4)
// _q315 temp[2*N_SAMPLES];             // Not used anymore

// float X[N_SAMPLES];                  // Not used anymore

// volatile uint32_t cycleCount;

msp_status status;


/******************************************************************************/
// Tasks' functions declaration

// Initialise the parameters used by the program
void Task_initialize(void);

// Main DFT task that controls the execution of the DFT
void Task_control_dft(void);

// Cosine calculation task
void Task_dft_cosine(void);

// Sine calculation task
void Task_dft_sine(void);

// Multiply and accumulate task
void Task_mac(void);

// Conversion task
void Task_convert(void);

// Magnitude task
void Task_magnitude(void);

/******************************************************************************/
// Definition of tasks, preceded by the PERSISTENT declaration

#pragma PERSISTENT(T_init)
NewTask(T_init,Task_initialize,0)

#pragma PERSISTENT(T_control)
NewTask(T_control,Task_control_dft,1)

#pragma PERSISTENT(T_cos)
NewTask(T_cos,Task_dft_cosine,1)

#pragma PERSISTENT(T_sin)
NewTask(T_sin,Task_dft_sine,1)

#pragma PERSISTENT(T_mac)
NewTask(T_mac,Task_mac,0)

#pragma PERSISTENT(T_conv)
NewTask(T_conv,Task_convert,0)

#pragma PERSISTENT(T_mag)
NewTask(T_mag,Task_magnitude,1)

/******************************************************************************/
// Inform the program about the task to execute on the first start of the
// system, preceded by the PERSISTENT declaration

#pragma PERSISTENT(PersState)
InitialTask(T_init)


/******************************************************************************/
// Definition of fields, preceded by the PERSISTENT declaration
// Order of declaration: All fields per origin task

// T_init Fields
#pragma PERSISTENT(PersField(T_init, T_mac, input))
NewField(T_init, T_mac, input, INT16, N_SAMPLES)

// T_control Fields
/*self-field*/
#pragma PERSISTENT(PersSField0(T_control, inside_index))
#pragma PERSISTENT(PersSField1(T_control, inside_index))
NewSelfField(T_control, inside_index, UINT16, 1, SELF_FIELD_CODE_1)

#pragma PERSISTENT(PersField(T_control, T_cos, index))
NewField(T_control, T_cos, index, UINT16, 1)

#pragma PERSISTENT(PersField(T_control, T_sin, index))
NewField(T_control, T_sin, index, UINT16, 1)

#pragma PERSISTENT(PersField(T_control, T_mac, index))
NewField(T_control, T_mac, index, UINT16, 1)

//T_cos
/*self-field*/
#pragma PERSISTENT(PersSField0(T_cos, inside_index))
#pragma PERSISTENT(PersSField1(T_cos, inside_index))
NewSelfField(T_cos, inside_index, UINT16, 1, SELF_FIELD_CODE_2)

#pragma PERSISTENT(PersField(T_cos, T_mac, output_cos))
NewField(T_cos, T_mac, output_cos, INT16, N_SAMPLES)

//T_sin
/*self-field*/
#pragma PERSISTENT(PersSField0(T_sin, inside_index))
#pragma PERSISTENT(PersSField1(T_sin, inside_index))
NewSelfField(T_sin, inside_index, UINT16, 1, SELF_FIELD_CODE_3)

#pragma PERSISTENT(PersField(T_sin, T_mac, output_sin))
NewField(T_sin, T_mac, output_sin, INT16, N_SAMPLES)

//T_mac
#pragma PERSISTENT(PersField(T_mac, T_conv, output_coeff))
NewField(T_mac, T_conv, output_coeff, INT32, 2*N_SAMPLES)

//T_conv
#pragma PERSISTENT(PersField(T_conv, T_mag, output_real))
NewField(T_conv, T_mag, output_real, INT16, N_SAMPLES)

#pragma PERSISTENT(PersField(T_conv, T_mag, output_img))
NewField(T_conv, T_mag, output_img, INT16, N_SAMPLES)

//T_mag
/*self-field*/
#pragma PERSISTENT(PersSField0(T_mag, inside_index))
#pragma PERSISTENT(PersSField1(T_mag, inside_index))
NewSelfField(T_mag, inside_index, UINT16, 1, SELF_FIELD_CODE_4)

// NOTE: self-fields belonging to the same self-channel must have progressive
//       self-field codes, e.g.
//       - first self-field: SELF_FIELD_CODE_1
//       - second self-field: SELF_FIELD_CODE_2

/******************************************************************************/



int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	PM5CTL0 &= ~LOCKLPM5;

	P1DIR |= 0x01;
    P1OUT = 0x00;

	Resume();
	return 0;
}


void Task_initialize()
{
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

    // Scale down signal x to avoid saturation/overflow
    msp_scale_q15_params scaleParams;
    scaleParams.length = N_SAMPLES;
    scaleParams.scale = _Q15(SCALE_FACTOR);
    scaleParams.shift = 0;
    status = msp_scale_q15(&scaleParams, x, x);

    WriteField_16(T_init, T_mac, input, x)

    StartTask(T_control)
}

void Task_control_dft()
{
	uint16_t k;
	ReadSelfField_U16(T_control, inside_index, &k)

	k++;

	if (k <= N_SAMPLES) {
		WriteSelfField_U16(T_control, inside_index, &k)
		WriteField_U16(T_control, T_cos, index, &k)
		WriteField_U16(T_control, T_sin, index, &k)
		WriteField_U16(T_control, T_mac, index, &k)
		//Inside Loop Execution
		StartTask(T_cos)
	}
	else
		//The Loop Calculation has finished
		StartTask(T_conv)
}

void Task_dft_cosine()
{
	uint16_t n;
	uint16_t i;
	ReadField_U16(T_control, T_cos, index, &n)
	//ReadSelfField_U16(T_cos,inside_index,&in_index)
	n--;
	for (i=0; i<N_SAMPLES; i++)
		x1[i] = _Q15(cosf(i*n*2*PI/N_SAMPLES));

	//WriteSelfField_U16(T_cos,inside_index,&n)
	WriteField_16(T_cos, T_mac, output_cos, x1)
	StartTask(T_sin)
}

void Task_dft_sine()
{
	uint16_t n;
	uint16_t i;
	ReadField_U16(T_control, T_sin, index, &n)
	//ReadSelfField_U16(T_cos,inside_index,&in_index)
	n--;
	for (i=0; i<N_SAMPLES; i++)
        x2[i] = _Q15(-sinf(i*n*2*PI/N_SAMPLES));

	//WriteSelfField_U16(T_sin,inside_index,&n)
	WriteField_16(T_sin, T_mac, output_sin, x2)
	StartTask(T_mac)
}

void Task_mac()
{
	 uint16_t k;
	 ReadField_16(T_init, T_mac, input, x)
	 ReadField_U16(T_control, T_mac, index, &k)
	 k--;

	 ReadField_16(T_cos, T_mac, output_cos, x1)
	 ReadField_16(T_sin, T_mac, output_sin, x2)

	 msp_mac_q15_params macParams;
     macParams.length = N_SAMPLES;

	 msp_mac_q15(&macParams, x, x1, &coeff[k]);
     msp_mac_q15(&macParams, x, x2, &coeff[k+N_SAMPLES]);

     WriteField_32(T_mac, T_conv, output_coeff, coeff)
     StartTask(T_control)
}


 void Task_convert()
 {
 	ReadField_32(T_mac, T_conv, output_coeff, coeff)

 	msp_iq31_to_q15_params convParams;
    convParams.length = N_SAMPLES;

    status = msp_iq31_to_q15(&convParams, &coeff[0], x1);
    status = msp_iq31_to_q15(&convParams, &coeff[N_SAMPLES], x2);

    WriteField_16(T_conv, T_mag, output_img, x2)
    WriteField_16(T_conv, T_mag, output_real, x1)
    StartTask(T_mag)
 }

 void Task_magnitude()
 {
 	uint16_t k;
 	ReadSelfField_U16(T_mag, inside_index, &k)
 	k++;
 	ReadField_16(T_conv, T_mag, output_real, x1)
 	ReadField_16(T_conv, T_mag, output_img, x2)

 	x[k-1] = _Q15mag(x1[k-1], x2[k-1]);

 	WriteSelfField_U16(T_mag, inside_index, &k)

 	if ((k-1)==N_SAMPLES) {
 	   P1OUT ^= 0x01;
 	   StartTask(T_init)
 	}
 	else
 		StartTask(T_mag)
 }
