/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== gpiointerrupt.c ========
 */


/*
 *
 * Morse Code Program
 * Charles Haines
 * Southern New Hampshire University
 * CS350
 * 11/23/2023
 *
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Timer.h>

/* Driver configuration */
#include "ti_drivers_config.h"


// Task Structure
typedef struct task {
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task;


// Only one task needed since messages don't run concurrently.
task tasks[1];

const unsigned char tasksNum = 1;
const unsigned long tasksPeriodGCD = 500;
const unsigned long periodSOS = 500;  // Period in ms
volatile bool switchMessage = false; // Flag to indicate when message should be swapped
bool messageSos = true; // True if message is SOS and false if message is OK

// Define states 
enum SOS_States { SOS_SMStart, SOS_Dot, SOS_Dash, SOS_PostChar, SOS_PostWord, OK_Dash, OK_Dot, OK_PostChar, OK_PostWord };
int TickFct_SOS(int state); // Tick function prototype


// Timer callback function
// Calls SOS Tick Function periodically.
void timerCallback(Timer_Handle myHandle, int_fast16_t status)
{
    unsigned char i;
    for (i = 0; i < tasksNum; ++i) {
        if (tasks[i].elapsedTime >= tasks[i].period) {
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            tasks[i].elapsedTime = 0;
        }
        tasks[i].elapsedTime += tasksPeriodGCD;
    }
}

// Function to initiate a 32 bit timer.
void initTimer()
{
    Timer_Handle timer0;
    Timer_Params params;

    Timer_init();

    Timer_Params_init(&params);
    params.periodUnits = Timer_PERIOD_US;
    params.period = tasksPeriodGCD * 1000; // convert ms to µs
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = timerCallback;

    timer0 = Timer_open(CONFIG_TIMER_0, &params);

    if (timer0 == NULL) {
        // Failed to initialize timer
        while(1) {}
    }

    if (Timer_start(timer0) == Timer_STATUS_ERROR) {
        // Failed to start timer
        while(1) {}
    }
}

/*
 *  ======== handleGpioButton ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_0.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 *
 *  Sets flag to switch messages when a button is pressed.
 *  Message doesn't switch until current message finishes transmitting.
 */
void handleGpioButton(uint_least8_t index)
{
    // Switch message from SOS to OK or vice versa
    switchMessage = true;
}


/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* Call driver init functions */
    GPIO_init();

    /* Configure the LED and button pins */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);


    unsigned char i = 0;

    // Set up task parameters
    tasks[i].state = SOS_SMStart;
    tasks[i].period = periodSOS;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &TickFct_SOS;

    // Set button callback functions and enable interrupts.
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, handleGpioButton);
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);
    if (CONFIG_GPIO_BUTTON_0 != CONFIG_GPIO_BUTTON_1) {
        GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
        GPIO_setCallback(CONFIG_GPIO_BUTTON_1, handleGpioButton);
        GPIO_enableInt(CONFIG_GPIO_BUTTON_1);
    }

    // Initiate the timer that runs the SOS task
    initTimer();

    return (NULL);
}


/*
 * TickFct_SOS
 *
 * SOS Tick Function
 * Displays one of two messages: SOS or OK in morse code using a green led (dash) and a red led (dot).
 */

int TickFct_SOS(int state) {

    // Local Static Variables
    static unsigned char localTicks;
    static unsigned char currentCharacter;
    static unsigned char currentSymbol;

    // If the switchMessage flag is raised, switch the message and lower the sitchMessage flag.
    if (switchMessage) {
        messageSos = !messageSos;
        switchMessage = false;
    }

    // Handle state transitions
    switch(state) {

    // Initial state. Reset index variables and proceed to either SOS or OK message
    case SOS_SMStart:
        localTicks = 0;
        currentCharacter = 0;
        currentSymbol = 0;

        if (messageSos) {
            state = SOS_Dot;
        }
        else {
            state = OK_Dash;
        }

        break;

        // SOS Dot State
        //If this is the last DOT in SOS, go to post word pause state.
        // Otherwise go to post character pause state.
    case SOS_Dot:
        if (currentCharacter == 2 && currentSymbol == 2) {
            state = SOS_PostWord;
        }
        else {
            state = SOS_PostChar;
            localTicks = 0;
        }
        break;

        // SOS_Dash State
        // If the dash was displayed for 3 periods (1500ms) go to post character pause state
    case SOS_Dash:
        if (localTicks >= 3) {
            state = SOS_PostChar;
            localTicks = 0;
        }
        break;

        // Post Character Pause State
        // Advance to the next symbol or character in SOS
    case SOS_PostChar:
        if (localTicks <= 2) {
            break;
        }
        localTicks = 0;

        if(currentCharacter == 0) {
            state = SOS_Dot;
        }
        else if (currentCharacter== 1) {
            state = SOS_Dash;
        }
        else if (currentCharacter == 2){
            state = SOS_Dot;
        }
        break;

        // SOS Post Word State
        // Go back to initial state SOS_SMStart to transmit the next word
    case SOS_PostWord:
        if (localTicks <= 6) {
            break;
        }
        state = SOS_SMStart;
        break;


    // OK Dash State
    // If this is the last symbol of the last character in OK, go to post
    // word pause state.
    // otherwise go to post character pause state
    case OK_Dash:
        if (localTicks <= 2) {
            break;
        }

        localTicks = 0;

        if (currentCharacter == 1 && currentSymbol == 2) {
            state = OK_PostWord;
        }
        else {
            state = OK_PostChar;
        }
        break;

    // OK Dot State
    // Go to post character pause state
    case OK_Dot:
        state = OK_PostChar;
        break;

    // OK post character pause state
    // Advance to the next symbol in OK
    case OK_PostChar:
        if (localTicks <= 2) {
            break;
        }

        localTicks = 0;

        if (currentCharacter == 1 && currentSymbol == 1) {
            state = OK_Dot;
        }
        else {
            state = OK_Dash;
        }
        break;

    // OK post word pause state
    // Go back to initial state SOS_SMStart to transmit the next word
    case OK_PostWord:
        if (localTicks <= 6) {
            break;
        }
        state = SOS_SMStart;
        break;

    // Execution should never reach here, but if it somehow does,
    // Restart the transmission from the beginning.
    default:
        state = SOS_SMStart;
        break;
    }


    // Handle state actions. Heavy usage of switch case fall through ahead! Exercise caution!
    switch (state) {

    // Display a DOT (Red LED 500ms)
    case SOS_Dot:
    case OK_Dot:
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
        break;

    // Display a DASH (Green LED 1500ms)
    case SOS_Dash:
    case OK_Dash:
        GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON);
        localTicks++;
        break;

    // Post character pause state for SOS
    // Turn off LEDs
    // Advances index variables
    case SOS_PostChar:
    case OK_PostChar:
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
        GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);
        localTicks++;

        if (localTicks >= 3) {
            currentSymbol++;
            if (currentSymbol == 3) {
                currentSymbol = 0;
                currentCharacter++;
            }
        }
        break;

    // SOS Post word pause state. Turn off LEDS
    case SOS_PostWord:
    case OK_PostWord:
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
        GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);
        localTicks++;
        break;
    }

    return state;
}
