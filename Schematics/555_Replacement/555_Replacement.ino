//#include <avr/io.h>

#define MODE_BUTTON PB2
#define STEP_BUTTON PB1

#define CLOCK PB3

// Make sure that if a button is clicked less than the DEBOUNCE_INTERVAL_MS from 
// the last time it is ignored.
#define DEBOUNCE_INTERVAL_MS 25

volatile boolean runMode = true;        // When true we are in astable mode otherwise
                                        // in monostable mode.
bool ModeState = 0;
bool LastModeState = 0; 
unsigned int lastModeButtonClick = 0;   // Used to de-bounce the mode button
bool StepState = 0;
bool LastStepState = 0; 
unsigned int lastStepButtonClick = 0;   // Used to de-bounce the step button


void setup() {
  // Setup the CLOCK pin as an output
  pinMode(CLOCK, OUTPUT);
  // Setup both the Step and mode buttons as input
  pinMode(STEP_BUTTON, INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  // Set the initial value of the clock pin to LOW
  digitalWrite(CLOCK, LOW);
}



// Send a clock pulse with an equal duty cycle of rate (in ms).
void sendClockPulse(int rate) {
    digitalWrite(CLOCK, HIGH);
    delay(rate);
    digitalWrite(CLOCK, LOW);
    delay(rate);
}

// Main Loop
void loop() {

  // Read the potentiometer settings.  We never want to be zero so will add a minimum
  // interval (10ms in this case).  I'm also inverting the value so that when the 
  // potentiometer is at its highest resistance we're pulsing at the fastest rate
  // just to make it more intuitive.
  int rate = 10 + (1023 - analogRead(A2));

  ModeState = digitalRead(MODE_BUTTON);
  StepState = digitalRead(STEP_BUTTON);

  if (LastModeState != ModeState) {
    if ( (millis() - lastModeButtonClick) > DEBOUNCE_INTERVAL_MS) {
      if (ModeState == LOW){
        runMode = !runMode;
      }
      // Save the last time it was pressed    
      lastModeButtonClick = millis();
      LastModeState = ModeState;
    }
  }

  // Check state of the mode button.  If we are in astable mode then just pulse the clock
  if ( runMode ) {
    // Send a Clock pulse
    sendClockPulse(rate);
  } 
  else {
    if (LastStepState != StepState) {    
      if ( (millis() - lastStepButtonClick) > DEBOUNCE_INTERVAL_MS) {
        if (StepState == LOW){
          // Send a Clock Pulse
          sendClockPulse(10);
        }
        // Save the last time it was pressed 
        lastStepButtonClick = millis();
        LastStepState = StepState;
      }
    }
  }
}