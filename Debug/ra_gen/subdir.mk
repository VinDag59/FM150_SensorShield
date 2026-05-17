################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra_gen/common_data.c \
../ra_gen/hal_data.c \
../ra_gen/main.c \
../ra_gen/pin_data.c \
../ra_gen/vector_data.c 

C_DEPS += \
./ra_gen/common_data.d \
./ra_gen/hal_data.d \
./ra_gen/main.d \
./ra_gen/pin_data.d \
./ra_gen/vector_data.d 

CREF += \
FM150_SensorShield.cref 

OBJS += \
./ra_gen/common_data.o \
./ra_gen/hal_data.o \
./ra_gen/main.o \
./ra_gen/pin_data.o \
./ra_gen/vector_data.o 

MAP += \
FM150_SensorShield.map 


# Each subdirectory must supply rules for building sources it contributes
ra_gen/%.o: ../ra_gen/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-mcpu=cortex-m23 -mthumb -mlittle-endian -Oz -ffunction-sections -fdata-sections -fno-strict-aliasing -fmessage-length=0 -funsigned-char -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Waggregate-return -Wno-parentheses-equality -Wfloat-equal -g3 -std=c99 -fshort-enums -fno-unroll-loops -I"W:\\Workspace_2025-12\\FM150_SensorShield\\ra_gen" -I"." -I"W:\\Workspace_2025-12\\FM150_SensorShield\\ra_cfg\\fsp_cfg\\bsp" -I"W:\\Workspace_2025-12\\FM150_SensorShield\\ra_cfg\\fsp_cfg" -I"W:\\Workspace_2025-12\\FM150_SensorShield\\src" -I"W:\\Workspace_2025-12\\FM150_SensorShield\\Modules\\Scheduler" -I"W:\\Workspace_2025-12\\FM150_SensorShield\\Modules\\Serial_ASCII" -I"W:\\Workspace_2025-12\\FM150_SensorShield\\Modules\\SimpleKeyboard" -I"W:\\Workspace_2025-12\\FM150_SensorShield\\ra\\fsp\\inc" -I"W:\\Workspace_2025-12\\FM150_SensorShield\\ra\\fsp\\inc\\api" -I"W:\\Workspace_2025-12\\FM150_SensorShield\\ra\\fsp\\inc\\instances" -I"W:\\Workspace_2025-12\\FM150_SensorShield\\ra\\arm\\CMSIS_6\\CMSIS\\Core\\Include" -D_RENESAS_RA_ -D_RA_CORE=CM23 -D_RA_ORDINAL=1 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -x c "$<" -c -o "$@")
	@clang --target=arm-none-eabi @"$@.in"

