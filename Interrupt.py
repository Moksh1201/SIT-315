#Define constants for pins
const int MOTION_SENSOR_PIN = 7;  #Pin for motion sensor
const int LED_PIN = 3;             #Pin for LED
const int BUTTON_PIN = 2;          #Pin for button

#Variables for motion sensor and button states
int motionState = LOW;             #Variable to store motion sensor state
bool functionsEnabled = true;      #Flag to enable or disable functions

void setup() {
  Serial.begin(9600);               #Initialize serial communication
  pinMode(MOTION_SENSOR_PIN, INPUT); #Set motion sensor pin as input
  pinMode(LED_PIN, OUTPUT);          #Set LED pin as output
  pinMode(BUTTON_PIN, INPUT_PULLUP); #Set button pin as input with pull-up resistor

  #Attach interrupt to button pin (rising edge trigger)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, RISING);
}

void loop() {
  #Read motion sensor state (no need for continuous loop check)
  motionState = digitalRead(MOTION_SENSOR_PIN);

  #Only execute functions if enabled
  if (functionsEnabled) {
    #In this part it handle motion detection logic
    if (motionState == HIGH) {
      Serial.println("Movement detected");
      digitalWrite(LED_PIN, HIGH);    #Turn on the LED
    } else if (motionState == LOW) {
      Serial.println("Movement stopped");
      digitalWrite(LED_PIN, LOW);     #Turn off the LED
    }
  }
  delay(800);
}

#In this part this function is to handle button press interrupt
void handleButtonPress() {
  functionsEnabled = !functionsEnabled;  #Toggle functionsEnabled flag
Serial.println("Functions " + String(functionsEnabled ? "enabled" : "disabled"));

}
