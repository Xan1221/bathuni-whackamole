#include <Servo.h>
/*Library source code zip file included in submission*/
#include <PinChangeInt.h>

Servo myServo;
int pos; //servo position

int speakerPin = 12;
int length = 2;

// assign LEDs and button to pins
int ledPin[] = {4,5,6};
int playerOneButton = 2, playerTwoButton = 3, playerThreeButton = 8;
int playerOneWhiteLED = 9, playerTwoWhiteLED = 13, playerThreeWhiteLED = 7;

// declare variables
int delayTime, delayTimeHigher, delayTimeLower;// time delay between lights on/off
int randNumber;
int difficulty;
bool firstGame = true;
int playerOneWhiteLEDOn, playerTwoWhiteLEDOn, playerThreeWhiteLEDOn;
int volatile playerOneScore = 0, playerTwoScore = 0, playerThreeScore = 0;
boolean playerOneButtonState = LOW, playerTwoButtonState = LOW, playerThreeButtonState = LOW;
int playerOneMisClicks = 0, playerTwoMisClicks = 0, playerThreeMisClicks = 0, maxMisClicks;
boolean gameOver = false, isClicked = false, settingUpGame = true;
void playerInput();
void updateScore();

//play the appropriate tone
void playTone(int myTone) {
  tone(speakerPin, myTone);
}

//Initialize the position of the servo
void setUpServo() {
  pos = 90;
  myServo.attach(10);
  myServo.write(pos);
}

//setup interrupt, button input and LED outputs
void setup() {
  Serial.begin(9600);
  Serial.println("Welcome to Wack-A-Mole");
  Serial.println("‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾");
  
  setUpServo();
  attachInterrupt(digitalPinToInterrupt(playerOneButton), playerInput, FALLING); // specify interrupt routine for P1
  attachInterrupt(digitalPinToInterrupt(playerTwoButton), playerInput, FALLING); // specify interrupt routine for P2
  attachPinChangeInterrupt(playerThreeButton, playerInput, FALLING); //specify interrupt routine for P3 using library (specified above)
  for (int i=0; i<3; i++){
    pinMode(ledPin[i], OUTPUT);
  }
  pinMode(playerOneButton, INPUT);
  pinMode(playerTwoButton, INPUT);
  pinMode(playerThreeButton, INPUT);
  pinMode(playerOneWhiteLED, OUTPUT);
  pinMode(playerTwoWhiteLED, OUTPUT);
  pinMode(playerThreeWhiteLED, OUTPUT);
  
}

//use of a servo to determine who is winning in two player mode
void updateServo() {
  if (playerOneScore > playerTwoScore && playerOneScore > playerThreeScore) { //player 1 winning
    pos = 176; //(180 causes servo to vibrate)
    myServo.write(pos);
  }
  else if (playerTwoScore > playerOneScore && playerTwoScore > playerThreeScore) { //player 2 winning
    pos = 90;
    myServo.write(pos);
  }
  else if (playerThreeScore > playerOneScore && playerThreeScore > playerTwoScore) { //player 3 winning
    pos = 0;
    myServo.write(pos);
  }
  else { //draw
    myServo.write(60);
  }
}

//display score and if a player wins game
void printScore() {
  if (playerOneScore < 10 && playerTwoScore < 10 && playerThreeScore < 10) {
    Serial.print(playerOneScore);
    Serial.print("     ");
    Serial.print(playerTwoScore);
    Serial.print("     ");
    Serial.print(playerThreeScore);
    Serial.println();
  }
  else {
      if (playerOneScore == 10) {
       Serial.println("Player one wins!");
       gameOver = true;
     }
     else if (playerTwoScore == 10) {
       Serial.println("Player two wins!");
       gameOver = true;
     }
     else if (playerThreeScore == 10) {
      Serial.println("Player three wins!");
      gameOver = true;
     }
  }
}

//loop through all lights to change state
void accessAllLights(int lightState) {
  for (int i=0; i<3; i++) {
    digitalWrite(ledPin[i], lightState);
  }
  digitalWrite(playerOneWhiteLED, lightState);
  digitalWrite(playerTwoWhiteLED, lightState);
  digitalWrite(playerThreeWhiteLED, lightState);
}

//display welcome message
void gameStartDisplay() {
  Serial.println("Let the games begin!");
  Serial.println("P1    P2    P3");
  for (int count = 0; count<2; count++) {
    accessAllLights(HIGH); //Turning all lights on
    delay(500);
    accessAllLights(LOW); //Turning all lights off
    delay(500);
  }
}

//check the end game condition
bool checkEndGame(bool gameOver) {
  if (gameOver == true) {
    return true;
  }
  else {
    return false;
  }
}

//initialising variables before game
void initialiseGame() {
  playerOneScore = 0;
  playerTwoScore = 0;
  playerThreeScore = 0;
  gameOver = false;
  settingUpGame = true;
}

//Loop through the flashing lights and then reset the game as the game is over
void endGameLoop() {
  for (int count = 0; count<3; count++) {
      accessAllLights(HIGH);
      delay(1000);
      accessAllLights(LOW);
      delay(1000);
  }
  initialiseGame();
}

//depending on which difficulty the user chooses the delayTime will vary
void lightDelay() {
  if (difficulty == 3) {
    delayTime = random(delayTimeLower,delayTimeHigher); //hard difficulty randomises delay
  }
  delay(delayTime);
}

//display the options of dificulty availiable to the user
void difficuiltyMenu() {
  Serial.println("1) Easy");
  Serial.println("2) Medium");
  Serial.println("3) Hard");
}

//displays information about difficuily selected
void printDifficultyDetails() {
  Serial.println();
  if (difficulty == 3) {
    Serial.print("Difficulty set to hard.");
    Serial.println();
    Serial.print("Delay time set between ");
    Serial.print(delayTimeHigher);
    Serial.print("ms and ");
    Serial.print(delayTimeLower);
    Serial.print("ms.");
    Serial.println();
    Serial.print("Maximum misclicks are set to ");
    Serial.print(maxMisClicks);
    Serial.print(".");
    Serial.println();
    Serial.print("The delays have been randomised.");
  }
  else {
    if (difficulty == 1) {
      Serial.print("Difficulty set to easy");
    }
    else if (difficulty == 2) {
      Serial.print("Difficulty set to medium");
    }
    Serial.println();
    Serial.print("Delay time set to ");
    Serial.print(delayTime);
    Serial.print("ms. ");
    Serial.println();
    Serial.print("Maximum misclicks are set to ");
    Serial.print(maxMisClicks);
    Serial.print(".");
  }
  Serial.println();
  Serial.println();
}

//user inputs the difficulty of their choice
int setDifficulty() {
  difficuiltyMenu();
  while (Serial.available() == 0) {}
  difficulty = Serial.parseInt();
  
  if (difficulty == 1) {
    delayTime = 1000;
    maxMisClicks = 8;
  }
  else if (difficulty == 2) {
    delayTime  = 700;
    maxMisClicks = 4;
  }
  else if (difficulty == 3) {
    delayTimeHigher = 700;
    delayTimeLower = 300;
    maxMisClicks = 2;
  }
  printDifficultyDetails();
  firstGame = false;
}


//run main program loop
void loop() {
  if (settingUpGame) {
    if (firstGame == true) {
      setDifficulty();
    }
    gameStartDisplay();
    settingUpGame = false;
  }
  
  randNumber = random(3); //select a random led number
  digitalWrite(ledPin[randNumber], HIGH); // light the LED with this number
  
  //accomodating for if the white light is off and the piezzo is on turn the piezzo off
  
  noTone(speakerPin);
  lightDelay(); //delay light on
  
  digitalWrite(ledPin[randNumber], LOW);
  isClicked = false;
 
  playerOneWhiteLEDOn = digitalRead(playerOneWhiteLED);
  playerTwoWhiteLEDOn = digitalRead(playerTwoWhiteLED);
  playerThreeWhiteLEDOn = digitalRead(playerThreeWhiteLED);

  //if whiteLED on = turn it off
  //then turn off the piezzo as it was called on in the interrupt
  if(playerOneWhiteLEDOn == HIGH) {
    digitalWrite(playerOneWhiteLED, LOW);
    noTone(speakerPin);
  }
  if (playerTwoWhiteLEDOn == HIGH) {
    digitalWrite(playerTwoWhiteLED, LOW);
    noTone(speakerPin);
  }
  if (playerThreeWhiteLEDOn == HIGH) {
    digitalWrite(playerThreeWhiteLED, LOW);
    noTone(speakerPin);
  }

  lightDelay(); //delay light off

  if(checkEndGame(gameOver) == true) { //reset
    noTone(speakerPin);
    endGameLoop();
    delay(2000);
  }
}

/*Avoids multiple button triggers*/
/*Function referenced as 1) in report*/
boolean debounceButton(boolean buttonState, int playerButton) {
  boolean stateNow = digitalRead(playerButton);
  if (buttonState != stateNow) {
    delay(2);
    stateNow = digitalRead(playerButton);
  }
}

void updateScore() {
  //check if the LED is on as well as the change in button state to accomodate for debouncing.
  //If a player reaches the button first is clicked is equal to true meaning that the other player cant score the piont ensuring no draws.
  if((digitalRead(ledPin[randNumber]) == HIGH) && (debounceButton(playerOneButtonState, playerOneButton) == LOW)) {
     playerOneScore++;
     playTone(330); //correct click tone played
     digitalWrite(playerOneWhiteLED, HIGH);
     playerOneButtonState=HIGH;
     printScore();
     updateServo();
     isClicked = true;
   }
   
   //check if the LED is off as well as the change in button state to accomodate for debouncing, check if someone clicked at the wrong time and incremnt misclicks. 
   else if ((digitalRead(ledPin[randNumber]) == LOW) && (debounceButton(playerOneButtonState, playerOneButton) == LOW)) {
     if (playerOneMisClicks == maxMisClicks) { //if too many misclicks decrement score by one
       if (playerOneScore != 0) {
         playerOneScore--;
         Serial.println("Player ONE clicked too many times!");
         printScore();
         updateServo();
       }
       playerOneMisClicks = 0; //reset misclicks
     }
     else {
      playTone(261); //incorrect click tone played
      playerOneMisClicks++;
     }
   }

   //same as above for player 2
   if((digitalRead(ledPin[randNumber]) == HIGH) && (debounceButton(playerTwoButtonState, playerTwoButton) == LOW)) {
       playerTwoScore++;
       playTone(330);
       digitalWrite(playerTwoWhiteLED, HIGH);
       playerTwoButtonState=HIGH;
       printScore();
       updateServo();
       isClicked = true;
   }
   //same as above for player 2
   else if ((digitalRead(ledPin[randNumber]) == LOW) && (debounceButton(playerTwoButtonState, playerTwoButton) == LOW)) {
     if (playerTwoMisClicks == maxMisClicks) {
       if (playerTwoScore != 0) {
         playerTwoScore--;
         Serial.println("Player TWO clicked too many times!");
         printScore();
         updateServo();
       }
       playerTwoMisClicks = 0;
     }
     else {
       playTone(261);
      playerTwoMisClicks++;
     }
   }

   //same as above for player 3
   if((digitalRead(ledPin[randNumber]) == HIGH) && (debounceButton(playerThreeButtonState, playerThreeButton) == LOW)) {
       playerThreeScore++;
       playTone(330);
       digitalWrite(playerThreeWhiteLED, HIGH);
       playerThreeButtonState=HIGH;
       printScore();
       updateServo();
       isClicked = true;
   }
   
   //same as above for player 3
   else if ((digitalRead(ledPin[randNumber]) == LOW) && (debounceButton(playerThreeButtonState, playerThreeButton) == LOW)) {
     if (playerThreeMisClicks == maxMisClicks) {
       if (playerThreeScore != 0) {
         playerThreeScore--;
         Serial.println("Player THREE clicked too many times!");
         printScore();
         updateServo();
       }
       playerThreeMisClicks = 0;
     }
     else {
       playTone(261);
      playerThreeMisClicks++;
     }
   }
}

//interrupt function
void playerInput() {
  //encapsulating everything from player input only update score if the game is set up and someone hasn't already scored the point for the light
  if ((settingUpGame == false) && (isClicked == false) && (playerOneScore!=10) && (playerTwoScore != 10) && (playerThreeScore != 10)) {
    updateScore();
  }
}
