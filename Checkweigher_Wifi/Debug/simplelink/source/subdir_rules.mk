################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
simplelink/source/device.obj: C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source/device.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="simplelink/source/device.d" --obj_directory="simplelink/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

simplelink/source/driver.obj: C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source/driver.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="simplelink/source/driver.d" --obj_directory="simplelink/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

simplelink/source/flowcont.obj: C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source/flowcont.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="simplelink/source/flowcont.d" --obj_directory="simplelink/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

simplelink/source/fs.obj: C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source/fs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="simplelink/source/fs.d" --obj_directory="simplelink/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

simplelink/source/netapp.obj: C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source/netapp.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="simplelink/source/netapp.d" --obj_directory="simplelink/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

simplelink/source/netcfg.obj: C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source/netcfg.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="simplelink/source/netcfg.d" --obj_directory="simplelink/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

simplelink/source/nonos.obj: C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source/nonos.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="simplelink/source/nonos.d" --obj_directory="simplelink/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

simplelink/source/socket.obj: C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source/socket.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="simplelink/source/socket.d" --obj_directory="simplelink/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

simplelink/source/spawn.obj: C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source/spawn.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="simplelink/source/spawn.d" --obj_directory="simplelink/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

simplelink/source/wlan.obj: C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source/wlan.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --opt_for_speed=2 --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/ti/TivaWare_C_Series-2.1.0.12573/" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/examples/common" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/include" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/simplelink/source" --include_path="C:/ti/CC3100SDK_1.2.0/cc3100-sdk/platform/tiva-c-launchpad" --define=PART_TM4C123GH6PM --define=_USE_CLI_ --define=TARGET_IS_BLIZZARD_RA1=1 --define=ccs="ccs" -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="simplelink/source/wlan.d" --obj_directory="simplelink/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


