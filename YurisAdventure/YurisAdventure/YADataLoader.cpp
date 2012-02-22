#include "stdafx.h"

// ya GAME INCLUDES
#include "YAButtonEventHandler.h"
#include "YADataLoader.h"
#include "YAGame.h"
#include "YAKeyEventHandler.h"
#include "YATextGenerator.h"

#include "SSSF_SourceCode\text\BufferedTextFileReader.h"

// GAME OBJECT INCLUDES
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\graphics\GameGraphics.h"
#include "SSSF_SourceCode\gsm\state\GameState.h"
#include "SSSF_SourceCode\gsm\ai\RandomFloatingBot.h"
#include "SSSF_SourceCode\gsm\world\TiledLayer.h"
#include "SSSF_SourceCode\gui\Cursor.h"
#include "SSSF_SourceCode\gui\GameGUI.h"
#include "SSSF_SourceCode\gui\ScreenGUI.h"
#include "SSSF_SourceCode\input\GameInput.h"
#include "SSSF_SourceCode\os\GameOS.h"
#include "SSSF_SourceCode\text\GameText.h"

// WINDOWS PLATFORM INCLUDES
#include "SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsOS.h"
#include "SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsInput.h"
#include "SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsTimer.h"

// DIRECTX INCLUDES
#include "SSSF_SourceCode\PlatformPlugins\DirectXPlugin\DirectXGraphics.h"
#include "SSSF_SourceCode\PlatformPlugins\DirectXPlugin\DirectXTextureManager.h"

/*
	loadGame - This method loads the setup game data into the game and
	constructs all the needed objects for the game to work.
*/
void YADataLoader::loadGame(Game *game, wstring gameInitFile)
{
	// AND LET'S READ IN THE GAME SETUP INFO
	// FIRST LOAD ALL THE PROPERTIES
	map<wstring,wstring> *properties = new map<wstring,wstring>();
	loadGameProperties(game, properties, gameInitFile);

	// WE NEED THE TITLE AND USE_FULLSCREEN_MODE TO INITIALIZE OUR WINDOW
	wstring titleProp = (*properties)[DG_TITLE];
	wstring useFullscreenProp = (*properties)[DG_USE_FULLSCREEN_MODE];
	bool useFullscreen = false;
	if (useFullscreenProp.compare(L"true") == 0)
		useFullscreen = true;

	// GET THE SCREEN WIDTH AND HEIGHT
	int screenWidth, screenHeight;
	wstring screenWidthProp = (*properties)[DG_SCREEN_WIDTH];
	wstring screenHeightProp = (*properties)[DG_SCREEN_HEIGHT];
	wstringstream(screenWidthProp) >> screenWidth;
	wstringstream(screenHeightProp) >> screenHeight;

	// MAKE A CUSTOM GameOS OBJECT (WindowsOS) FOR SOME WINDOWS
	// PLATFORM STUFF, INCLUDING A Window OF COURSE
	WindowsOS *yaOS = new WindowsOS(	hInstance, 
										nCmdShow,
										useFullscreen,
										titleProp,
										screenWidth, screenHeight,
										game);
	
	int textFontSize;
	wstring textFontSizeProp = (*properties)[DG_TEXT_FONT_SIZE];
	wstringstream(textFontSizeProp) >> textFontSize;

	// RENDERING WILL BE DONE USING DirectX
	DirectXGraphics *yaGraphics = new DirectXGraphics(game);
	yaGraphics->init(screenWidth, screenHeight);
	yaGraphics->initGraphics(yaOS->getWindowHandle(), useFullscreen);
	yaGraphics->initTextFont(textFontSize);

	// AND NOW LOAD THE COLORS THE GRAPHICS WILL USE
	// AS A COLOR KEY AND FOR RENDERING TEXT
	initColors(yaGraphics, properties);

	// WE'LL USE WINDOWS PLATFORM METHODS FOR GETTING INPUT
	WindowsInput *yaInput = new WindowsInput();

	// AND WINDOWS FOR THE TIMER AS WELL
	WindowsTimer *yaTimer = new WindowsTimer();

	// NOW INITIALIZE THE Game WITH ALL THE
	// PLATFORM AND GAME SPECIFIC DATA WE JUST CREATED
	game->initPlatformPlugins(	(GameGraphics*)yaGraphics,
								(GameInput*)yaInput,
								(GameOS*)yaOS,
								(GameTimer*)yaTimer);

	// LOAD OUR CUSTOM TEXT GENERATOR, WHICH DRAWS
	// TEXT ON THE SCREEN EACH FRAME
	YATextGenerator *yaTextGenerator = new YATextGenerator();
	yaTextGenerator->initText(game);
	GameText *text = game->getText();
	text->initDebugFile(DG_DEBUG_FILE);
	text->setTextGenerator((TextGenerator*)yaTextGenerator);

	// INIT THE VIEWPORT TOO
	initViewport(game->getGUI(), properties);	

	// WE DON'T NEED THE PROPERTIES MAP ANYMORE, THE GAME IS ALL LOADED
	delete properties;
}

/*
	initColors - this helper method loads the color key, used for loading
	images, and the font color, used for rendering text.
*/
void YADataLoader::initColors(	GameGraphics *graphics,
									map<wstring,wstring> *properties)
{
	int fontRed, fontGreen, fontBlue;
	wstring fontRedProp = (*properties)[DG_FONT_COLOR_RED];
	wstring fontGreenProp = (*properties)[DG_FONT_COLOR_GREEN];
	wstring fontBlueProp = (*properties)[DG_FONT_COLOR_BLUE];
	wstringstream(fontRedProp) >> fontRed;
	wstringstream(fontGreenProp) >> fontGreen;
	wstringstream(fontBlueProp) >> fontBlue;

	// COLOR USED FOR RENDERING TEXT
	graphics->setFontColor(fontRed, fontGreen, fontBlue);

	int keyRed, keyGreen, keyBlue;
	wstring keyRedProp = (*properties)[DG_COLOR_KEY_RED];
	wstring keyGreenProp = (*properties)[DG_COLOR_KEY_GREEN];
	wstring keyBlueProp = (*properties)[DG_COLOR_KEY_BLUE];
	wstringstream(keyRedProp) >> keyRed;
	wstringstream(keyGreenProp) >> keyGreen;
	wstringstream(keyBlueProp) >> keyBlue;

	// COLOR KEY - COLOR TO BE IGNORED WHEN LOADING AN IMAGE
	// NOTE, IF YOU WISH TO USE PNG IMAGES THAT CONTAIN ALPHA
	// CHANNEL DATA, YOU DON'T NEED A COLOR KEY
	graphics->setColorKey(keyRed, keyGreen, keyBlue);
}

/*
	loadGUI - This method loads all the GUI assets described in the guiInitFile
	argument. Note that we are loading all GUI art for all GUIs when the application
	first starts. We'll learn a better technique later in the semester.
*/
void YADataLoader::loadGUI(Game *game, wstring guiInitFile)
{
	// Load From File all the GUI
    game->getGUI()->addScreenGUI(GS_MAIN_MENU, loadGUIFromFile(game, guiInitFile));
    game->getGUI()->addScreenGUI(GS_MENU_CONTROLS_MENU, loadGUIFromFile(game, mainMenuControlsGUIFile));
    game->getGUI()->addScreenGUI(GS_MENU_ABOUT_MENU, loadGUIFromFile(game, mainMenuAboutGUIFile));
	game->getGUI()->addScreenGUI(GS_PAUSED, loadGUIFromFile(game, pausedGameFile));
	game->getGUI()->addScreenGUI(GS_IN_GAME_CONTROLS, loadGUIFromFile(game, controlsGameFile));
	game->getGUI()->addScreenGUI(GS_IN_GAME_ABOUT, loadGUIFromFile(game, aboutGameFile));
	game->getGUI()->addScreenGUI(GS_SPLASH_SCREEN, loadGUIFromFile(game, splashscreenFile));

	// InGame GUI Empty - TODO
	game->getGUI()->addScreenGUI(GS_GAME_IN_PROGRESS,	loadGUIFromFile(game, inGameFile));

	// Init Cursor (Gives error if removed) - FIX
	initCursor(game->getGUI(), (DirectXTextureManager*)game->getGraphics()->getGUITextureManager());
}

/*
	loadLevel - This method should load the data the level described by the
	levelInitFile argument in to the Game's game state manager.
*/
void YADataLoader::loadWorld(Game *game, wstring levelInitFile)	
{
	// NOTE:	I AM DEMONSTRATING HOW TO LOAD A LEVEL
	//			PROGRAMICALLY. YOU SHOULD DO THIS
	//			USING CSV FILES.
	hardCodedLoadLevelExample(game);
}

/*
	initCursor - initializes a simple little cursor for the gui.
*/
void YADataLoader::initCursor(GameGUI *gui, DirectXTextureManager *guiTextureManager)
{
	// SETUP THE CURSOR
	vector<unsigned int> *imageIDs = new vector<unsigned int>();
	int imageID;

	// - FIRST LOAD THE GREEN CURSOR IMAGE
	imageID = guiTextureManager->loadTexture(DG_GREEN_CURSOR_PATH);
	imageIDs->push_back(imageID);

	// - AND NOW THE RED ONE
	imageID = guiTextureManager->loadTexture(DG_RED_CURSOR_PATH);
	imageIDs->push_back(imageID);

	// - NOW BUILD AND LOAD THE CURSOR
	Cursor *cursor = new Cursor();
	cursor->initCursor(	imageIDs,
						*(imageIDs->begin()),
						0,
						0,
						0,
						0, // Invisible Cursor
						32,
						32);
	gui->setCursor(cursor);
}

/*
	initViewport - initializes the game's viewport.
*/
void YADataLoader::initViewport(GameGUI *gui, map<wstring,wstring> *properties)
{
	// AND NOW SPECIFY THE VIEWPORT SIZE AND LOCATION. NOTE THAT IN THIS EXAMPLE,
	// WE ARE PUTTING A TOOLBAR WITH A BUTTON ACCROSS THE NORTH OF THE APPLICATION.
	// THAT TOOLBAR HAS A HEIGHT OF 64 PIXELS, SO WE'LL MAKE THAT THE OFFSET FOR
	// THE VIEWPORT IN THE Y AXIS
	Viewport *viewport = gui->getViewport();

	int viewportOffsetX, viewportOffsetY, screenWidth, screenHeight;
	wstring viewportOffsetXProp = (*properties)[DG_VIEWPORT_OFFSET_X];
	wstring viewportOffsetYProp = (*properties)[DG_VIEWPORT_OFFSET_Y];
	wstring screenWidthProp = (*properties)[DG_SCREEN_WIDTH];
	wstring screenHeightProp = (*properties)[DG_SCREEN_HEIGHT];
	wstringstream(viewportOffsetXProp) >> viewportOffsetX;
	wstringstream(viewportOffsetYProp) >> viewportOffsetY;
	wstringstream(screenWidthProp) >> screenWidth;
	wstringstream(screenHeightProp) >> screenHeight;
	int viewportWidth = screenWidth - viewportOffsetX;
	int viewportHeight = screenHeight - viewportOffsetY;
	viewport->setViewportWidth(viewportWidth);
	viewport->setViewportHeight(viewportHeight);
	viewport->setViewportOffsetX(viewportOffsetX);
	viewport->setViewportOffsetY(viewportOffsetY);
}

/*
	loadLevelExample - This method loads an example level via a hard-coded
	approach. This can be useful for testing code, but is not a practical
	solution. Game levels should be loaded from files.
*/
void YADataLoader::hardCodedLoadLevelExample(Game *game)
{
	// FIRST SETUP THE GAME WORLD DIMENSIONS
	GameStateManager *gsm = game->getGSM();
	GameGraphics *graphics = game->getGraphics();
	TextureManager *worldTextureManager = graphics->getWorldTextureManager();

	// NOTE THAT THE WORLD WILL BE THE SAME DIMENSIONS AS OUR TILED LAYER,
	// WE COULD GET A PARALLAX SCROLLING EFFECT IF WE MADE A LAYER SMALLER,
	// AND THEN SCROLLED IT SLOWER
	World *world = gsm->getWorld();
	world->setWorldWidth(NUM_COLUMNS * TILE_WIDTH);
	world->setWorldHeight(NUM_ROWS * TILE_HEIGHT);

	// Take a look which level to load
	map<wstring,wstring> *levels = new map<wstring,wstring>();
	loadGameProperties(game, levels, L"data/levels.txt");

	// Load Background

	wstring levelPath = (*levels)[L"LEVEL1_BG_PATH"];

	int oi = 0;

	int backgroundID = graphics->getWorldTextureManager()->loadTexture((*levels)[L"LEVEL1_BG_PATH"]);
	world->setBackground(backgroundID);

	// Initiate World


	world->addLayer(loadTiledLayerFromFile(game, L"data/Levels/Level1.txt", L"data/Levels/Level1_Map.txt"));

	// AND NOW LET'S MAKE A MAIN CHARACTER SPRITE
	AnimatedSpriteType *ast = new AnimatedSpriteType();
	//int spriteImageID1 = worldTextureManager->loadTexture(PLAYER_IDLE1_PATH);
	//int spriteImageID2 = worldTextureManager->loadTexture(PLAYER_IDLE2_PATH);

	// SIZE OF SPRITE IMAGES
	ast->setTextureSize(PLAYER_WIDTH, PLAYER_HEIGHT);

	// NOW LET'S ADD AN ANIMATION STATE
	// FIRST THE NAME
	ast->addAnimationSequence(IDLE_STATE_RIGHT);
	ast->addAnimationFrame(IDLE_STATE_RIGHT, worldTextureManager->loadTexture(PLAYER_IDLE_RIGHT1_PATH), 25);
	ast->addAnimationFrame(IDLE_STATE_RIGHT, worldTextureManager->loadTexture(PLAYER_IDLE_RIGHT2_PATH), 25);

	ast->addAnimationSequence(IDLE_STATE_LEFT);
	ast->addAnimationFrame(IDLE_STATE_LEFT, worldTextureManager->loadTexture(PLAYER_IDLE_LEFT1_PATH), 25);
	ast->addAnimationFrame(IDLE_STATE_LEFT, worldTextureManager->loadTexture(PLAYER_IDLE_LEFT2_PATH), 25);

	ast->addAnimationSequence(MOVING_RIGHT_STATE);
	ast->addAnimationFrame(MOVING_RIGHT_STATE, worldTextureManager->loadTexture(PLAYER_MOVING_RIGHT1_PATH), 10);
	ast->addAnimationFrame(MOVING_RIGHT_STATE, worldTextureManager->loadTexture(PLAYER_MOVING_RIGHT2_PATH), 10);
	ast->addAnimationFrame(MOVING_RIGHT_STATE, worldTextureManager->loadTexture(PLAYER_MOVING_RIGHT3_PATH), 10);

	ast->addAnimationSequence(MOVING_LEFT_STATE);
	ast->addAnimationFrame(MOVING_LEFT_STATE, worldTextureManager->loadTexture(PLAYER_MOVING_LEFT1_PATH), 10);
	ast->addAnimationFrame(MOVING_LEFT_STATE, worldTextureManager->loadTexture(PLAYER_MOVING_LEFT2_PATH), 10);
	ast->addAnimationFrame(MOVING_LEFT_STATE, worldTextureManager->loadTexture(PLAYER_MOVING_LEFT3_PATH), 10);



	SpriteManager *spriteManager = gsm->getSpriteManager();
	unsigned int spriteTypeID = spriteManager->addSpriteType(ast);
	ast->setSpriteTypeID(spriteTypeID);

	AnimatedSprite *player = spriteManager->getPlayer();
	player->setSpriteType(ast);
	PhysicalProperties *playerProps = player->getPhysicalProperties();
	playerProps->setX(10);
	playerProps->setY(10);
	playerProps->setVelocity(0.0f, 0.0f);
	playerProps->setAccelerationX(0);
	playerProps->setAccelerationY(0);

	// WE WILL SET LOTS OF OTHER PROPERTIES ONCE
	// WE START DOING COLLISIONS AND PHYSICS

	player->setAlpha(255);
	player->setCurrentState(IDLE_STATE_RIGHT);

	// Copied

	// LET'S MAKE ANOTHER TYPE OF SPRITE TO USE NOW FOR BOTS
	wstring BOT_FLOATING0_IMG = L"./textures/world/sprites/hex/Hex0.png";
	wstring BOT_FLOATING1_IMG = L"./textures/world/sprites/hex/Hex1.png";
	wstring BOT_FLOATING2_IMG = L"./textures/world/sprites/hex/Hex2.png";
	wstring BOT_FLOATING3_IMG = L"./textures/world/sprites/hex/Hex3.png";
	int BOT_WIDTH = 64;
	int BOT_HEIGHT = 64;
	wstring FLOATING_STATE = L"FLOATING_STATE";

	ast = new AnimatedSpriteType();
	vector<unsigned int> botImageIDs;
	botImageIDs.push_back(worldTextureManager->loadTexture(BOT_FLOATING0_IMG));
	botImageIDs.push_back(worldTextureManager->loadTexture(BOT_FLOATING1_IMG));
	botImageIDs.push_back(worldTextureManager->loadTexture(BOT_FLOATING2_IMG));
	botImageIDs.push_back(worldTextureManager->loadTexture(BOT_FLOATING3_IMG));
	ast->setTextureSize(BOT_WIDTH, BOT_HEIGHT);
	ast->addAnimationSequence(FLOATING_STATE);
	for (int i = 0; i < 4; i++)
		ast->addAnimationFrame(FLOATING_STATE, botImageIDs.at(i), 10);
	spriteTypeID = spriteManager->addSpriteType(ast);
	ast->setSpriteTypeID(spriteTypeID);

	// AND LET'S MAKE 100 OF THEM IN RANDOM PLACES WITH RANDOM TRANSPARENCIES
	for (int i = 0; i < 4; i++)
	{
		RandomFloatingBot *bot = new RandomFloatingBot(gsm->getPhysics(), 2, 20, 2);
		bot->setSpriteType(ast);
		bot->setCurrentState(FLOATING_STATE);
		bot->setAlpha((rand()%200) + 55);
		bot->pickRandomVelocity(gsm->getPhysics());
		PhysicalProperties *pp = bot->getPhysicalProperties();
		pp->setCollidable(false);
		int x = (i * 300) + 100;
		int y = (i * 300) + 100;
		pp->setX(x);
		pp->setY(y);
		pp->setAccelerationX(0.0f);
		pp->setAccelerationY(0.0f);
		spriteManager->addBot(bot);
	}
}

ScreenGUI* YADataLoader::loadGUIFromFile(Game *game, wstring guiFile)
{
    map<wstring,wstring> *properties = new map<wstring,wstring>();
	loadGameProperties(game, properties, guiFile);

    // Initialize All Variables
    int imageQuantity, optionQuantity, x, y, width, height;
    wstring imageQuantityS, optionQuantityS, xS, yS, widthS, heightS, path, pathUnselected, pathSelected, animated, command;

    imageQuantityS = (*properties)[GUI_QUANTITY_OF_IMAGES];
    wstringstream(imageQuantityS) >> imageQuantity;

    optionQuantityS = (*properties)[GUI_QUANTITY_OF_OPTIONS];
    wstringstream(optionQuantityS) >> optionQuantity;

    // Texture Stuff
    GameGraphics *graphics = game->getGraphics();
	DirectXTextureManager *guiTextureManager = (DirectXTextureManager*)graphics->getGUITextureManager();
    unsigned int imageID, imageID2;

    // Initialize screen
    ScreenGUI* screen = new ScreenGUI();

    // Add Images
    for ( int i = 1 ; i <= imageQuantity ; i++) {

        wstringstream stream;

        // Get Size and Position
        stream << i << GUI_IMAGE_WIDTH;       
        widthS = (*properties)[stream.str()];
        wstringstream(widthS) >> width;
        stream.str(L"");

        stream << i << GUI_IMAGE_HEIGHT;       
        heightS = (*properties)[stream.str()];
        wstringstream(heightS) >> height;
        stream.str(L"");

        stream << i << GUI_IMAGE_X;       
        xS = (*properties)[stream.str()];
        wstringstream(xS) >> x;
        stream.str(L"");

        stream << i << GUI_IMAGE_Y;       
        yS = (*properties)[stream.str()];
        wstringstream(yS) >> y;
        stream.str(L"");

        // Path and Animated

        stream << i << GUI_IMAGE_PATH;
        path = stream.str();
        stream.str(L"");

        stream << i << GUI_IMAGE_ANIMATED;
        animated = stream.str();
        stream.str(L"");

        // Create and Add Image

        imageID = guiTextureManager->loadTexture((*properties)[path]);
	    OverlayImage *imageToAdd = new OverlayImage();
	    imageToAdd->x = x;
	    imageToAdd->y = y;
	    imageToAdd->z = 0;
	    imageToAdd->alpha = 255;
        imageToAdd->width = width;
        imageToAdd->height = height;
	    imageToAdd->imageID = imageID;
        screen->addOverlayImage(imageToAdd);

    }

    Button *buttonToAdd;

    for (int i = 1 ; i <= optionQuantity ; i++) {

        wstringstream stream;

        // Get Size and Position
        stream << i << GUI_OPTION_WIDTH;       
        widthS = (*properties)[stream.str()];
        wstringstream(widthS) >> width;
        stream.str(L"");

        stream << i << GUI_OPTION_HEIGHT;       
        heightS = (*properties)[stream.str()];
        wstringstream(heightS) >> height;
        stream.str(L"");

        stream << i << GUI_OPTION_X;       
        xS = (*properties)[stream.str()];
        wstringstream(xS) >> x;
        stream.str(L"");

        stream << i << GUI_OPTION_Y;       
        yS = (*properties)[stream.str()];
        wstringstream(yS) >> y;
        stream.str(L"");

        // Path, Animated and Command

        stream << i << GUI_OPTION_PATH_UNSELECTED;
        pathUnselected = (*properties)[stream.str()];
        stream.str(L"");

        stream << i << GUI_OPTION_PATH_SELECTED;
        pathSelected = (*properties)[stream.str()];
        stream.str(L"");

        stream << i << GUI_OPTION_COMMAND;
        command = (*properties)[stream.str()];
        stream.str(L"");

        buttonToAdd = new Button();

	    // - GET THE BUTTON COMMAND AND IMAGE IDs

        imageID = guiTextureManager->loadTexture(pathUnselected);

        if (pathUnselected.compare(pathSelected) == 0) {
            imageID2 = imageID;
        } else {
            imageID2 = guiTextureManager->loadTexture(pathSelected);
        }

	    // - INIT THE BUTTON
	    buttonToAdd->initButton(imageID, 
							    imageID2, // Doesnt Change when mouse over
							    x,
							    y,
							    0,
							    255,
                                width,
                                height,
							    false,
                                command);

	    // AND NOW LOAD IT INTO A ScreenGUI
	    screen->addButton(buttonToAdd);
    }

    return screen;
}

TiledLayer* YADataLoader::loadTiledLayerFromFile(Game *game, wstring worldFile, wstring worldMapFile)
{

	map<wstring,wstring> *properties = new map<wstring,wstring>();
	loadGameProperties(game, properties, worldFile);

	// Load World File Map
	// TO-DO

	// NOW LOAD OUR TILED BACKGROUND
	TiledLayer *tiledLayer = new TiledLayer(	NUM_COLUMNS,	NUM_ROWS, 
												TILE_WIDTH,		TILE_HEIGHT, 
												0, true, 
												NUM_COLUMNS * TILE_WIDTH,
												NUM_ROWS * TILE_HEIGHT);

	map<wstring,int> *texturesID = new map<wstring,int>();

	int texturesCount, emptyTileCode;
	wstring textureCountS, elementTilePath, elementCode;

	emptyTileCode = game->getGraphics()->getWorldTextureManager()->loadTexture(L"textures/world/tiles/empty_tile.png");

	wstringstream stream;

	// Count
    textureCountS = (*properties)[L"NUMBER_OF_ELEMENTS"];
    wstringstream(textureCountS) >> texturesCount;
    stream.str(L"");

	// Load Textures
	for (int i = 1 ; i <= texturesCount ; i++) {
		stream << i << L"ELEMENT_TILE_PATH";
        elementTilePath = (*properties)[stream.str()];
		stream.str(L"");
		stream << i << L"ELEMENT_CODE";

		wstring blabla = (*properties)[stream.str()];
		int texCode = game->getGraphics()->getWorldTextureManager()->loadTexture(elementTilePath);

		// Insert the Code and ID
		(*texturesID)[(*properties)[stream.str()]] = texCode;
		stream.str(L"");
	}

	// Lets load our World

	BufferedTextFileReader reader;
	reader.initFile(worldMapFile);
	wstring line;

	int numberOfTilesTotal = 0;

	int tileCounterPerLine = 0;

	while (reader.hasMoreLines())
	{
		line = reader.getNextLine();

		int lineSize = line.size();

		tileCounterPerLine = 0;

		for (int i = 0 ; i < line.size() ; i++) {

			if (&(line[i]) == L"\n" || (line[i]) == 'N') {
				while (tileCounterPerLine != NUM_COLUMNS) {
					// Fill with Empty Tiles
					Tile *tileToAdd = new Tile();
					tileToAdd->collidable = false;
					tileToAdd->textureID = emptyTileCode;
					tiledLayer->addTile(tileToAdd);
					tileCounterPerLine++;
					numberOfTilesTotal++;
				}

				continue;
			}

		if (line[i] == ' ') {
			Tile *tileToAdd = new Tile();
			tileToAdd->collidable = false;
			tileToAdd->textureID = emptyTileCode;
			tiledLayer->addTile(tileToAdd);
			tileCounterPerLine++;
			numberOfTilesTotal++;
			continue;
		}

		Tile *tileToAdd = new Tile();

		tileToAdd->collidable = false;

		stream << line[i];
		wstring blabla = stream.str();
		tileToAdd->textureID = (*texturesID)[ stream.str() ];
		stream.str(L"");

		tiledLayer->addTile(tileToAdd);
		numberOfTilesTotal++;

		}

	}

	return tiledLayer;
}