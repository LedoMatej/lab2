#include <avr/sleep.h>    // Sleep library
#include <avr/power.h>    // Power management library
#include <avr/wdt.h>      // Watchdog timer library

int blinkCount = 0;       // Counter for blinks
bool isAwake = true;      // Flag to control awake/sleep state
volatile int wdtCounter = 0;  // Counter for WDT interrupts
volatile bool wdtTriggered = false;  // Flag to track WDT wake-up
volatile bool buttonTriggered = false;  // Flag to track button wake-up

void setup() {
  pinMode(8, OUTPUT);      // LED pin as output
  digitalWrite(8, LOW);    // Ensure LED starts off
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);      // For debugging
  Serial.println("Setup complete");
  
  // Disable watchdog timer initially
  wdt_disable();
}

void loop() {
  if (isAwake) {
    // Blink 5 times
    if (blinkCount < 5) {
      digitalWrite(8, HIGH);
      delay(500);
      digitalWrite(8, LOW);
      delay(500);
      blinkCount++;
      Serial.print("Blink #");
      Serial.println(blinkCount);
    }
    
    // After 5 blinks, go to sleep
    if (blinkCount >= 5) {
      Serial.println("Preparing to sleep...");
      delay(100);  // Let Serial finish
      goToSleep();
    }
  }

  if (wdtTriggered) {
    Serial.println("Woke up via WDT!");
    wdtTriggered = false;
    // After waking up
    blinkCount = 0;
    isAwake = true;       
  }

  else if(buttonTriggered){
    Serial.println("Woke up via button!");
    buttonTriggered = false;
    // After waking up
    blinkCount = 0;
    isAwake = true;   
  }
}

void goToSleep() {
  isAwake = false;
  wdtCounter = 0;  // Reset WDT counter
  wdtTriggered = false;  // Reset WDT wake-up flag
  buttonTriggered = false; // Reset button wake-up flag
  
  // Setup watchdog timer for 8 seconds
  cli();  // Disable interrupts during setup
  wdt_reset();  // Reset watchdog
  MCUSR = 0;  // Clear reset flags
  WDTCSR |= (1 << WDCE) | (1 << WDE);  // Enable watchdog change
  WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);  // 8 seconds, interrupt mode
  sei();  // Re-enable interrupts

  attachInterrupt(digitalPinToInterrupt(2), wakeUp, FALLING);
  
  sleep_enable();
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  
  Serial.println("Sleeping now...");
  delay(100);  // Optional final delay for Serial
  
  sleep_cpu();  // Enter sleep mode
}


void wakeUp(){
  buttonTriggered = true;
  detachInterrupt(digitalPinToInterrupt(2));
}

// Watchdog Timer Interrupt Service Routine
ISR(WDT_vect) {
  wdtCounter++;
  if (wdtCounter >= 2) {  // Two 8-second intervals = 16 seconds
    wdt_disable();  // Disable WDT
    isAwake = true;  // Wake up
    wdtTriggered = true;  // Mark that WDT caused the wake-up
  }
}