/*============================================================================*\
 * Garage Bot - ledTimer
 * Peter Eldred 2021-04
 * 
 * A wrapper around the ESP32 hardware timer to simplify its usage for flashing
 * LEDs
\*============================================================================*/

#include "ledTimer.h"
#include "_config.h"


portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t * hwtimer = NULL;
volatile SemaphoreHandle_t timerSemaphore;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;

void IRAM_ATTR onLEDTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
}



/**
 * Constructor
 */
LEDTimer::LEDTimer(){}


/**
 * Initialise
 */
void LEDTimer::init() {
  #ifdef SERIAL_DEBUG
  Serial.print("Initialising LED Timer...");
  #endif

  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  hwtimer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(hwtimer, &onLEDTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(hwtimer, LED_TIMER_CYCLE_MS * 1000, true);

  // Start an alarm
  timerAlarmEnable(hwtimer);
  
  #ifdef SERIAL_DEBUG
  Serial.println(" done.");
  #endif  
}

/**
 * Run
 * 
 * @param currentMillis the current milliseconds as passed down from the main loop
 */
void LEDTimer::run(unsigned long currentMillis) {
  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    uint32_t isrCount = 0, isrTime = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    portEXIT_CRITICAL(&timerMux);

    // Call the timer
    if (onTimer) {
      onTimer();
    }
  }
}
