// Pin assignments for LED and potentiometer
const byte LED_PIN = 13;
const byte METER_PIN = A4;

// Timer configuration constants
const byte PRESCALER = 64;         // Prescaler value
const long CLOCK_FREQ = 16000000;  // Clock frequency of the board

void setup() {
  // Set LED pin as output
  pinMode(LED_PIN, OUTPUT);

  // Set potentiometer pin as input
  pinMode(METER_PIN, INPUT);

  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize timer with an initial frequency of 2 Hz
  startTimer(2.0);
}

void loop() {
  // To read potentiometer value to control timer frequency
  int potValue = analogRead(METER_PIN);

  // Map potentiometer value to a desired frequency range
  double timerFrequency = map(potValue, 0, 1023, 0.5, 10);

  // Update timer frequency based on potentiometer value
  startTimer(timerFrequency);

  // Adding a small delay to avoid overwhelming the serial port
  delay(100);
}

void startTimer(double timerFrequency) {
  // Disable interrupts temporarily
  noInterrupts();

  // Calculate timer counts based on frequency and clock settings
  long timerCounts = CLOCK_FREQ / (PRESCALER * timerFrequency);
  byte compareMatch = timerCounts - 1;

  // Reset timer counter
  TCNT1 = 0;

  // Set compare match value to trigger interrupt at the desired interval
  OCR1A = compareMatch;

  // Configure timer for CTC mode and set prescaler
  TCCR1A = 0;             // Clear control registers (optional)
  TCCR1B = 0;             // Clear control registers (optional)
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= PRESCALER;    // Set prescaler

  // Enable timer compare interrupt to blink the LED
  TIMSK1 |= (1 << OCIE1A);

  // Re-enable interrupts
  interrupts();
}

// Interrupt service routine for Timer1 compare match
ISR(TIMER1_COMPA_vect) {
  // Toggle the LED state on each timer interrupt
  digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
}
