################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
board/board.obj: C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad/board.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="board/board.d" --obj_directory="board" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


