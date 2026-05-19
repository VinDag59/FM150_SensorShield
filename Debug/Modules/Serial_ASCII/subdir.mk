################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Modules/Serial_ASCII/ASCII_numbers.c \
../Modules/Serial_ASCII/Serial.c \
../Modules/Serial_ASCII/Serial_user.c 

C_DEPS += \
./Modules/Serial_ASCII/ASCII_numbers.d \
./Modules/Serial_ASCII/Serial.d \
./Modules/Serial_ASCII/Serial_user.d 

CREF += \
FM150_SensorShield.cref 

OBJS += \
./Modules/Serial_ASCII/ASCII_numbers.o \
./Modules/Serial_ASCII/Serial.o \
./Modules/Serial_ASCII/Serial_user.o 

MAP += \
FM150_SensorShield.map 


# Each subdirectory must supply rules for building sources it contributes
Modules/Serial_ASCII/%.o: ../Modules/Serial_ASCII/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-mcpu=cortex-m23 -mthumb -mlittle-endian -Oz -ffunction-sections -fdata-sections -fno-strict-aliasing -fmessage-length=0 -funsigned-char -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Waggregate-return -Wno-parentheses-equality -Wfloat-equal -g3 -std=c99 -fshort-enums -fno-unroll-loops -I"C:\\Users\\VincentDAgostino\\e2_studio\\Workspace_Travel26\\FM150_SensorShield\\ra_gen" -I"." -I"C:\\Users\\VincentDAgostino\\e2_studio\\Workspace_Travel26\\FM150_SensorShield\\ra_cfg\\fsp_cfg\\bsp" -I"C:\\Users\\VincentDAgostino\\e2_studio\\Workspace_Travel26\\FM150_SensorShield\\ra_cfg\\fsp_cfg" -I"C:\\Users\\VincentDAgostino\\e2_studio\\Workspace_Travel26\\FM150_SensorShield\\src" -I"C:\\Users\\VincentDAgostino\\e2_studio\\Workspace_Travel26\\FM150_SensorShield\\Modules\\Scheduler" -I"C:\\Users\\VincentDAgostino\\e2_studio\\Workspace_Travel26\\FM150_SensorShield\\Modules\\Serial_ASCII" -I"C:\\Users\\VincentDAgostino\\e2_studio\\Workspace_Travel26\\FM150_SensorShield\\Modules\\SimpleKeyboard" -I"C:\\Users\\VincentDAgostino\\e2_studio\\Workspace_Travel26\\FM150_SensorShield\\ra\\fsp\\inc" -I"C:\\Users\\VincentDAgostino\\e2_studio\\Workspace_Travel26\\FM150_SensorShield\\ra\\fsp\\inc\\api" -I"C:\\Users\\VincentDAgostino\\e2_studio\\Workspace_Travel26\\FM150_SensorShield\\ra\\fsp\\inc\\instances" -I"C:\\Users\\VincentDAgostino\\e2_studio\\Workspace_Travel26\\FM150_SensorShield\\ra\\arm\\CMSIS_6\\CMSIS\\Core\\Include" -D_RENESAS_RA_ -D_RA_CORE=CM23 -D_RA_ORDINAL=1 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -x c "$<" -c -o "$@")
	@clang --target=arm-none-eabi @"$@.in"

