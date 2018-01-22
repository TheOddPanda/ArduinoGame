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

#define PLAYER_HORIZONTAL_POS 1

#define TERRAIN_WIDTH 16
#define TERRAIN_EMPTY 0
#define TERRAIN_LOWER_BLOCK 1
#define TERRAIN_UPPER_BLOCK 2

#define PLAYER_FRAME_OFF 0
#define PLAYER_LOWER_RUN_FRAME_FIRST 1
#define PLAYER_LOWER_RUN_FRAME_SECOND 2

#define FIRST_JUMP_FRAME_PLAYER 3
#define SECOND_JUMP_FRAME_PLAYER 4
#define THIRD_JUMP_FRAME_PLAYER 5
#define FOURTH_JUMP_FRAME_PLAYER 6
#define FIFTH_JUMP_FRAME_PLAYER 7
#define SIXTH_JUMP_FRAME_PLAYER 8
#define SEVENTH_JUMP_FRAME_PLAYER 9
#define EIGTH_JUMP_FRAME_PLAYER 10

#define PLAYER_RUN_FRAME_UPPER_FIRST 11
#define PLAYER_RUN_FRAME_UPPER_SECOND 12

#define DELAY_TIME_MS 250

LiquidCrystal lcdDisplay(11, 9, 6, 5, 4, 3);
static char terrainUpper[TERRAIN_WIDTH + 1];
static char terrainLower[TERRAIN_WIDTH + 1];
static bool buttonIsPushed = false;

#define SPRITE_EMPTY_TERRAIN ' '
#define TERRAIN_WIDTH 16

void initBitmaps() {
	static byte bitmaps[] = {
		B01100,
		B01100,
		B00000,
		B01100,
		B11110,
		B01100,
		B01010,
		B10010,

		B01100,
		B01100,
		B00000,
		B01100,
		B01100,
		B01100,
		B01100,
		B01100,

		B01100,
		B01100,
		B00000,
		B11110,
		B01100,
		B01111,
		B10000,
		B00000,

		B11110,
		B01100,
		B01111,
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
		B11000
	};

	for (int i = 0; i < 7; i++) {
		lcdDisplay.createChar(i + 1, &bitmaps[i * 8]);
	}
	for (int i = 0; i < TERRAIN_WIDTH; i++) {
		terrainUpper[i] = SPRITE_EMPTY_TERRAIN;
		terrainLower[i] = SPRITE_EMPTY_TERRAIN;
	}
}

void updateTerrain(char* terrainPointer, byte newTerrain) {
	for (int i = 0; i < TERRAIN_WIDTH; i++) {
		char currentTerrain = terrainPointer[i];
		char nextTerrainBlock =
				(i == TERRAIN_WIDTH - 1) ? newTerrain : terrainPointer[i + 1];
		switch (currentTerrain) {
		case SPRITE_EMPTY_TERRAIN:
			terrainPointer[i] =
					(nextTerrainBlock == SPRITE_SOLID_TERRAIN) ?
							SPRITE_SOLID_RIGHT_TERRAIN : SPRITE_EMPTY_TERRAIN;
			break;
		case SPRITE_SOLID_TERRAIN:
			terrainPointer[i] =
					(nextTerrainBlock == SPRITE_EMPTY_TERRAIN) ?
							SPRITE_SOLID_TERRAIN_LEFT : SPRITE_SOLID_TERRAIN;
			break;
		case SPRITE_SOLID_RIGHT_TERRAIN:
			terrainPointer[i] = SPRITE_SOLID_TERRAIN;
			break;
		case SPRITE_SOLID_TERRAIN_LEFT:
			terrainPointer[i] = SPRITE_EMPTY_TERRAIN;
			break;
		}
	}
}

bool drawPlayer(byte position, char* terrainUpperPointer,
		char* terrainLowerPointer, unsigned int currentScore) {
	bool collision = false;
	char upperSave = terrainUpperPointer[PLAYER_HORIZONTAL_POS];
	char lowerSave = terrainLowerPointer[PLAYER_HORIZONTAL_POS];
	byte upper = 0;
	byte lower = 0;
	switch (position) {
	case PLAYER_FRAME_OFF:
		upper = SPRITE_EMPTY_TERRAIN;
		lower = SPRITE_EMPTY_TERRAIN;
		break;

	case PLAYER_LOWER_RUN_FRAME_FIRST:
		upper = SPRITE_EMPTY_TERRAIN;
		lower = FIRST_SPRITE_RUN;
		break;

	case PLAYER_LOWER_RUN_FRAME_SECOND:
		upper = SPRITE_EMPTY_TERRAIN;
		lower = SECOND_SPRITE_RUN;
		break;

	case FIRST_JUMP_FRAME_PLAYER:
	case EIGTH_JUMP_FRAME_PLAYER:
		upper = SPRITE_EMPTY_TERRAIN;
		lower = JUMP_SPRITE;
		break;

	case SECOND_JUMP_FRAME_PLAYER:
	case SEVENTH_JUMP_FRAME_PLAYER:
		upper = UPPER_JUMP_SPRITE;
		lower = SPRITE_JUMP_LOWER;
		break;

	case THIRD_JUMP_FRAME_PLAYER:
	case FOURTH_JUMP_FRAME_PLAYER:
	case FIFTH_JUMP_FRAME_PLAYER:
	case SIXTH_JUMP_FRAME_PLAYER:
		upper = JUMP_SPRITE;
		lower = SPRITE_EMPTY_TERRAIN;
		break;

	case PLAYER_RUN_FRAME_UPPER_FIRST:
		upper = FIRST_SPRITE_RUN;
		lower = SPRITE_EMPTY_TERRAIN;
		break;

	case PLAYER_RUN_FRAME_UPPER_SECOND:
		upper = SECOND_SPRITE_RUN;
		lower = SPRITE_EMPTY_TERRAIN;
		break;
	}

	if (upper != ' ') {
		terrainUpperPointer[PLAYER_HORIZONTAL_POS] = upper;
		collision = (upperSave == SPRITE_EMPTY_TERRAIN) ? false : true;
	}

	if (lower != ' ') {
		terrainLowerPointer[PLAYER_HORIZONTAL_POS] = lower;
		collision |= (lowerSave == SPRITE_EMPTY_TERRAIN) ? false : true;
	}

	byte digits = (int) log10((double) currentScore) + 1;

	terrainUpperPointer[TERRAIN_WIDTH] = '\0';
	terrainLowerPointer[TERRAIN_WIDTH] = '\0';
	char tmp = terrainUpperPointer[16 - digits];
	terrainUpperPointer[16 - digits] = '\0';
	lcdDisplay.setCursor(0, 0);
	lcdDisplay.print(terrainUpperPointer);
	terrainUpperPointer[16 - digits] = tmp;
	lcdDisplay.setCursor(0, 1);
	lcdDisplay.print(terrainLowerPointer);

	lcdDisplay.setCursor(16 - digits, 0);
	lcdDisplay.print(currentScore);

	terrainUpperPointer[PLAYER_HORIZONTAL_POS] = upperSave;
	terrainLowerPointer[PLAYER_HORIZONTAL_POS] = lowerSave;
	return collision;
}

void buttonPushOccured() {
	buttonIsPushed = true;
}

void setup() {
	pinMode(PIN_READ_WRITE, OUTPUT);
	digitalWrite(PIN_READ_WRITE, LOW);
	pinMode(PIN_CONTRAST, OUTPUT);
	digitalWrite(PIN_CONTRAST, LOW);
	pinMode(PIN_BUTTON, INPUT);
	digitalWrite(PIN_BUTTON, HIGH);
	pinMode(PIN_AUTOPLAY, OUTPUT);
	digitalWrite(PIN_AUTOPLAY, HIGH);

	attachInterrupt(0, buttonPushOccured, FALLING);

	initBitmaps();

	lcdDisplay.begin(16, 2);
}

void play() {
	static byte playerPosition = PLAYER_LOWER_RUN_FRAME_FIRST;
	static byte newTerrainType = TERRAIN_EMPTY;
	static byte newTerrainDuration = 1;
	static bool isPlaying = false;
	static bool isBlinking = false;
	static unsigned int score = 0;

	if (!isPlaying) {
		drawPlayer((isBlinking) ? PLAYER_FRAME_OFF : playerPosition,
				terrainUpper, terrainLower, score >> 3);
		if (isBlinking) {
			lcdDisplay.setCursor(0, 3);
			lcdDisplay.print("Press Start");
		}
		delay(DELAY_TIME_MS);
		isBlinking = !isBlinking;
		if (buttonIsPushed) {
			initBitmaps();
			playerPosition = PLAYER_LOWER_RUN_FRAME_FIRST;
			isPlaying = true;
			buttonIsPushed = false;
			score = 0;
		}
		return;
	}

	updateTerrain(terrainLower,
			newTerrainType == TERRAIN_LOWER_BLOCK ?
			SPRITE_SOLID_TERRAIN : SPRITE_EMPTY_TERRAIN);

	updateTerrain(terrainUpper,
			newTerrainType == TERRAIN_UPPER_BLOCK ?
			SPRITE_SOLID_TERRAIN : SPRITE_EMPTY_TERRAIN);

	if (--newTerrainDuration == 0) {
		if (newTerrainType == TERRAIN_EMPTY) {
			newTerrainType = (random(3) == 0) ?
			TERRAIN_UPPER_BLOCK : TERRAIN_LOWER_BLOCK;
			newTerrainDuration = 2 + random(10);
		} else {
			newTerrainType = TERRAIN_EMPTY;
			newTerrainDuration = 10 + random(10);
		}
	}

	if (buttonIsPushed) {
		if (playerPosition <= PLAYER_LOWER_RUN_FRAME_SECOND)
			playerPosition = FIRST_JUMP_FRAME_PLAYER;
		buttonIsPushed = false;
	}

	if (drawPlayer(playerPosition, terrainUpper, terrainLower, score >> 3)) {
		isPlaying = false;
	} else {
		if (playerPosition == PLAYER_LOWER_RUN_FRAME_SECOND || playerPosition == EIGTH_JUMP_FRAME_PLAYER) {
			playerPosition = PLAYER_LOWER_RUN_FRAME_FIRST;
		} else if ((playerPosition >= THIRD_JUMP_FRAME_PLAYER
				&& playerPosition <= FIFTH_JUMP_FRAME_PLAYER)
				&& terrainLower[PLAYER_HORIZONTAL_POS] != SPRITE_EMPTY_TERRAIN) {
			playerPosition = PLAYER_RUN_FRAME_UPPER_FIRST;
		} else if (playerPosition >= PLAYER_RUN_FRAME_UPPER_FIRST
				&& terrainLower[PLAYER_HORIZONTAL_POS] == SPRITE_EMPTY_TERRAIN) {
			playerPosition = FIFTH_JUMP_FRAME_PLAYER;
		} else if (playerPosition == PLAYER_RUN_FRAME_UPPER_SECOND) {
			playerPosition = PLAYER_RUN_FRAME_UPPER_FIRST;
		} else {
			playerPosition++;
		}
		score++;

		digitalWrite(PIN_AUTOPLAY,
				terrainLower[PLAYER_HORIZONTAL_POS + 2] == SPRITE_EMPTY_TERRAIN ?
						HIGH : LOW);
	}
	delay(100);
}
