/*******************************************************************************
 * Filename:			main_loop.c
 * Revised:				Date: 2020.02.20
 * Revision:			V001
 * Description:		    Main loop function
*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "main_loop.h"
#include "state_machine.h"
#include "gpio.h"

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#define DEBOUNCE_DELAY	50

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/
volatile uint32_t eventFlags = 0;

void DebounceTimerCallback(uint8_t softwareTimerId)
{
	eventFlags |= (0x01 << softwareTimerId);
}
/*******************************************************************************
 * EVENT FLAG FUNCTIONS
 ******************************************************************************/
static void ticketButtonPressedEventFlag(void);
static void sensorCancelaAcionado(void);
static void veiculoChegou(void);

// Initial event flag jump table
static void (*EventFlags[])(void) =
{
	veiculoChegou,
	ticketButtonPressedEventFlag,
	sensorCancelaAcionado,
};

static void ticketButtonPressedEventFlag(void)
{
	if(HAL_GPIO_ReadPin(INSERT_COIN_GPIO_Port, INSERT_COIN_Pin) == GPIO_PIN_RESET)
	{
		sStateMachine.InsertCoin();
	}
}

static void sensorCancelaAcionado(void)
{
	if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == GPIO_PIN_RESET)
	{
		sStateMachine.DispenseButtonPressed();
	}
}

void MainLoop(void)
{
    uint8_t i = 0;


    sStateMachine.Initialize();

    for(;;)
    {
        if(eventFlags != 0)
        {
            for(i = 0; i < maximumEventFlag; i++)
            {
            	// Detected event flag
                if(((eventFlags >> i) & 0x01) == 0x01)
                {
                	// Call related function through jump table
                    (*EventFlags[i])();
                    // Clear event flag
                    eventFlags &= (~(0x01 << i));
                }
            }
        }
    }
}

/*******************************************************************************
 * INTERRUPT CALLBACK
 ******************************************************************************/
/*******************************************************************************
 * @fn      HAL_GPIO_EXTI_Callback
 * @brief   EXTI line detection callback
 * @param	gpioPin
 * @return	None
 ******************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t gpioPin)
{
	switch(gpioPin)
	{
		case SENSOR_Pin:
			eventFlags = 0x02;
			break;
		case BUTTON_Pin:
			eventFlags = 0x01;
			break;
		default:
			break;
	}
}
