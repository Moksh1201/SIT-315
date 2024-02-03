# Define constants for pins
const int motion_sen = 7;       # This pin is defined for motion sensor
const int trigger_sonic = 8;  # This pin is defined for ultrasonic sensor trigger
const int echo_sonic = 9;     # This pin is defined for ultrasonic sensor echo
const int motion_led = 3;          # This pin is defined for LED connected to motion sensor
const int sonic_led = 4;        # This pin is defined for LED based on ultrasonic distance
const int push_button = 2;               # This pin is defined for button

# these are set for motion sensor, ultrasonic sensor, and button states
int prevMotionState = LOW;         # To to store previous motion sensor state
int motionState = LOW;             # To to store current motion sensor state
int ultrasonicDistance = 0;        # To to store ultrasonic sensor distance in centimeters
bool functionsEnabled = true;      # To enable or disable functions

void setup() {
  Serial.begin(9600);               # Initialize serial communication
  pinMode(motion_sen, INPUT); # motion sensor pin as input
  pinMode(trigger_sonic, OUTPUT);  # ultrasonic trigger pin as output
  pinMode(echo_sonic, INPUT);     # ultrasonic echo pin as input
  pinMode(motion_led, OUTPUT);   # LED pin for motion sensor as output
  pinMode(sonic_led, OUTPUT); # LED pin for distance as output
  pinMode(push_button, INPUT_PULLUP); # button pin as input with pull-up resistor

  # Attach interrupt to button pin (rising edge trigger)
  attachInterrupt(digitalPinToInterrupt(push_button), handleButtonPress, RISING);
}

void loop() {
  # motion sensor current state 
  motionState = digitalRead(motion_sen);

  # Read ultrasonic sensor distance
  int newUltrasonicDistance = getUltrasonicDistance();

  # Only execute functions if enabled
  if (functionsEnabled) {
    # Handle motion detection logic
    if (motionState != prevMotionState) {
      if (motionState == HIGH) {
        Serial.println("Movement detected");
        digitalWrite(motion_led, HIGH);  # Turn on the LED for motion
      } else {
        Serial.println("Movement stopped");
        digitalWrite(motion_led, LOW);   # Turn off the LED for motion
      }
    }

    # ultrasonic sensor value
    if (newUltrasonicDistance != ultrasonicDistance) {
      ultrasonicDistance = newUltrasonicDistance;
      Serial.println("Ultrasonic distance: " + String(ultrasonicDistance) + " cm");

      # Concdition for turning on the led light
      if (ultrasonicDistance < 331) {
        digitalWrite(sonic_led, HIGH);
      } else {
        digitalWrite(sonic_led, LOW);
      }
    }
  }

  # Update previous motion state
  prevMotionState = motionState;

  delay(800);
}

# Function to handle button press interrupt
void handleButtonPress() {
  functionsEnabled = !functionsEnabled;  
  Serial.println("Functions " + String(functionsEnabled ? "enabled" : "disabled"));

  # Turn off both LEDs when functions are disabled
  if (!functionsEnabled) {
    digitalWrite(motion_led, LOW);
    digitalWrite(sonic_led, LOW);
  }
}

# Function to get ultrasonic sensor distance in centimeters
int getUltrasonicDistance() {
  digitalWrite(trigger_sonic, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_sonic, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_sonic, LOW);

  return pulseIn(echo_sonic, HIGH) * 0.034 / 2;
}
