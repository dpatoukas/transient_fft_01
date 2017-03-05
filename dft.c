/*
 ============================================================================
 Name        : dft_test.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Simple implementation of the DFT
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SAMPL_FREQ		1024
#define N_SAMPLES		1024

int main(void) {

    // time and frequency domain data arrays
    int n, k;								// indices for time and frequency domains
    float x[N_SAMPLES];						// discrete-time signal, x
    float xRE[N_SAMPLES], xIM[N_SAMPLES];	// DFT of x (real and imaginary parts)
    float X[N_SAMPLES];						// spectrum of x

    // Generate input as summation of two sine waves at different frequencies
    int f1 = 20;
    int f2 = 40;
    float t;
    for (n=0 ; n<N_SAMPLES ; n++) {
    	t = (float) n/SAMPL_FREQ;
    	x[n] = sin(2 * M_PI * f1 * t) + sin(2 * M_PI * f2 * t);
    	printf("%f ", x[n]);
    }

    printf("\n");

    // Calculate DFT of x using brute force
    for (k=0 ; k<N_SAMPLES ; k++)
    {
        // Real part of X[k]
        xRE[k] = 0;
        for (n=0 ; n<N_SAMPLES ; n++) xRE[k] += x[n] * cos(n * k * 2*M_PI / N_SAMPLES);

        // Imaginary part of X[k]
        xIM[k] = 0;
        for (n=0 ; n<N_SAMPLES ; n++) xIM[k] -= x[n] * sin(n * k * 2*M_PI / N_SAMPLES);

        // Magnitude at k-th frequency
        X[k] = sqrt(xRE[k]*xRE[k] + xIM[k]*xIM[k]);
        printf("%f ", X[k]);
    }

	return 0;
}
