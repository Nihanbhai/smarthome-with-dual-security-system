const int ledPin = 13;      // LED pin
const int relayPin = 7;     // Relay pin for fan control

void setup() {
  // Start regular serial communication at 9600 baud (for Bluetooth)
  Serial.begin(9600);

  // Set pins as output/input
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);


  // Initially turn off LED and relay
  digitalWrite(ledPin, LOW);
  digitalWrite(relayPin, HIGH);

  Serial.println("Bluetooth LED and Fan Control with Sensor");
  Serial.println("Send '1' to turn ON the LED");
  Serial.println("Send '0' to turn OFF the LED");
  Serial.println("Send '3' to turn ON the fan (relay)");
  Serial.println("Send '2' to turn OFF the fan (relay)");
}

void loop() {
  // Check if Bluetooth data is available via hardware serial
  if (Serial.available()) {
    char data = Serial.read();  // Read the incoming data from Bluetooth

    // Print the received data to the Serial Monitor
    Serial.print("Received: ");
    Serial.println(data);

    // Bluetooth control for LED
    if (data == '1') {
      digitalWrite(ledPin, HIGH);  // Turn LED on
      Serial.println("LED is ON");
    } else if (data == '0') {
      digitalWrite(ledPin, LOW);   // Turn LED off
      Serial.println("LED is OFF");
    }

    // Bluetooth control for relay (fan)
    if (data == '3') {
      digitalWrite(relayPin, HIGH);  // Turn fan on (relay activated)
      Serial.println("Fan is ON");
    } else if (data == '2') {
      digitalWrite(relayPin, LOW);   // Turn fan off (relay deactivated)
      Serial.println("Fan is OFF");
    }
  }
}
