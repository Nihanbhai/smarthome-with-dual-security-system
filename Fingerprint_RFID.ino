#include <SPI.h>
#include <RFID.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Servo.h>

Servo myservo1;
Servo myservo2;
// RFID setup
#define SS_PIN 10
#define RST_PIN 9
RFID rfid(SS_PIN, RST_PIN);

// Predefined RFID card IDs (replace with your card's serial number)
String allowedCard = "217 197 143 2"; // Replace with the actual card's serial number
String allowedCard1 = "35 225 214 38"; // Replace with the actual card's serial number

String rfidCard;

// Fingerprint sensor setup
SoftwareSerial mySerial(2, 3); // RX, TX for fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Buzzer and Servo setup
const int buzzerPin = 8;    // Pin for the buzzer
const int ledPin = 13;      // Pin for the LED indicator

void setup() {
  // RFID setup
  Serial.begin(9600);
  Serial.println("Starting the RFID and Fingerprint Reader...");
  pinMode(ledPin, OUTPUT);
  SPI.begin();
  rfid.init();
  myservo1.attach(5);
  myservo2.attach(6);
  myservo1.write(180);
  myservo2.write(180);

  // Fingerprint setup
  mySerial.begin(57600);
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Fingerprint sensor not detected :(");
    while (1);  // Halt the program if fingerprint sensor not found
  }

  Serial.println("Place your finger on the sensor or use the RFID card.");
}

void loop() {
  // Check for fingerprint match
  int fingerprintStatus = finger.getImage();
  if (fingerprintStatus == FINGERPRINT_OK) {
    Serial.println("Fingerprint detected, processing...");
    fingerprintMatch();
  }

  // Check for RFID card
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " +
                 String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      Serial.print("RFID Card detected: ");
      Serial.println(rfidCard);

      // Compare RFID card with allowed cards
      if (rfidCard == allowedCard || rfidCard == allowedCard1) {
        Serial.println("Access granted: Card matched!");
        myservo2.write(90);
        delay(3000);
        myservo2.write(180);

      } else {
        Serial.println("Access denied: Card not recognized.");
      }
    }
    rfid.halt();
  }

  // Check for enrollment or deletion commands
  if (Serial.available()) {
    char option = Serial.read();
    switch (option) {
      case 'e':
        enrollFingerprint();
        break;
      case 'd':
        deleteFingerprint();
        break;
      default:
        Serial.println("Use 'e' to enroll, 'd' to delete.");
        break;
    }
  }
}

// Fingerprint match function
void fingerprintMatch() {
  int p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to process fingerprint.");
    return;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    myservo1.write(90);
    delay(3000);
    myservo1.write(180);
    
    Serial.print("Fingerprint matched! ID: ");
    Serial.println(finger.fingerID);
    Serial.print("Confidence: ");
    Serial.println(finger.confidence);
  } else {
    Serial.println("Fingerprint not recognized!");
  }
}

// Enroll a new fingerprint
void enrollFingerprint() {
  int id = 0;
  Serial.println("Enter ID (1 - 127) to enroll:");
  while (id == 0) {
    while (Serial.available() == 0); // Wait for input
    id = Serial.parseInt(); // Parse input as integer
    if (id < 1 || id > 127) {
      Serial.println("Invalid ID, please choose an ID between 1 and 127.");
      id = 0; // Reset ID and ask again
    }
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);

  // Fingerprint enrollment logic (as in your code)
  int p = -1;
  Serial.println("Place finger on the sensor");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER)
      Serial.print(".");
    else if (p == FINGERPRINT_IMAGEFAIL)
      Serial.println("Imaging error");
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
      Serial.println("Communication error");
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image");
    return;
  }

  Serial.println("Remove finger and place it again");
  delay(2000);
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  Serial.println("Place finger again");
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting second image");
    return;
  }

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Fingerprints matched!");
  } else {
    Serial.println("Fingerprints did not match");
    return;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Fingerprint enrolled successfully.");
  } else {
    Serial.println("Failed to enroll fingerprint.");
  }
}

// Delete a fingerprint by ID
void deleteFingerprint() {
  int id = 0;
  Serial.println("Enter ID to delete:");
  while (id == 0) {
    while (Serial.available() == 0); // Wait for input
    id = Serial.parseInt(); // Parse input as integer
    if (id < 1 || id > 127) {
      Serial.println("Invalid ID, please choose an ID between 1 and 127.");
      id = 0; // Reset ID and ask again
    }
  }

  if (finger.deleteModel(id) == FINGERPRINT_OK) {
    Serial.println("Fingerprint deleted");
  } else {
    Serial.println("Failed to delete fingerprint");
  }
}
