################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../browser.o \
../get_token.o \
../parse.o \
../parse_css.o \
../render.o 

C_SRCS += \
../browser.c \
../get_token.c \
../parse.c \
../parse_css.c \
../render.c \
../test.c 

OBJS += \
./browser.o \
./get_token.o \
./parse.o \
./parse_css.o \
./render.o \
./test.o 

C_DEPS += \
./browser.d \
./get_token.d \
./parse.d \
./parse_css.d \
./render.d \
./test.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/gtk-2.0 -I/usr/include/cairo -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


