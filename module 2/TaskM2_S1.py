// Pin assignments for LED and potentiometer
const byte LED_PIN = 13;  // LED is connected to pin 13
const byte METER_PIN = A4;  // Potentiometer is connected to analog pin A4

// Constants for configuring the timer
const byte PRESCALER = 64;  // Divides the clock frequency
const long CLOCK_FREQ = 16000000;  // Frequency of the board's clock

void setup() {
  // Setting LED pin as an output
  pinMode(LED_PIN, OUTPUT);

  // Setting potentiometer pin as an input
  pinMode(METER_PIN, INPUT);

  // Starting serial communication for debugging
  Serial.begin(9600);

  // Initializing the timer with a frequency of 2 Hz
  startTimer(2.0);
}

void loop() {
  // Reading the value from the potentiometer to control the timer frequency
  int potValue = analogRead(METER_PIN);

  // Mapping the potentiometer value to a range of frequencies
  double timerFrequency = map(potValue, 0, 1023, 0.5, 10);

  // Updating the timer frequency based on the potentiometer value
  startTimer(timerFrequency);

  // Adding a small delay to prevent overloading the serial port
  delay(100);
}

void startTimer(double timerFrequency) {
  // Temporarily disabling interrupts
  noInterrupts();

  // Calculating the timer counts based on the frequency and clock settings
  long timerCounts = CLOCK_FREQ / (PRESCALER * timerFrequency);
  byte compareMatch = timerCounts - 1;

  // Resetting the timer counter
  TCNT1 = 0;

  // Setting the compare match value to trigger an interrupt at the desired interval
  OCR1A = compareMatch;

  // Configuring the timer for CTC mode and setting the prescaler
  TCCR1A = 0;  // Clearing control registers (optional)
  TCCR1B = 0;  // Clearing control registers (optional)
  TCCR1B |= (1 << WGM12);  // CTC mode
  TCCR1B |= PRESCALER;  // Setting the prescaler

  // Enabling the timer compare interrupt to blink the LED
  TIMSK1 |= (1 << OCIE1A);

  // Re-enabling interrupts
  interrupts();
}

// Interrupt service routine for Timer1 compare match
ISR(TIMER1_COMPA_vect) {
  // Toggling the LED state on each timer interrupt
  digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
}
