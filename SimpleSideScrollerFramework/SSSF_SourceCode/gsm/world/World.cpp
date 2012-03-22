/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	World.cpp

	See World.h for a class description.
*/

#include "stdafx.h"
#include "SSSF_SourceCode\dataLoader\GameDataLoader.h"
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\graphics\GameGraphics.h"
#include "SSSF_SourceCode\graphics\RenderList.h"
#include "SSSF_SourceCode\graphics\TextureManager.h"
#include "SSSF_SourceCode\gsm\sprite\SpriteManager.h"
#include "SSSF_SourceCode\gsm\state\GameStateManager.h"
#include "SSSF_SourceCode\gsm\world\World.h"
#include "SSSF_SourceCode\gui\Viewport.h"

/*
	World - Default Constructor, it constructs the layers
	vector, allowing new layers to be added.
*/
World::World()	
{
	layers = new vector<WorldLayer*>();
	worldWidth = 0;
	worldHeight = 0;
	backgroundID = -1;
	objectiveSeeker = NULL;
	endOfLevelPosition = NULL;
}

/*
	~World - This destructor will remove the memory allocated
	for the layer vector.
*/
World::~World()	
{
	delete layers;
	delete objectiveSeeker;
	delete endOfLevelPosition;
}

/*
	addLayer - This method is how layers are added to the World.
	These layers might be TiledLayers, SparseLayers, or 
	IsometricLayers, all of which are child classes of WorldLayer.
*/
void World::addLayer(WorldLayer *layerToAdd)
{
	layers->push_back(layerToAdd);
}

/*
	addWorldRenderItemsToRenderList - This method sends the render
	list and viewport to each of the layers such that they
	may fill it with RenderItems to draw.
*/
void World::addWorldRenderItemsToRenderList(Game *game)
{
	GameStateManager *gsm = game->getGSM();
	GameGUI *gui = game->getGUI();
	if (gsm->isWorldRenderable())
	{
		GameGraphics *graphics = game->getGraphics();
		RenderList *renderList = graphics->getWorldRenderList();
		Viewport *viewport = gui->getViewport();

		if (backgroundID != -1) {

			renderList->addRenderItem(backgroundID, 0, 0, 0, 255, 1024, 768);

		}

		for (unsigned int i = 0; i < layers->size(); i++)
		{
			layers->at(i)->addRenderItemsToRenderList(	renderList,
														viewport);
		}
	}
}

/*
	clear - This method removes all data from the World. It should
	be called first when a level is unloaded or changed. If it
	is not called, an application runs the risk of having lots
	of extra data sitting around that may slow the progam down.
	Or, if the world thinks a level is still active, it might add
	items to the render list using image ids that have already been
	cleared from the GameGraphics' texture manager for the world.
	That would likely result in an exception.
*/
void World::unloadWorld()
{

	vector<WorldLayer*>::iterator it = layers->begin();
	while (it != layers->end())
	{
		WorldLayer* prev = *(it);
		it++;
		delete prev;
	}
	delete layers;
	delete objectiveSeeker;
	delete endOfLevelPosition;
	backgroundID = -1;
	layers = new vector<WorldLayer*>();

	worldWidth = 0;
	worldHeight = 0;
}

/*
	update - This method should be called once per frame. Note that
	the World is for static objects, so we don't have anything
	to update as/is. But, should the need arise, one could add non-
	collidable layers to a game world and update them here. For
	example, particle systems.
*/
void World::update(Game *game)
{
	// NOTE THAT THIS METHOD IS NOT IMPLEMENTED BUT COULD BE
	// SHOULD YOU WISH TO ADD ANY NON-COLLIDABLE LAYERS WITH
	// DYNAMIC CONTENT OR PARTICLE SYSTEMS

	if (objectiveSeeker != NULL && endOfLevelPosition != NULL) {

		objectiveSeeker->verifyObjective(game);
		
		// Verify Player Position
		AnimatedSprite* p = game->getGSM()->getSpriteManager()->getPlayer();

		int x = p->getPhysicalProperties()->getX() + p->getBoundingVolume()->getX()
			+ p->getBoundingVolume()->getWidth() / 2,
			y = p->getPhysicalProperties()->getY() + p->getBoundingVolume()->getY()
			+ p->getBoundingVolume()->getHeight() / 2;

		if (endOfLevelPosition->getX() < x &&
			x < endOfLevelPosition->getX() + 64 && // TIle Width
			endOfLevelPosition->getY() < y &&
			y < endOfLevelPosition->getY() + 64 //&&
			) {//objectiveSeeker->hasCompletedObjective()) {

			game->getGSM()->nextLevel(game);

		}

	}
}

void World::setBackground(int id)
{
	backgroundID = id;
}