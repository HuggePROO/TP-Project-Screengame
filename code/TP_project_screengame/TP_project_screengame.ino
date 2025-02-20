/*
  File: TP_project_screengame.ino
  Author: Hugo Larsson
  Date: 2025-02-18
  Description: This game is a near identical copy to Guitar Hero but made with an Arduino Uno.
*/
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>             // Arduino reference library

// Used for creating unnasigned pointers
void placeHolder() {}

const int bRed = 2;
const int bGre = 3;
const int bYel = 4;
const int bBlu = 5;
int selectedButton = 0;
int yDraw = 10;
int yDel = 50;
int yMax = 50;
int currentNote = 1;
String currentScreen = "home";
bool homeScreenHasRun = false;
bool menuSelectHasRun = false;
bool selectHasRun = false;
bool play = false;
float p = 3.1415926;
int song[14] = {1,1,2,2,3,3,2,4,4,3,3,2,2,1};
int songLength = 14;
int songStep = 0;
void (*gate1)() = placeHolder;
void (*gate2)() = placeHolder;
void (*gate3)() = placeHolder;
void (*gateNbr[3])() = {*gate1, *gate2, *gate3};



#define TFT_DC         8
#define TFT_RST        9
#define TFT_CS         10
#define TFT_MOSI       11
#define TFT_SCLK       13

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);




void setup(void) {
  Serial.begin(9600);

  // declaring button inputs
  pinMode(bRed, INPUT);
  pinMode(bGre, INPUT);
  pinMode(bYel, INPUT);
  pinMode(bBlu, INPUT);

  tft.initR(INITR_144GREENTAB);

  // run the first screen once
  playScreen(); 

}

void loop() {
  
  //wait for any button to be pressed
  if (digitalRead(bRed) || digitalRead(bGre) || digitalRead(bYel) || digitalRead(bBlu) == HIGH) { 
  play = true;
  }

  while (play == true) {
    //run home screen once
    if(!homeScreenHasRun) {
      currentScreen = homeScreen();
      homeScreenHasRun = true;
    }
    //start the menu
    menuSelect(currentScreen);
  }
}


void playScreen() {
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(24, 24, 80, 80, 40, ST77XX_WHITE);
  tft.fillTriangle(48, 45, 48, 83, 84, 64, ST77XX_BLUE);
  tft.setCursor(30, 5);
  tft.setTextColor(ST77XX_ORANGE);
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.print("Guitar \n   Hero \n \n \n \n \n  AT HOME");
  delay(500);
  //reset all screens to be run again (currently not functioning properly)
  homeScreenHasRun = false;
  menuSelectHasRun = false;
  selectHasRun = false;
  play = false;
  selectedButton = 0;
  

}

String homeScreen() {
  tft.fillScreen(ST77XX_ORANGE);
  tft.fillRect(10, 10, 108, 30, ST77XX_WHITE);
  tft.setCursor(28, 18);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.print("LEVELS");
  tft.fillRect(10, 50, 108, 30, ST77XX_WHITE);
  tft.setCursor(17, 58);
  tft.print("Graphics");
  tft.fillTriangle(2, 109, 4, 103, 15, 108, ST77XX_RED);
  tft.fillTriangle(2, 109, 15, 108, 10, 113, ST77XX_RED);
  tft.fillTriangle(15, 116, 28, 93, 23, 90, ST77XX_RED);
  tft.fillTriangle(10, 113, 15, 116, 23, 90, ST77XX_RED);
  tft.fillTriangle(32, 116, 46, 90, 60, 116, ST77XX_GREEN);
  tft.fillTriangle(60, 90, 75, 116, 90, 90, ST77XX_YELLOW);
  tft.fillTriangle(94, 104, 108, 90, 122, 104, ST77XX_BLUE);
  tft.fillRect(98, 104, 8, 14, ST77XX_BLUE);
  tft.fillRect(111, 104, 8, 14, ST77XX_BLUE);
  tft.fillRect(106, 104, 8, 5, ST77XX_BLUE);

  //declare the amount of valid buttons on the screens (2)
  yMax = 50;
  currentScreen = "home";
  
  return currentScreen;
}



String menuSelect(String currentScreen) {
  
  //run the menu selector once
  if (!menuSelectHasRun) {

    //wait for red button to be pressed
    while (digitalRead(bRed) == LOW) {

      tft.drawRect(10, yDraw, 108, 30, ST77XX_BLUE);
      tft.drawRect(11, (yDraw + 1), 106, 28, ST77XX_BLUE);
      tft.drawRect(10, yDel, 108, 30, ST77XX_WHITE);
      tft.drawRect(11, (yDel + 1), 106, 28, ST77XX_WHITE);

      //navigate downwards
      if (digitalRead(bYel) == HIGH) {
        yDraw += 40;
        yDel = (yDraw - 40);
        selectedButton += 1;
      }

      //navigate upwards
      if (digitalRead(bGre) == HIGH) {
        yDraw -= 40;
        yDel = (yDraw + 40);
        selectedButton -= 1;

      }

      //restrict selector to move below all valid buttons
      if (yDraw > yMax) {
        yDraw = yMax;
        yDel = (yDraw - 40);
        selectedButton -= 1;
      }

      //restrict selector to move above all valid buttons
      if (yDraw < 10) {
        yDraw = 10;
        yDel = (yDraw + 40);
        selectedButton += 1;
      }

    }
  menuSelectHasRun = true;
  }

  //run appropriate function for selected button once
  if (!selectHasRun) {
    select();
    selectHasRun = true;
  }
  return;
}

void select() {

  //declare appropriate function per selected button
  if (currentScreen == "home") {
    gateNbr[0] = levelScreen;
    gateNbr[1] = playScreen;
  } else if (currentScreen == "levels"){
    gateNbr[0] = gameScreen;
    gateNbr[1] = gameScreen;
    gateNbr[2] = gameScreen;
  }

  //run appropriate function
  gateNbr[selectedButton]();
}


String levelScreen() {
  tft.fillScreen(ST77XX_GREEN);
  tft.fillRect(10, 10, 108, 30, ST77XX_WHITE);
  tft.setCursor(22, 18);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.print("LEVEL 1");
  tft.fillRect(10, 50, 108, 30, ST77XX_WHITE);
  tft.setCursor(22, 58);
  tft.print("LEVEL 2");
  tft.fillRect(10, 90, 108, 30, ST77XX_WHITE);
  tft.setTextColor(ST77XX_RED);
  tft.setCursor(34, 98);
  tft.print("TTFAF");

  //declare the amount of valid buttons on the screens (3)
  yMax = 90;
  currentScreen = "levels";

  //allow the selection to be run again
  selectHasRun = false;
  menuSelectHasRun = false;
  menuSelect(currentScreen);
}


void gameScreen() {
  tft.fillScreen(ST77XX_BLACK);
  currentNote = 1;
  songStep = 0;

  while (true) {
    //play until end of song
    while (songStep < songLength) {
      currentNote = song[songStep];
      // run until key pressed
      game();
    }
    winScreen();
  }
}

//the game drops squares in the song "twinkle twinkle little star"
void game() {

  //declaring starting positions for dropping squares
  int xPos = (currentNote - 1) * 32;
  int noteY = 0;

  //draws the current square
  while (noteY < 96) {
    tft.fillRect(xPos, noteY, 32, 32, getColor(currentNote));
    delay(10);
    tft.fillRect(xPos, noteY, 32, 32, ST77XX_BLACK);
    //moves the position the next square to be drawn down
    noteY += 5;

    //checks if correct button has been pressed
    if (isButtonPressed(currentNote)) {
      tft.fillRect(xPos, noteY, 32, 32, ST77XX_BLACK);
      //continues the song
      songStep++;
      return;
    }
  }
    gameOver();
}

//detect button press for the current note
bool isButtonPressed(int note) {
  if (note == 1 && digitalRead(bRed) == HIGH) return true;
  if (note == 2 && digitalRead(bGre) == HIGH) return true;
  if (note == 3 && digitalRead(bYel) == HIGH) return true;
  if (note == 4 && digitalRead(bBlu) == HIGH) return true;
  return false;
}

//declares appropriate color for drawn square
uint16_t getColor(int note) {
  if (note == 1) return ST77XX_RED;
  if (note == 2) return ST77XX_GREEN;
  if (note == 3) return ST77XX_YELLOW;
  if (note == 4) return ST77XX_BLUE;
}


void winScreen() {
  tft.fillScreen(ST77XX_GREEN);
  tft.setCursor(10, 44);
  tft.setTextColor(ST77XX_BLACK);
  tft.print("You Win!");
  tft.fillTriangle(94, 104, 108, 90, 122, 104, ST77XX_BLUE);
  tft.fillRect(98, 104, 8, 14, ST77XX_BLUE);
  tft.fillRect(111, 104, 8, 14, ST77XX_BLUE);
  tft.fillRect(106, 104, 8, 5, ST77XX_BLUE);

  tft.setCursor(0, 76);
  tft.setTextSize(1);
  tft.print("program will now begin self destruct mode");

  //waits until home button is pressed
  while (digitalRead(bBlu) == LOW) {}

  playScreen();
}


void gameOver() {
  tft.fillScreen(ST77XX_RED);
  tft.setCursor(10, 44);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Game Over :(");
  tft.fillTriangle(94, 104, 108, 90, 122, 104, ST77XX_BLUE);
  tft.fillRect(98, 104, 8, 14, ST77XX_BLUE);
  tft.fillRect(111, 104, 8, 14, ST77XX_BLUE);
  tft.fillRect(106, 104, 8, 5, ST77XX_BLUE);

  tft.setCursor(0, 76);
  tft.setTextSize(1);
  tft.print("program will now begin self destruct mode");

  //waits until home button is pressed
  while (digitalRead(bBlu) == LOW) {}

  playScreen();
}

