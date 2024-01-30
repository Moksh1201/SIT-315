// Defining the constants to whcih the light and PIR sensor is connected
const int MOTION_SENSOR_PIN = 7;  // Pin whihc will catch motion sensor's output
const int LED_PIN = 3;            // Pin for LED's input

// 	It is placed for keep check on motion sensor current state
int currState = LOW;
int prevState = LOW;

void setup() {
  Serial.begin(9600);      // This is to initialize serial communication at which rate data will be sent
  pinMode(MOTION_SENSOR_PIN, INPUT);  // This part is for ccnfigure motion sensor pin as input
  pinMode(LED_PIN, OUTPUT);          // This part is for configure LED pin as output
}

void loop() {
  // This line is to read current status of the motion sensor
  currState = digitalRead(MOTION_SENSOR_PIN);

  // This is for checking any change in motion sensor status
  if (currState == HIGH && prevState == LOW) {
    Serial.println("Movement detected");
    digitalWrite(LED_PIN, HIGH);  // To turn the LED on
  } else if (currState == LOW && prevState == HIGH) {
    Serial.println("Movement stopped");
    digitalWrite(LED_PIN, LOW);   // To turn the LED off.
  }

  // This part store the current status until there is a change in it and it check continouly in loop
  prevState = currState;
}
