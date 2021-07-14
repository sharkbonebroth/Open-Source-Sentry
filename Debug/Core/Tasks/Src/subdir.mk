################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Tasks/Src/GimbalControlTask.c \
../Core/Tasks/Src/MovementControlTask.c \
../Core/Tasks/Src/RefereeProcessingTask.c 

OBJS += \
./Core/Tasks/Src/GimbalControlTask.o \
./Core/Tasks/Src/MovementControlTask.o \
./Core/Tasks/Src/RefereeProcessingTask.o 

C_DEPS += \
./Core/Tasks/Src/GimbalControlTask.d \
./Core/Tasks/Src/MovementControlTask.d \
./Core/Tasks/Src/RefereeProcessingTask.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Tasks/Src/GimbalControlTask.o: ../Core/Tasks/Src/GimbalControlTask.c Core/Tasks/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F427xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/Zheng Hao Chin/STM32CubeIDE/workspace_1.6.1/OpenSourceSentry/Core/BSP/Inc" -I"C:/Users/Zheng Hao Chin/STM32CubeIDE/workspace_1.6.1/OpenSourceSentry/Core/Tasks/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Tasks/Src/GimbalControlTask.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Tasks/Src/MovementControlTask.o: ../Core/Tasks/Src/MovementControlTask.c Core/Tasks/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F427xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/Zheng Hao Chin/STM32CubeIDE/workspace_1.6.1/OpenSourceSentry/Core/BSP/Inc" -I"C:/Users/Zheng Hao Chin/STM32CubeIDE/workspace_1.6.1/OpenSourceSentry/Core/Tasks/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Tasks/Src/MovementControlTask.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Tasks/Src/RefereeProcessingTask.o: ../Core/Tasks/Src/RefereeProcessingTask.c Core/Tasks/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F427xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/Zheng Hao Chin/STM32CubeIDE/workspace_1.6.1/OpenSourceSentry/Core/BSP/Inc" -I"C:/Users/Zheng Hao Chin/STM32CubeIDE/workspace_1.6.1/OpenSourceSentry/Core/Tasks/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Tasks/Src/RefereeProcessingTask.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

