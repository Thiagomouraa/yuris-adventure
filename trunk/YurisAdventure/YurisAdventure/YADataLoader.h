/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	DummyDataLoader.h

	This class provides a custom importer for the DummyGame to import
	game data, gui data, and world (i.e. level) data.
*/
#pragma once
#include "stdafx.h"
#include <stdio.h>
#include "SSSF_SourceCode\dataLoader\GameDataLoader.h"
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\PlatformPlugins\DirectXPlugin\DirectXTextureManager.h"
#include "SSSF_SourceCode\gui\ScreenGUI.h"

class YADataLoader : public GameDataLoader
{
private:
	// WE NEED THESE GUYS TO INIT OUR WINDOWS WINDOW
	HINSTANCE hInstance;
	int nCmdShow;

public:
	YADataLoader()	{}
	~YADataLoader()	{}

	// INLINED MUTATOR METHOD
	void initWinHandle(HINSTANCE initHInstance, int initNCmdShow)
	{
		hInstance = initHInstance;
		nCmdShow = initNCmdShow;
	}


	// THESE SHOULD BE LOADING DATA FROM FILES. THEY
	// ARE ALL DEFINED IN DummyDataLoader.cpp
	void loadGame(Game *game, wstring gameInitFile);
	void loadGUI(Game *game, wstring guiInitFile);
	void loadWorld(Game *game, wstring levelInitFile);

	// THESE ARE HARD-CODED EXAMPLES OF GAME DATA LOADING
	void hardCodedLoadLevelExample(Game *game);

	// SOME HELPER METHODS FOR SETTING UP THE GAME
	void initColors(GameGraphics *graphics,	map<wstring,wstring> *properties);
	void initViewport(GameGUI *gui, map<wstring,wstring> *properties);

	// AND SOME HELPER METHODS FOR LOADING THE GUI
	void initCursor(GameGUI *gui, DirectXTextureManager *guiTextureManager);
    
    // New Methods
    ScreenGUI* loadGUIFromFile(Game *game, wstring guiFile);
};