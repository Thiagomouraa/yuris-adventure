/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	GameInput.cpp

	See GameInput.h for a class description.
*/

#include "stdafx.h"
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\gui\Button.h"
#include "SSSF_SourceCode\gui\Cursor.h"
#include "SSSF_SourceCode\gui\GameGUI.h"
#include "SSSF_SourceCode\gui\ScreenGUI.h"
#include "SSSF_SourceCode\input\GameInput.h"
#include "SSSF_SourceCode\input\KeyEventHandler.h"
#include "SSSF_SourceCode\os\GameOS.h"

/*
	GameInput - Default constructor, it will initialize the input state variables.
*/
GameInput::GameInput()
{
	initInputState();
}

/*
	~GameInput - Destructor, it will destroy the mousePoint pointer.
*/
GameInput::~GameInput()
{
}

/*
	initInputState - This method empties all of the data about
	key presses. This would be called at the start of the application.
*/
void GameInput::initInputState()
{
	// RESET ALL KEYS
	for (int i = 0; i < 256; i++)
	{
		inputState[i].isFirstPress	= false;
		inputState[i].isPressed		= false;
		inputState[i].wasHeldDown	= false;
	}
}
