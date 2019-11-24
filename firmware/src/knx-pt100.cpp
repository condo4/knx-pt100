#include "mcu-core/Core/Src/main.c"
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <knx.h>
#include <knx/dpt.h>

//#define DEBUG_ADC

static float paramScale[4]  = {0.0, 0.0, 0.0, 0.0};
static float paramOffset[4] = {0.0, 0.0, 0.0, 0.0};
static float paramTrig[4] = {0.0, 0.0, 0.0, 0.0};

// create named references for easy access to group objects
#define goCh1Temp     knx.getGroupObject(1)
#define goCh1Volt     knx.getGroupObject(2)
#define goCh2Temp     knx.getGroupObject(3)
#define goCh2Volt     knx.getGroupObject(4)
#define goCh3Temp     knx.getGroupObject(5)
#define goCh3Volt     knx.getGroupObject(6)
#define goCh4Temp     knx.getGroupObject(7)
#define goCh4Volt     knx.getGroupObject(8)
#define goDebugMode   knx.getGroupObject(9)


#define VREF_VALUE (3300)

__IO uint16_t ADCValue[4];
static float CurrentVoltages[4];
static float CurrentTemperature[4];

void debugCallback(GroupObject& go)
{
    printf("Debug Write\n");

    for(uint16_t ch = 0; ch < 4; ch++)
    {
        GroupObject &temp = knx.getGroupObject(1 + (ch*2));
        printf("Temperature of ch%i: %0.2f\n", ch, CurrentTemperature[ch]);
        temp.value(CurrentTemperature[ch]);

        GroupObject &volt = knx.getGroupObject(2 + (ch*2));
        printf("Voltage of ch%i: %0.2f\n", ch, CurrentVoltages[ch]);
        volt.value(CurrentVoltages[ch]);
    }
    go.value(false);
}
extern "C" {
    void EXTI15_10_IRQHandler(void);
}
void EXTI15_10_IRQHandler(void)
{
    static uint32_t lastpressed=0;
    /* USER CODE BEGIN EXTI4_15_IRQn 0 */
    if(__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_11)){
        if (HAL_GetTick() - lastpressed > 200){
            knx.toogleProgMode();
            lastpressed = HAL_GetTick();
        }
     }
     HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}

void knx_setup()
{
    // read adress table, association table, groupobject table and parameters from eeprom
    knx.readMemory();

    // print values of parameters if device is already configured
    if (knx.configured())
    {
        // register callback for reset GO
        for(int i = 0; i < 4; i++)
        {
            paramScale[i]  = knx.paramFloat(0  + (4*i));
            paramOffset[i] = knx.paramFloat(16 + (4*i));
            paramTrig[i]   = knx.paramFloat(32 + (4*i));

            printf("Scale%i: %f\n",  i + 1, paramScale[i]);
            printf("Offset%i: %f\n", i + 1, paramOffset[i]);
            printf("Trig%i: %f\n",   i + 1, paramTrig[i]);
        }

        goCh1Temp.dataPointType(DPT_Value_Temp);
        goCh2Temp.dataPointType(DPT_Value_Temp);
        goCh3Temp.dataPointType(DPT_Value_Temp);
        goCh4Temp.dataPointType(DPT_Value_Temp);
        goCh1Volt.dataPointType(DPT_Value_Volt);
        goCh2Volt.dataPointType(DPT_Value_Volt);
        goCh3Volt.dataPointType(DPT_Value_Volt);
        goCh4Volt.dataPointType(DPT_Value_Volt);
        goDebugMode.dataPointType(DPT_Enable);

        goDebugMode.callback(debugCallback);
    }

    // start the framework.
    knx.start();
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
#ifdef DEBUG_ADC
    printf("%li: New adc conversion: %i %i %i %i\n", HAL_GetTick(), ADCValue[0], ADCValue[1], ADCValue[2], ADCValue[3]);
#endif
    HAL_ADC_Stop_DMA(&hadc1);

    for(int ch=0; ch < 4; ch++)
    {
        CurrentVoltages[ch] = __HAL_ADC_CALC_DATA_TO_VOLTAGE(VREF_VALUE, ADCValue[ch], LL_ADC_RESOLUTION_12B);
        CurrentTemperature[ch] = (paramScale[ch] * CurrentVoltages[ch]) + paramOffset[ch];
#ifdef DEBUG_ADC
        printf("%i: %0.1f mV -> %0.2f °C\r\n", ch, CurrentVoltages[ch], CurrentTemperature[ch]);
#endif
    }

    /* Refresh KNX Object */
    for(uint16_t ch = 0; ch < 4; ch++)
    {
        GroupObject &temp = knx.getGroupObject(1 + (ch*2));
        float busTemp = static_cast<float>(temp.value());
        if(fabs(busTemp - CurrentTemperature[ch]) > paramTrig[ch])
        {
            printf("Temperature of ch%i changed from %0.2f to %0.2f\n", ch, busTemp, CurrentTemperature[ch]);
            temp.value(CurrentTemperature[ch]);
        }
    }
}


int main(void)
{
    static uint32_t last_adc_meas = 0;
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    HAL_GPIO_WritePin(KNX_PROG_LED_GPIO_Port, KNX_PROG_LED_Pin, GPIO_PIN_SET);
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    printf("\n\n\n______________Start device______________\n");
    knx_setup();


    last_adc_meas = HAL_GetTick();
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADCValue, hadc1.Init.NbrOfConversion);
    while (1)
    {
        knx.loop();

        if(!knx.enabled())
        {
            printf("WATCHDOG: Reboot\n");
            HAL_NVIC_SystemReset();
        }
        if(HAL_GetTick() > (last_adc_meas + 5000) || last_adc_meas >  HAL_GetTick())/* Meas each 5s */
        {
            last_adc_meas = HAL_GetTick();
            HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADCValue, hadc1.Init.NbrOfConversion);
        }
    }
}
