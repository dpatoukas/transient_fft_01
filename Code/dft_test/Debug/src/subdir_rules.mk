################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/dft_test.obj: ../src/dft_test.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/Applications/ti/ccsv7/ccs_base/msp430/include" --include_path="/Applications/ti/msp/MSP430Ware_3_70_00_05/iqmathlib/include" --include_path="/Users/Carlo/Documents/Uni/Delft/1st Year/Q3.Wireless Networking/Project/transient_fft_01/Code/dft_test/inc" --include_path="/Users/Carlo/Documents/Uni/Delft/1st Year/Q3.Wireless Networking/Project/transient_fft_01/Code/dft_test/driverlib/MSP430FR5xx_6xx" --include_path="/Users/Carlo/Documents/Uni/Delft/1st Year/Q3.Wireless Networking/Project/transient_fft_01/Code/dft_test/dsplib/include" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.1.LTS/include" --advice:hw_config=all --define=DEPRECATED --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --float_operations_allowed=all --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="src/dft_test.d" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/util.obj: ../src/util.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/Applications/ti/ccsv7/ccs_base/msp430/include" --include_path="/Applications/ti/msp/MSP430Ware_3_70_00_05/iqmathlib/include" --include_path="/Users/Carlo/Documents/Uni/Delft/1st Year/Q3.Wireless Networking/Project/transient_fft_01/Code/dft_test/inc" --include_path="/Users/Carlo/Documents/Uni/Delft/1st Year/Q3.Wireless Networking/Project/transient_fft_01/Code/dft_test/driverlib/MSP430FR5xx_6xx" --include_path="/Users/Carlo/Documents/Uni/Delft/1st Year/Q3.Wireless Networking/Project/transient_fft_01/Code/dft_test/dsplib/include" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.1.LTS/include" --advice:hw_config=all --define=DEPRECATED --define=__MSP430FR5994__ --define=_MPU_ENABLE -g --float_operations_allowed=all --printf_support=full --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="src/util.d" --obj_directory="src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


