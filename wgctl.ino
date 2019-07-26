#include <Wiegand.h>

WIEGAND wg;

int beepPin = 4;
int relayPin = 5;

//magic words to perform specific actions
char* toggleGarageMagic = "garageDoor31415";
char* beepTwiceMagic = "beepTwice6969";
char* beepFourMagic = "beepFour54321";
char* identifyMagic = "identify8675309";
char* garageCtlMagic = "garageCtl123";

//serial receive vars
const int maxSerialLine = 1 << 10; //1K max serial line len
char serialLineBuf[maxSerialLine]; //serial line buffer
char serialLinePos = 0;

void setup() {
  Serial.begin(230400);

  // default Wiegand Pin 2 and Pin 3 see image on README.md
  // for non UNO board, use wg.begin(pinD0, pinD1) where pinD0 and pinD1
  // are the pins connected to D0 and D1 of wiegand reader respectively.
  wg.begin();
  //wg.begin(6, 7);

  //setup beep pin
  pinMode(beepPin, OUTPUT);
  digitalWrite(beepPin, HIGH); //drive high, beep off

  //setup relay pin
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); //drive low, relay off

  //ensure serial read buffer is reset
  resetSerialLine();
}

void loop() {
  //check for entry command first
  if (wg.available())
    Serial.println(wg.getCode()); //send the code to the controller

  //handle any commands over serial
  handleSerialRX();
}

void handleSerialRX() {
  //handle any available chars
  char newChar;
  while (Serial.available() > 0) {
    //get the new char
    newChar = Serial.read();

    //if the char is a newline, we've reached the end of the line, handle it
    if (newChar == '\n')
      handleSerialLine();

    //if this the last char without a newline, trash the line
    else if (serialLinePos >= maxSerialLine - 1)
      resetSerialLine();

    //otherwise, save the new char
    else
      serialLineBuf[serialLinePos++] = newChar;
  }
}

bool match(char* str) {
  return !strcmp(serialLineBuf, str);
}

void handleSerialLine() {
  //handle each command
  if (match(toggleGarageMagic))
    toggleDoor();

  if (match(beepTwiceMagic))
    beep(2);

  if (match(beepFourMagic))
    beep(4);

  if (match(identifyMagic))
    Serial.println(garageCtlMagic); //send controller identifier

  //reset the serial line
  resetSerialLine();
}

void resetSerialLine() {
  for (int i = 0 ; i < maxSerialLine ; i++)
    serialLineBuf[i] = (char)0;

  serialLinePos = 0;
}

void toggleDoor() {
  digitalWrite(relayPin, HIGH);
  delay(500);
  digitalWrite(relayPin, LOW);
  delay(500);
}

void beep(int beeps) {
  delay(200); //wait for the device beep to end
  for (int i = 0 ; i < beeps ; i++) {
    digitalWrite(beepPin, LOW);
    delay(100);
    digitalWrite(beepPin, HIGH);
    delay(100);
  }
}

