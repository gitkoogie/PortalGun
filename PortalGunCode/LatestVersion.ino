/*
RDFM
https://www.arduino.cc/en/Tutorial/Foundations/ShiftOut
*/

// Rotary Encoder Inputs
#define CLK 3
#define DT 2
#define SW 4
#define FIRE 5
int counter = 0;
int currentStateCLK;
int lastStateCLK;

unsigned long lastButtonPress = 0;
int codeCounter = 0;
int code[4];

// define
void checkRotaryAndUpdateCounter();
void updateShiftRegister(int nr, int disp);

// Pin 2-8 is connected to the 7 segments of the display.
const int dataPin = 8;
const int clockPin = 9;
const int latchPin = 10;

int displayValues[5];
int iterator = 0;
// List of patterns for different digits.
// 0 = ACTIVATE LED SEGMENT
// 1 = DEACTIVATE LED SEGMENT
byte list_of_digits[11] = {
                      0b11110111, // 0
                      0b00010100, // 1
                      0b11001101, // 2
                      0b01011101, // 3
                      0b00011110, // 4
                      0b01011011, // 5
                      0b11011011, // 6
                      0b00010101, // 7
                      0b11011111, // 8
                      0b00011111, // 9
                      0b11000011 // C
                      };
                      
// List of digits to Light up
byte list_of_leds[4] = {
                      0b01111111, // Light up one display
                      0b10111111,
                      0b11011111,
                      0b11101111
                      };
/*
 * element = element in above array
 *             11=element7
 *               ------ 
 *              |      |
 * 10=element6  |      |  7=element5
 *              |5=el4 |
 *                ------
 *              |      |
 * 1=element0   |      |  4=element3
 *              |      |   
 *               ------
 *               2=element1    .3=element2
*/

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the Serial monitor
  Serial.begin(9600);
  // Initialize pins for 2 Shift Registers 
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  // Initialize pins for rotary encoder
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);
  
  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);

  // pin for portal 
  pinMode(FIRE, OUTPUT);
  
}


// the loop routine runs over and over again forever:
void loop() {
   // läs rotary
  checkRotaryAndUpdateCounter();
  // bryt ner counter
  displayValues[0] = 10;
  displayValues[2] = ((counter % 1000) - (counter % 100)) / 100;
  displayValues[2] = ((counter % 100) - (counter % 10)) / 10;
  displayValues[3] = counter % 10;
  // uppdatera 7 seg
  updateShiftRegister(displayValues[iterator],iterator);
  // kolla knappen
  if(!readButtonPress()){
    digitalWrite(FIRE , !digitalRead(FIRE));
  }
  // öka iterator för display
  iterator++;    // increment iterator
  if(iterator > 3){ // if iterator exceed displays 
    iterator = 0;
  }
  
}

// Check rotary and increment/decrement global counter
void checkRotaryAndUpdateCounter(){
  
  // Read the current state of CLK
  currentStateCLK = digitalRead(CLK);
  // If last and current state of CLK are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
    // If the DT state is different than the CLK state then
    // the encoder is rotating counter-clockwise =>  decrement
    if (digitalRead(DT) != currentStateCLK && counter > 0) {
          counter--; 
    } else if(counter < 999){
      // Clockwise => increment
      counter++;
    }
  }
  // Remember last CLK state
  lastStateCLK = currentStateCLK;
  // help debounce
  delay(1);  
}

/*  READ BUTTON */
int readButtonPress(){
  
  // Read the button state
  int btnState = digitalRead(SW);
  //If we detect LOW signal, button is pressed
  if (btnState == LOW) {
    //if 50ms have passed since last LOW pulse, it means that the
    //button has been pressed, released and pressed again
/*
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");
      code[codeCounter] = counter;
      codeCounter++;
      if(codeCounter == 5)
        codeCounter = 0;
    }
*/
    // Remember last button press event
    lastButtonPress = millis();
    // Put in a slight delay to help debounce the reading
    delay(1);
  }
  
  return btnState;
}


// Update shift registers
void updateShiftRegister(int nr, int disp){
   digitalWrite(latchPin, LOW);
   digitalWrite(clockPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, list_of_leds[disp]);
   shiftOut(dataPin, clockPin, LSBFIRST, list_of_digits[nr]);
   digitalWrite(latchPin, HIGH);
}
