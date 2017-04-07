![logo](https://github.com/thijmenketel/transient_fft_01/blob/master/html/ip_2.png "logo")

# Discrete Fourier Transform for transiently powered devices
Solution for DFT analysis with an intermittent power source. Implementation is done on a MSP430 microcontroller development kit and will later be tested on real prototypes at Delft University of Technology. A library for generic operation with transiently powered devices is also developed.

## Team members
The team consists of three students from the master Embedded Systems at TU Delft:
 - [Carlo Delle Donne](https://github.com/cdelledonne)
 - [Dimitris Patoukas](https://github.com/dpatoukas)
 - [Thijmen Ketel](https://github.com/thijmenketel)

## Hardware
The project is implemented on a MSP430FR5994 from Texas Instruments (information can be found [here](http://www.ti.com/tool/msp-exp430fr5994)).

## Library Documentation
An extensive documentation can be found at `/html/index.html` (clone/download the repo and open the file in your browser).

## TODOs
- Make the cosine/sine tasks parametric
- Measure the execution cycles of each task using `msp_benchmarkStart()` and `msp_benchmarkStop()` functions from DSPLib
- Fix the documentation
