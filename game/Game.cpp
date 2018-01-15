#include <Arduino.h>
#include <LiquidCrystal.h>

#define PIN_BUTTON 2
#define PIN_AUTOPLAY 1
#define PIN_READ_WRITE 10
#define PIN_CONTRAST 12

#define FIRST_SPRITE_RUN 1
#define SECOND_SPRITE_RUN 2
#define JUMP_SPRITE 3
#define UPPER_JUMP_SPRITE '.'
#define SPRITE_JUMP_LOWER 4
#define SPRITE_EMPTY_TERRAIN ' '
#define SPRITE_SOLID_TERRAIN 5
#define SPRITE_SOLID_RIGHT_TERRAIN 6
#define SPRITE_SOLID_TERRAIN_LEFT 7

#define PLAYER_HORIZONTAL_POSITION 1

#define TERRAIN_WIDTH 16
#define TERRAIN_EMPTY 0
#define TERRAIN_LOWER_BLOCK 1
#define TERRAIN_UPPER_BLOCK 2

#define PLAYER_POSITION_OFF 0
#define PLAYER_LOWER_RUN_POSITION_FIRST 1
#define PLAYER_LOWER_RUN_POSITION_SECOND 2

#define FIRST_JUMP_POSITION_PLAYER 3
#define SECOND_JUMP_POSITION_PLAYER 4
#define THIRD_JUMP_POSITION_PLAYER 5
#define FOURTH_JUMP_POSITION_PLAYER 6
#define FIFTH_JUMP_POSITION_PLAYER 7
#define SIXTH_JUMP_POSITION_PLAYER 8
#define SEVENTH_JUMP_POSITION_PLAYER 9
#define EIGTH_JUMP_POSITION_PLAYER 10

#define PLAYER_RUN_POSITION_UPPER_FIRST 11
#define PLAYER_RUN_POSITION_UPPER_SECOND 12

LiquidCrystal lcdDisplay(11, 9, 6, 5, 4, 3);
static char terrainUpper[TERRAIN_WIDTH + 1];
static char terrainLower[TERRAIN_WIDTH + 1];
static bool buttonIsPushed = false;

#define SPRITE_EMPTY_TERRAIN ' '
#define TERRAIN_WIDTH 16

void initializeGraphics(){
  static byte graphics[] = {
    B01100,
    B01100,
    B00000,
    B01110,
    B11100,
    B01100,
    B11010,
    B10011,
    B01100,
    B01100,
    B00000,
    B01100,
    B01100,
    B01100,
    B01100,
    B01110,
    B01100,
    B01100,
    B00000,
    B11110,
    B01101,
    B11111,
    B10000,
    B00000,
    B11110,
    B01101,
    B11111,
    B10000,
    B00000,
    B00000,
    B00000,
    B00000,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
  };
  int index;
  for (index = 0; index < 7; ++index) {
	  lcdDisplay.createChar(index + 1, &graphics[index * 8]);
  }
  for (index = 0; index < TERRAIN_WIDTH; ++index) {
    terrainUpper[index] = SPRITE_EMPTY_TERRAIN;
    terrainLower[index] = SPRITE_EMPTY_TERRAIN;
  }
}

void updateTerrain(char* terrainPointer, byte newTerrain){
  for (int index = 0; index < TERRAIN_WIDTH; ++index) {
    char currentTerrain = terrainPointer[index];
    char nextTerrainBlock = (index == TERRAIN_WIDTH-1) ? newTerrain : terrainPointer[index+1];
    switch (currentTerrain){
      case SPRITE_EMPTY_TERRAIN:
        terrainPointer[index] = (nextTerrainBlock == SPRITE_SOLID_TERRAIN) ? SPRITE_SOLID_RIGHT_TERRAIN : SPRITE_EMPTY_TERRAIN;
        break;
      case SPRITE_SOLID_TERRAIN:
        terrainPointer[index] = (nextTerrainBlock == SPRITE_EMPTY_TERRAIN) ? SPRITE_SOLID_TERRAIN_LEFT : SPRITE_SOLID_TERRAIN;
        break;
      case SPRITE_SOLID_RIGHT_TERRAIN:
        terrainPointer[index] = SPRITE_SOLID_TERRAIN;
        break;
      case SPRITE_SOLID_TERRAIN_LEFT:
        terrainPointer[index] = SPRITE_EMPTY_TERRAIN;
        break;
    }
  }
}

bool drawPlayer(byte position, char* terrainUpperPointer, char* terrainLowerPointer, unsigned int currentScore) {
  bool collision = false;
  char upperSave = terrainUpperPointer[PLAYER_HORIZONTAL_POSITION];
  char lowerSave = terrainLowerPointer[PLAYER_HORIZONTAL_POSITION];
  byte upper, lower;
  switch (position) {
    case PLAYER_POSITION_OFF:
      upper = lower = SPRITE_EMPTY_TERRAIN;
      break;
    case PLAYER_LOWER_RUN_POSITION_FIRST:
      upper = SPRITE_EMPTY_TERRAIN;
      lower = FIRST_SPRITE_RUN;
      break;
    case PLAYER_LOWER_RUN_POSITION_SECOND:
      upper = SPRITE_EMPTY_TERRAIN;
      lower = SECOND_SPRITE_RUN;
      break;
    case FIRST_JUMP_POSITION_PLAYER:
    case EIGTH_JUMP_POSITION_PLAYER:
      upper = SPRITE_EMPTY_TERRAIN;
      lower = JUMP_SPRITE;
      break;
    case SECOND_JUMP_POSITION_PLAYER:
    case SEVENTH_JUMP_POSITION_PLAYER:
      upper = UPPER_JUMP_SPRITE;
      lower = SPRITE_JUMP_LOWER;
      break;
    case THIRD_JUMP_POSITION_PLAYER:
    case FOURTH_JUMP_POSITION_PLAYER:
    case FIFTH_JUMP_POSITION_PLAYER:
    case SIXTH_JUMP_POSITION_PLAYER:
      upper = JUMP_SPRITE;
      lower = SPRITE_EMPTY_TERRAIN;
      break;
    case PLAYER_RUN_POSITION_UPPER_FIRST:
      upper = FIRST_SPRITE_RUN;
      lower = SPRITE_EMPTY_TERRAIN;
      break;
    case PLAYER_RUN_POSITION_UPPER_SECOND:
      upper = SECOND_SPRITE_RUN;
      lower = SPRITE_EMPTY_TERRAIN;
      break;
  }
  if (upper != ' ') {
    terrainUpperPointer[PLAYER_HORIZONTAL_POSITION] = upper;
    collision = (upperSave == SPRITE_EMPTY_TERRAIN) ? false : true;
  }
  if (lower != ' ') {
    terrainLowerPointer[PLAYER_HORIZONTAL_POSITION] = lower;
    collision |= (lowerSave == SPRITE_EMPTY_TERRAIN) ? false : true;
  }

  byte digits = (currentScore > 9999) ? 5 : (currentScore > 999) ? 4 : (currentScore > 99) ? 3 : (currentScore > 9) ? 2 : 1;

  terrainUpperPointer[TERRAIN_WIDTH] = '\0';
  terrainLowerPointer[TERRAIN_WIDTH] = '\0';
  char tmp = terrainUpperPointer[16-digits];
  terrainUpperPointer[16-digits] = '\0';
  lcdDisplay.setCursor(0,0);
  lcdDisplay.print(terrainUpperPointer);
  terrainUpperPointer[16-digits] = tmp;
  lcdDisplay.setCursor(0,1);
  lcdDisplay.print(terrainLowerPointer);

  lcdDisplay.setCursor(16 - digits,0);
  lcdDisplay.print(currentScore);

  terrainUpperPointer[PLAYER_HORIZONTAL_POSITION] = upperSave;
  terrainLowerPointer[PLAYER_HORIZONTAL_POSITION] = lowerSave;
  return collision;
}

void buttonPushOccured() {
  buttonIsPushed = true;
}

void setup(){
  pinMode(PIN_READ_WRITE, OUTPUT);
  digitalWrite(PIN_READ_WRITE, LOW);
  pinMode(PIN_CONTRAST, OUTPUT);
  digitalWrite(PIN_CONTRAST, LOW);
  pinMode(PIN_BUTTON, INPUT);
  digitalWrite(PIN_BUTTON, HIGH);
  pinMode(PIN_AUTOPLAY, OUTPUT);
  digitalWrite(PIN_AUTOPLAY, HIGH);

  attachInterrupt(0/*PIN_BUTTON*/, buttonPushOccured, FALLING);

  initializeGraphics();

  lcdDisplay.begin(16, 2);
}

void loop(){
  static byte playerPosition = PLAYER_LOWER_RUN_POSITION_FIRST;
  static byte newTerrainType = TERRAIN_EMPTY;
  static byte newTerrainDuration = 1;
  static bool isPlaying = false;
  static bool isBlinking = false;
  static unsigned int distance = 0;

  if (!isPlaying) {
    drawPlayer((isBlinking) ? PLAYER_POSITION_OFF : playerPosition, terrainUpper, terrainLower, distance >> 3);
    if (isBlinking) {
      lcdDisplay.setCursor(0,0);
      lcdDisplay.print("Press Start");
    }
    delay(250);
    isBlinking = !isBlinking;
    if (buttonIsPushed) {
      initializeGraphics();
      playerPosition = PLAYER_LOWER_RUN_POSITION_FIRST;
      isPlaying = true;
      buttonIsPushed = false;
      distance = 0;
    }
    return;
  }

  updateTerrain(terrainLower, newTerrainType == TERRAIN_LOWER_BLOCK ? SPRITE_SOLID_TERRAIN : SPRITE_EMPTY_TERRAIN);
  updateTerrain(terrainUpper, newTerrainType == TERRAIN_UPPER_BLOCK ? SPRITE_SOLID_TERRAIN : SPRITE_EMPTY_TERRAIN);

  if (--newTerrainDuration == 0) {
    if (newTerrainType == TERRAIN_EMPTY) {
      newTerrainType = (random(3) == 0) ? TERRAIN_UPPER_BLOCK : TERRAIN_LOWER_BLOCK;
      newTerrainDuration = 2 + random(10);
    } else {
      newTerrainType = TERRAIN_EMPTY;
      newTerrainDuration = 10 + random(10);
    }
  }

  if (buttonIsPushed) {
    if (playerPosition <= PLAYER_LOWER_RUN_POSITION_SECOND) playerPosition = FIRST_JUMP_POSITION_PLAYER;
    buttonIsPushed = false;
  }

  if (drawPlayer(playerPosition, terrainUpper, terrainLower, distance >> 3)) {
    isPlaying = false;
  } else {
    if (playerPosition == PLAYER_LOWER_RUN_POSITION_SECOND || playerPosition == EIGTH_JUMP_POSITION_PLAYER) {
      playerPosition = PLAYER_LOWER_RUN_POSITION_FIRST;
    } else if ((playerPosition >= THIRD_JUMP_POSITION_PLAYER && playerPosition <= FIFTH_JUMP_POSITION_PLAYER) && terrainLower[PLAYER_HORIZONTAL_POSITION] != SPRITE_EMPTY_TERRAIN) {
      playerPosition = PLAYER_RUN_POSITION_UPPER_FIRST;
    } else if (playerPosition >= PLAYER_RUN_POSITION_UPPER_FIRST && terrainLower[PLAYER_HORIZONTAL_POSITION] == SPRITE_EMPTY_TERRAIN) {
      playerPosition = FIFTH_JUMP_POSITION_PLAYER;
    } else if (playerPosition == PLAYER_RUN_POSITION_UPPER_SECOND) {
      playerPosition = PLAYER_RUN_POSITION_UPPER_FIRST;
    } else {
      ++playerPosition;
    }
    ++distance;

    digitalWrite(PIN_AUTOPLAY, terrainLower[PLAYER_HORIZONTAL_POSITION + 2] == SPRITE_EMPTY_TERRAIN ? HIGH : LOW);
  }
  delay(100);
}
