
#include "Blit3D.h"
#include "Letter.h"
#include <iostream>
#include <conio.h>

Blit3D *blit3D = NULL;

//memory leak detection
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//GLOBAL DATA
Sprite *backgroundSprite = NULL; //a pointer to a background sprite

AngelcodeFont * pixeled = NULL;
AngelcodeFont* pixeledWhite = NULL;
AngelcodeFont* time35 = NULL;
AngelcodeFont* time35white = NULL;

//**************************************************
Sprite* BG = NULL; //a pointer to a background sprite
Sprite* backgroundStats = NULL; //a pointer to a background sprite
Sprite* backgroundGame = NULL; //a pointer to a background sprite
Sprite* youwin = NULL; //a pointer to a you win sprite
Sprite* spriteLetter = NULL; //sprite for letters to display
Sprite* gameOverMessage = NULL; //a pointer to a game over sprite
Letter* letter;
Util* util;
Position* position;
LetterPosition* play;
std::vector<LetterPosition> playable;
bool timeToDraw = false;
bool gameOver = false;
int timer = 150;
int timeLife = 600;
int highScore = 0;
int level = 0;
double elapsedTime = 0;
int currentIndex = 0;
double frames = timer;
int lives = 6;
char currentKeyPressed = ' ';
bool liveDiscount = false;
bool keyPressed= false;
int splashScreenTime = 180;
bool youwon = false;


void makeLevel()
{
	std::vector<Letter> alphabet;
	std::vector<Position> positionsOnDisplay;
	

	level++;
	
	switch (level)
	{
	case 1:
		timer = 150;	//time between letters displayed
		timeLife = 600;	//life time of a letter on display
		break;
	case 2:
		timer = 135;
		timeLife = 630;
		break;
	case 3:
		timer = 125;
		timeLife = 630;
		break;
	case 4:
		timer = 100;
		timeLife = 600;
		break;

	}


	letter = new Letter;
	util = new Util;

	alphabet = letter->getAlphabet();		//generates an alphabet

	alphabet = util->shuffle(alphabet);		//shuffles the alphabet on random
	positionsOnDisplay = position->positionOnDisplay(); //generates positions 

	positionsOnDisplay = util->shufflePos(positionsOnDisplay); // shuffles positions on random

	playable = util->createGameList(alphabet, positionsOnDisplay, timeLife); //creates an object to assign a location for display and a letter

}


void Init()
{
	//load background image
	backgroundStats = blit3D->MakeSprite(0, 40, 420, 1000, "Media\\bgStatsB.png");
	backgroundGame = blit3D->MakeSprite(620, 40, 1200, 1000, "Media\\bgGameBoard.png");
	gameOverMessage = blit3D->MakeSprite(0, 0, 500, 281, "Media\\gameOverMessage.png");
	BG = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\BG.png");
	youwin = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\youwin.png");
	//BG->Blit(1920.f/2, 1080.f / 2);
	//youwin->Blit(1920.f/2, 1080.f / 2);
	//load an Angelcode binary32 font file
	pixeled = blit3D->MakeAngelcodeFontFromBinary32("Media\\pixeled\\pixeled40Black\\pixeled40Black.bin");
	pixeledWhite = blit3D->MakeAngelcodeFontFromBinary32("Media\\pixeled\\pixeled40White\\pixeled40White.bin");
	time35 = blit3D->MakeAngelcodeFontFromBinary32("Media\\time35.bin");
	time35white = blit3D->MakeAngelcodeFontFromBinary32("Media\\time35white.bin");
	//******************************************************************************************************************
	util->InitializeRNG();
	
	makeLevel();

}



void DeInit(void)
{
	// DO NOT delete the font!
	//If we need to delete a font at runtime, call DeleteFont().

	//any sprites/fonts still allocated are freed automatically by the Blit3D object when we destroy it
}

void Update(double seconds)
{

	if (splashScreenTime > 0)	//intro sprite
	{
		splashScreenTime--;
	}
	else
	{

		if (lives > 0)
		{
			frames++;
			if (frames >= timer / 2 && level == 4)
			{
				timer = 90;
				timeLife = 490;
			}

			if (currentIndex < 30)
			{
				if (frames >= timer)
				{
					playable.at(currentIndex).status = false;
					currentIndex++;
					frames = 0;
				}
			}
			else
			{
				bool stillShow = false;
				for (int i = 0; i < 30; i++)
				{
					if (!playable.at(i).status)
						stillShow = true;
				}
				if (!stillShow)
				{
					if (level < 4)
					{
						makeLevel();
						currentIndex = 0;
					}
					else
					{
						youwon = true;
					}
					
				}


			}


			for (int i = 0; i < 30; i++)
			{

				if (playable.at(i).status == false && playable.at(i).timer > 0)
				{
					playable.at(i).timer -= seconds;
					if (playable.at(i).letter.value == currentKeyPressed)
					{
						highScore += 10;
						playable.at(i).status = true;
						keyPressed = false;
					}
					else
					{
						if (playable.at(i).letter.value != currentKeyPressed && playable.at(i).status == false && keyPressed)
						{
							keyPressed = false;
						}
					}
				}

				if (playable.at(i).status == false && playable.at(i).timer == 0)
				{
					playable.at(i).status = true;
					lives--;
				}
			}
		}
		else if(lives == 0)
				gameOver = true;

		keyPressed = false;
	}
	
	//if enter key is pressed and is game over or won will start again
	if (currentKeyPressed == '0' && gameOver)
	{
		level = 0;
		lives = 6;
		highScore = 0;
		keyPressed = false;
		gameOver = false;
		youwon = false;
		makeLevel();
	}

}

void Draw(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	//clear colour: r,g,b,a 	
	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(splashScreenTime > 0)
		BG->Blit(1920.f / 2, 1080.f / 2);
	else
	{
		//draw the background for the sections of the game/stats
		backgroundStats->Blit(620.f / 2, 1080.f / 2); 
		backgroundGame->Blit(1220.f, 1080.f / 2);
		pixeledWhite->BlitText(150, 750, "Score: " + std::to_string(highScore));
		pixeledWhite->BlitText(150, 650, "Life : " + std::to_string(lives));
		pixeledWhite->BlitText(150, 120, "Level : " + std::to_string(level));

			if (!gameOver)
			{
				for (int i = 0; i < 30; i++)
				{
					if (!playable.at(i).status) //if status true will be displayed
					{

						Letter t_letter = playable.at(i).letter;
						std::string t_value;
						t_value.assign(1, t_letter.value);
						spriteLetter = blit3D->MakeSprite(0, 0, 195, 195, t_letter.letra);
						spriteLetter->Blit(playable.at(i).position.gridPosition.x + 100, playable.at(i).position.gridPosition.y - 100);
						time35white->BlitText(playable.at(i).position.gridPosition.x + 80, playable.at(i).position.gridPosition.y - 180, std::to_string(((playable.at(i).timer) * 100) / timeLife) + "%");
					}
				}
			}
			else
			{
				gameOverMessage->Blit(1920.f / 2, 1080.f / 2);
				pixeledWhite->BlitText((1920.f / 2) - 450, (1080.f / 2) - 100, "Press Enter to play again");
				
			}
		
	
			if (youwon)
			{
				youwin->Blit(1920.f / 2, 1080.f / 2);
				//pixeledWhite->BlitText((1920.f / 2) - 450, (1080.f / 2) - 100, "Press Enter to play again");
			}
			
		
	}
	


}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence

	if (action == GLFW_RELEASE)
		keyPressed = true;

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_A:
			currentKeyPressed = 'A';
			break;
		case GLFW_KEY_B:
			currentKeyPressed = 'B';
			break;
		case GLFW_KEY_C:
			currentKeyPressed = 'C';
			break;
		case GLFW_KEY_D:
			currentKeyPressed = 'D';
			break;
		case GLFW_KEY_E:
			currentKeyPressed = 'E';
			break;
		case GLFW_KEY_F:
			currentKeyPressed = 'F';
			break;
		case GLFW_KEY_G:
			currentKeyPressed = 'G';
			break;
		case GLFW_KEY_H:
			currentKeyPressed = 'H';
			break;
		case GLFW_KEY_I:
			currentKeyPressed = 'I';
			break;
		case GLFW_KEY_J:
			currentKeyPressed = 'J';
			break;
		case GLFW_KEY_K:
			currentKeyPressed = 'K';
			break;
		case GLFW_KEY_L:
			currentKeyPressed = 'L';
			break;
		case GLFW_KEY_M:
			currentKeyPressed = 'M';
			break;
		case GLFW_KEY_N:
			currentKeyPressed = 'N';
			break;
		case GLFW_KEY_O:
			currentKeyPressed = 'O';
			break;
		case GLFW_KEY_P:
			currentKeyPressed = 'P';
			break;
		case GLFW_KEY_Q:
			currentKeyPressed = 'Q';
			break;
		case GLFW_KEY_R:
			currentKeyPressed = 'R';
			break;
		case GLFW_KEY_S:
			currentKeyPressed = 'S';
			break;
		case GLFW_KEY_T:
			currentKeyPressed = 'T';
			break;
		case GLFW_KEY_U:
			currentKeyPressed = 'U';
			break;
		case GLFW_KEY_V:
			currentKeyPressed = 'V';
			break;
		case GLFW_KEY_W:
			currentKeyPressed = 'W';
			break;
		case GLFW_KEY_X:
			currentKeyPressed = 'X';
			break;
		case GLFW_KEY_Y:
			currentKeyPressed = 'Y';
			break;
		case GLFW_KEY_Z:
			currentKeyPressed = 'Z';
			break;
		case GLFW_KEY_1:
			currentKeyPressed = '1';
			break;
		case GLFW_KEY_2:
			currentKeyPressed = '2';
			break;
		case GLFW_KEY_3:
			currentKeyPressed = '3';
			break;
		case GLFW_KEY_4:
			currentKeyPressed = '4';
			break;
		case GLFW_KEY_5:
			currentKeyPressed = '5';
			break;
		case GLFW_KEY_6:
			currentKeyPressed = '6';
			break;
		case GLFW_KEY_7:
			currentKeyPressed = '7';
			break;
		case GLFW_KEY_8:
			currentKeyPressed = '8';
			break;
		case GLFW_KEY_9:
			currentKeyPressed = '9';
		case GLFW_KEY_SPACE:
			currentKeyPressed = ' ';
			break;
		case GLFW_KEY_BACKSPACE:
			currentKeyPressed = '-';
			break;
		case GLFW_KEY_ENTER:
			currentKeyPressed = '0';
			break;
		
		}
	}



}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	blit3D = new Blit3D(Blit3DWindowModel::BORDERLESSFULLSCREEN_1080P, 640, 400);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
}