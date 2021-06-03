#include <nds.h>
#include <stdio.h>
#include <nf_lib.h>
#include <math.h>
#include <iostream>
/*
#define true 1
#define false 0
#define True 1
#define False 0
*/
#define GameScreen 1
#define DebugScreen 0
using namespace std;

void saveGame(void);

// Best variables
int points = 0;
int autoclicks = 0;

int main(void) {
	// Initialization variables
	touchPosition touch;
	int lemon_pressed = false;
	int lemon_canGivePoints = true;
	int autoButton_pressed = false;
	int autoButton_canGiveClicks = true;
	int globalTimer = 0;
	char finalCharArrayTop[50];
	char finalCharArrayBottom[50];
	bool isBg1 = true;
	NF_SetRootFolder("NITROFS");
	
	// Initializing audio stuff
	soundEnable();
	NF_InitRawSoundBuffers();
	NF_LoadRawSound("bg-music", 2, 8000, 0);
	
	// Initializing stuff for the screens
	NF_Set2D(GameScreen, 0);
	NF_Set2D(DebugScreen, 0);
	
	// Initializing da background
	NF_InitTiledBgBuffers();
	NF_InitTiledBgSys(GameScreen);
	NF_InitTiledBgSys(DebugScreen);
	
	// Text Initialization
	NF_InitTextSys(GameScreen);
	NF_InitTextSys(DebugScreen);
	
	// Rendring LaTex(t)
	NF_LoadTextFont("default", "comic-sans", 256, 256, 0);
	NF_CreateTextLayer(GameScreen, 1, 0, "comic-sans");
	NF_DefineTextColor(GameScreen, 1, 0, 0, 0, 0); // White (Colour index 0)
	NF_DefineTextColor(GameScreen, 1, 1, 31, 31, 31); // Black (Colour index 1)
	NF_SetTextColor(GameScreen, 1, 1);
	
	// Init'ing le sprites
	NF_InitSpriteBuffers();
	NF_InitSpriteSys(GameScreen);
	NF_InitSpriteSys(DebugScreen);
	
	// Setting screen backgrounds
	NF_LoadTiledBg("top-screen-decor", "top-bg", 256, 256);
	NF_CreateTiledBg(DebugScreen, 3, "top-bg");
	
	NF_LoadTiledBg("bottom-screen", "bottom-bg", 256, 256);
	NF_CreateTiledBg(GameScreen, 3, "bottom-bg");
	
	NF_LoadTiledBg("bottom-screen2", "bottom-bg2", 256, 256);
	//NF_CreateTiledBg(GameScreen, 3, "bottom-bg2");
	
	/*--LOADING SPRITES--*/
	// Loading the lemon
	NF_LoadSpriteGfx("lemon-anim-new", 0, 64, 64);
	NF_LoadSpritePal("lemon-anim-new", 0);
	
	NF_VramSpriteGfx(GameScreen, 0, 0, false);
	NF_VramSpritePal(GameScreen, 0, 0);
	
	NF_CreateSprite(GameScreen, 0, 0, 0, 40, 32);//96, 64);
	NF_EnableSpriteRotScale(GameScreen, 0, 0, true);
	NF_SpriteRotScale(GameScreen, 0, 0, 384, 384);
	NF_SpriteFrame(GameScreen, 0, lemon_pressed);
	
	// Loading the autoclicker button
	NF_LoadSpriteGfx("Auto_Clicker_Button_Anim", 1, 64, 64);
	NF_LoadSpritePal("Auto_Clicker_Button_Anim", 1);
	
	NF_VramSpriteGfx(GameScreen, 1, 1, false);
	NF_VramSpritePal(GameScreen, 1, 1);
	
	NF_CreateSprite(GameScreen, 1, 1, 1, 192, 0);
	NF_SpriteFrame(GameScreen, 1, autoButton_pressed);
	
	// Starting bg music
	NF_PlayRawSound(2, 127, 64, true, 0);
	while (true) {
		// Getting player inputs
		consoleClear();
		scanKeys();
		touchRead(&touch);
		int held = keysHeld();
		int pressed = keysDown();
		
		// Hotkeys
		if (pressed & KEY_START) {
			saveGame();
		}
		if (pressed & KEY_SELECT) {
			points = 0;
			autoclicks = 0;
		}
		
		// Collision code for clicking the lemon
		if (((held & KEY_TOUCH) && (touch.px >= 40) && (touch.py >= 32) && (touch.px <= 176) && (touch.py <= 156)) || (held & KEY_A) || (held & KEY_B)) {
			lemon_pressed = true;
			NF_SpriteFrame(GameScreen, 0, lemon_pressed);
			if (lemon_canGivePoints) {
				points += 1;
				lemon_canGivePoints = false;
			}
		} else {
			lemon_pressed = false;
			lemon_canGivePoints = true;
			NF_SpriteFrame(GameScreen, 0, lemon_pressed);
		}
		
		// Collision code for clicking the autoclicker button
		if ((held & KEY_TOUCH) && (touch.px >= 192) && (touch.py >= 0) && (touch.px <= 256) && (touch.py <= 64)) {
			autoButton_pressed = true;
			NF_SpriteFrame(GameScreen, 1, autoButton_pressed);
			if (autoButton_canGiveClicks && points >= 100) {
				autoclicks += 1;
				points -= 100;
				autoButton_canGiveClicks = false;
			}
		} else {
			autoButton_pressed = false;
			autoButton_canGiveClicks = true;
			NF_SpriteFrame(GameScreen, 1, autoButton_pressed);
		}
		
		// Autoclicker handling
		if (globalTimer % 60 == 0) {
			points += autoclicks;
		}
		
		// Handling the background bg anim thingy
		if (globalTimer % 30 == 0) {
			isBg1 = !isBg1;	
		}
		
		if (isBg1) {
			NF_DeleteTiledBg(GameScreen, 3);
			NF_CreateTiledBg(GameScreen, 3, "bottom-bg");
		} else {
			NF_DeleteTiledBg(GameScreen, 3);
			NF_CreateTiledBg(GameScreen, 3, "bottom-bg2");
		}
		
		// Doing the text 
		NF_SetTextColor(GameScreen, 1, 0);
		sprintf(finalCharArrayTop,    "     Score: %d          ", points);
		sprintf(finalCharArrayBottom, "AutoClicks: %d          ", autoclicks);
		//sprintf(finalCharArrayTop, "Touch X: %d | Touch Y: %d----------------", touch.px, touch.py); // Debug
		NF_WriteText(GameScreen, 1, 0, 0, finalCharArrayTop);
		NF_WriteText(GameScreen, 1, 0, 1, finalCharArrayBottom);
		NF_WriteText(GameScreen, 1, 0, 22, "START = save, SELECT = reset");
		NF_WriteText(GameScreen, 1, 0, 23, "  (broken)    (doesn't save)");
		
		// Refresh display code
		NF_UpdateTextLayers();
		NF_SpriteOamSet(1);
		swiWaitForVBlank();
		oamUpdate(&oamSub);	
		globalTimer += 1;
	}
	saveGame();
	return 0;
}

void saveGame(void) {
	
}