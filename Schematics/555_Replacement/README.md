**Why?**

As everyone from the 2020's, I started following BenEater's youtube channel, The electronics-bug got me again heavily.

**Thoughts for food**

I didn't wanted to rely on the 555 setup like he described in his video's:
* The AVR design I'm using is only for debugging and will be exchanged in the final version by a fixed oscillator,
  => Single Step,
  => Continuous 
* 555-module electronics is known by me and good elaborate info already exists on the net.
* I wanted to learn more about AVR's also! and had spare ATTINY85's laying around... so I got started... :)

**How it started**

My design relies heavily on the setup made by "Steveggoyette" (https://github.com/stevegg/beneater_arduino_clock), but I exchanged its Arduino in the design by a Attiny85.

![image](https://github.com/user-attachments/assets/88e6bd0c-65fa-4495-9870-eaf74cfb9520)

**Code base**

The Arduino code itself is pretty straight forward (through the use of the Arduino IDE).
2 buttons and an analog input is all it takes.

For the buttons I didn't wanted to rely on external pull-down resistors.
So I rather went for the internal Pull-up resistors on Port B and switch the inputs to ground while being pushed.

Regular "one shot with debugging", check to see if we are in "Single Step" or "Continuous"-mode.
Pulse out in "Continuous"-mode comes from the potentiometer:

````
  // Read the potentiometer settings.  We never want to be zero so will add a minimum
  // interval (10ms in this case).  I'm also inverting the value so that when the 
  // potentiometer is at its highest resistance we're pulsing at the fastest rate
  // just to make it more intuitive.
  int rate = 10 + (1023 - analogRead(A2));
````

Pulse out in "Single Step"-mode comes from a fixed value (the minimum for the "continuous" -mode) so I can always use my equal  duty cycle subroutine.

=> Yeah I know, programmers are lazy... ;)


````
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
``````
I'll swing back later to get an ATTINY up and running without the use of the Arduino IDE.
