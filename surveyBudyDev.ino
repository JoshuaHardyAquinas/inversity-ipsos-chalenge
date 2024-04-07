//libraries
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//i2c screen setup
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//usds
int trigPin = 5;
int echoPin = 4;
int pingTravelTime;
int distance;
int dtm = 10;

//menus
int currentScreen = 0;
const char* Survey[][5][7]{
  {
    { "3" },
    
    { "2", "preference A-D", "question here", "A. 40", "b. 35", "C. 30", "D. 45" },
    { "3", "your age: ", "years" },
    { "3", "your hieght: ", "cm" },
    { "1", "scale of 1-9", "hows the survey?" },
  },
  {
    { "1" },
    { "3", "your age: " },
  }
};

int dt1 = 2000;
//keypadSetup
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'U' },
  { '4', '5', '6', 'D' },
  { '7', '8', '9', 'F' },
  { 'C', '0', '=', 'B' }
};

byte rowPins[ROWS] = { 13, 12, 11, 10 };
byte colPins[COLS] = { 9, 8, 7, 6 };
Keypad keyPad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char padInput;

//survey setup
String replies = "test";

void setup() {
  //welcome user
  lcd.begin(2, 16);
  lcd.print("Welcome!");
  lcd.setCursor(0, 1);
  lcd.print("Survey buddy v1");
  delay(dt1);
  lcd.clear();
  lcd.print("Brought to you");
  lcd.setCursor(0, 1);
  lcd.print("By Joshua Hardy!");
  delay(dt1);
  lcd.clear();
  //initialise pins and communication
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
void loop() {//main loop
  const char* homeScreen[][2] = {//screens for main menu
    { "surveys", "choose a survey!" },
    { "Admin panel" }
  };
  menuChange(homeScreen[currentScreen][0], homeScreen[currentScreen][1], 1);
  if (padInput == 'F') {//enter into sub menu
    int MenuScreen = currentScreen;
    currentScreen = 0;
    switch (MenuScreen) {
      case 0:
        surveyMenu();
        break;
      case 1:
        adminPannel();
        break;
    }
  }
}
void surveyMenu() {
  while (padInput != 'B') {//loop till exit
    menuChange(surveyMenuScreen[currentScreen][0], surveyMenuScreen[currentScreen][1], 2);
    if (padInput == 'F') {
      survey(currentScreen);
    }
  }
  currentScreen = 0;
}
void survey(int surveyNumber) {//take survey
  replies = "survey" + surveyNumber;
  lcd.clear();
  lcd.print("survey ");
  lcd.print(surveyNumber);
  lcd.print(" selected");
  lcd.setCursor(0, 1);
  lcd.print("answer here");
  delay(dt1);
  for (int CurrentQuestion = 1; CurrentQuestion < String(Survey[surveyNumber][0][0]).toInt() + 1; CurrentQuestion++) {//loop till max question count
    lcd.clear();
    lcd.print("question:");
    lcd.setCursor(0, 1);
    lcd.print(CurrentQuestion);
    delay(dt1);
    bool answerValid = false;
    switch (String(Survey[surveyNumber][CurrentQuestion][0]).toInt()) {//switch though each question type defined at start of question
      case 1:
        //1-9 question
        lcd.clear();
        lcd.print(Survey[surveyNumber][CurrentQuestion][1]);
        lcd.setCursor(0, 1);
        lcd.print(Survey[surveyNumber][CurrentQuestion][2]);
        delay(dt1);
        lcd.clear();
        lcd.print("your answer:");
        padInput = keyPad.waitForKey();
        replies = replies + "," + surveyNumber + "," + CurrentQuestion + "," + padInput;
        break;
      case 2:
        //A/B/C/D question
        for (int choices = 1; choices < 6; choices = choices + 2) {
          lcd.clear();
          lcd.print(Survey[surveyNumber][CurrentQuestion][choices]);
          lcd.setCursor(0, 1);
          lcd.print(Survey[surveyNumber][CurrentQuestion][choices + 1]);
          delay(dt1);
        }
        lcd.clear();
        lcd.print("your answer:");
        padInput = keyPad.waitForKey();
        replies = replies + "," + surveyNumber + "," + CurrentQuestion + "," + padInput;
        break;
      case 3: //distance sensor question
        distance = 0;
        lcd.clear();
        lcd.print(Survey[surveyNumber][CurrentQuestion][1]);
        lcd.setCursor(0, 1);
        lcd.print(Survey[surveyNumber][CurrentQuestion][2]);
        delay(dt1);
        while (true) {//loop till confirm value
          lcd.clear();
          lcd.print("any to measure");
          lcd.setCursor(0, 1);
          lcd.print(distance);
          lcd.setCursor(8, 1);
          lcd.print("D = exit");
          padInput = keyPad.waitForKey();
          if (padInput == 'B') {
            break;
          }
          //take reading
          digitalWrite(trigPin, LOW);
          delayMicroseconds(dtm);
          digitalWrite(trigPin, HIGH);
          delayMicroseconds(dtm);
          digitalWrite(trigPin, LOW);
          pingTravelTime = pulseIn(echoPin, HIGH);
          distance = pingTravelTime * 0.034 / 2;  //converts time to cm
        }
        replies = replies + "," + CurrentQuestion + "," + distance;//create survey file
        break;
    }
  }
}
void menuChange(char* screen1, char* screen2, int maxScreenVal) {//function to reduce repetituion of menu change code
  lcd.clear();
  lcd.print(screen1);
  lcd.setCursor(0, 1);
  lcd.print(screen2);
  padInput = keyPad.waitForKey();
  switch (padInput) {//wait fopr input
    case 'U':
      if (!currentScreen == 0) {
        currentScreen = currentScreen - 1;
      }
      break;
    case 'D':
      if (currentScreen != maxScreenVal) {
        currentScreen = currentScreen + 1;
      }
      break;
    case 'f':
      break;
  }
  delay(dt1 / 6);
}
void adminPannel() {//admin pannel to print survey to serial
  lcd.clear();
  lcd.print("press any button");
  lcd.setCursor(0, 1);
  lcd.print("to send survey");
  delay(dt1);
  keyPad.waitForKey();
  Serial.println(replies);
}