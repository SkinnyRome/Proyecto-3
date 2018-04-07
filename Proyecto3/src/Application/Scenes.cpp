#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreRenderWindow.h>

//Debug 
#ifdef _DEBUG
#include <iostream>
#endif



//Later removable
#include <OgreCamera.h>
#include <OgreNode.h>
#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <Components.h>

#include <Entity.h>
#include "InputManager.h"
#include "Game.h"
#include "Messages.h"
#include "Scenes.h"


#pragma region gameScene 

gameScene::gameScene(std::string id, Game * game) :_id(id), pGame(game){
	nMessages = -1;
}
gameScene::~gameScene()
{
	Entity * del;
	for (Entity * ent : _entities){
		del = ent;
		_entities.pop_front();
		delete ent;
	}

}
bool gameScene::updateEnts(float delta){
	for (auto ent : _entities){
		if(ent != NULL)ent->tick(delta);
	}
	return true;
} 
void gameScene::getMessage(Message * m){
	_messages.push_back(m);
}
void gameScene::addEntity(Entity * ent){
	_entities.push_back(ent);
}
void gameScene::dispatch() {
	/*We only process as many messages as we had at the start of the update.
	*Any messsage introduced as a result of reading a message is processed
	in the next frame*/
	nMessages = _messages.size();
	std::list<Message *>::iterator it =_messages.begin();
	for (int i = 0; i < nMessages && it != _messages.end(); i++, it++) {
		for (Entity * aux : _entities) {
			if((*it)->getEmmiter() != aux->getID())
				aux->getMessage(*it);
		}
	}
}
//Message disposal to use just before finishing the current scene loop.
// It uses the variable nMessages, that is set in the dispatch function.
void gameScene::clearMessageQueue() {
	
	for (int i = 0; i < nMessages; i++) {
		Message * aux = _messages.front();
		delete aux;
		_messages.pop_front();
	}
}
void gameScene::deleteEntity(std::string id){
	Entity * aux;
	bool found = false;
	for (std::list<Entity *>::iterator it = _entities.begin(); it != _entities.end() && !found;){
		if ((*it)->getID() == id){
			aux = *it;
			it = _entities.erase(it);
			delete aux;
			found = false;
		}
		else it++;
	}
}
#pragma endregion

//BASIC SCENE TO TEST SCENE IMPLEMENTATION.
//BUILDS A OGREHEAD BUT DOES NOT INCLUDE IT IN THE 
//SCENE ENTITY LIST. IT IS ONLY IN OGRE ENTITIES LIST.
//ALSO CREATS A BASIC ENTITY WITH A stringComponent attached
#pragma region basicScene
basicScene::basicScene(std::string id, Game * game): gameScene(id, game) {
	scnMgr = pGame->getRoot()->createSceneManager(Ogre::ST_GENERIC);


	//Self-explanatory methods
	cam = scnMgr->createCamera("MainCam");
	cam->setPosition(0, 0, 150);
	cam->lookAt(0, 0, -300);
	cam->setNearClipDistance(5);


	//------------------------------------------------------------------------------------------------------
	//ViewPort Addition
	vp = game->getRenderWindow()->addViewport(cam);
	vp->setBackgroundColour(Ogre::ColourValue(150, 150, 150));

	cam->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) /
		Ogre::Real(vp->getActualHeight()));
	
	
	//------------------------------------------------------------------------------------------------------
	//Scene SetUp

/*	try {
		Ogre::Entity * robot = scnMgr->createEntity("ogrehead.mesh");
		Ogre::SceneNode * robotNode = scnMgr->getRootSceneNode()->createChildSceneNode();
		robotNode->attachObject(robot);
	}
	catch (Ogre::FileNotFoundException e) {
		std::string a = e.getFullDescription();
		std::cout << a;
	}
	*/
	scnMgr->setAmbientLight(Ogre::ColourValue(.5, .5, .5));

	light = scnMgr->createLight("MainLight");
	light->setPosition(20, 80, 50);
	

	//SCENE DEBUG
	
	Entity * test1 = new Entity("test1", this);
	Entity * test2 = new Entity("test2", this);

	test2->addComponent(new stringComponent(test2));
	//test2->addComponent()
	

	test1->addComponent(new messageSendComponent(test1));
	test1->addComponent(new meshRenderComponent(Ogre::Vector3(0,0,100),"Ra.mesh", test1, scnMgr));


	addEntity(test1);
	addEntity(test2);
	
}
basicScene::~basicScene(){
	delete light;
	delete cam;
	delete vp;
	delete scnMgr;

}
bool basicScene::run(){
	//Here we would get the time between frames

	//Take messages from input
	InputManager::getInstance().getMessages(_messages);
	//Then we deliver the messages
	gameScene::dispatch();

	//Logic simulation done here
	bool aux = updateEnts(0.025);

	//Clear dispatched messages
	clearMessageQueue();

	return aux;

}

void basicScene::dispatch(){


}

#pragma endregion
#pragma region GamePlayScene
//Scene that runs and manage the battle phase of the game.
GamePlayScene::GamePlayScene(std::string id, Game * game, int nP) : gameScene(id, game) {
	scnMgr = pGame->getRoot()->createSceneManager(Ogre::ST_GENERIC);


	//Self-explanatory methods
	cam = scnMgr->createCamera("MainCam");
	cam->setPosition(0, 0, 150);
	cam->lookAt(0, 0, -300);
	cam->setNearClipDistance(5);


	//------------------------------------------------------------------------------------------------------
	//ViewPort Addition
	vp = game->getRenderWindow()->addViewport(cam);
	vp->setBackgroundColour(Ogre::ColourValue(150, 150, 150));

	cam->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) /
		Ogre::Real(vp->getActualHeight()));


	//------------------------------------------------------------------------------------------------------
	//Scene SetUp

	/*	try {
	Ogre::Entity * robot = scnMgr->createEntity("ogrehead.mesh");
	Ogre::SceneNode * robotNode = scnMgr->getRootSceneNode()->createChildSceneNode();
	robotNode->attachObject(robot);
	}
	catch (Ogre::FileNotFoundException e) {
	std::string a = e.getFullDescription();
	std::cout << a;
	}
	*/
	scnMgr->setAmbientLight(Ogre::ColourValue(.5, .5, .5));

	light = scnMgr->createLight("MainLight");
	light->setPosition(20, 80, 50);


	//GAMEPLAY SCENE SET UP
	//Create the players Entities(depends on the parameter passed by argument. This value
	//must correspond with the controllers connected).
	_nPlayers = nP;
	for (int i = 0; i < _nPlayers; i++){
		_players[i] = new Entity(std::to_string(i), this);
		addEntity(_players[i]);
	}

	//Set the starter state to LOADOUT
	_currState = GS_SETUP;



	/*SCENE DEBUG


	test2->addComponent(new stringComponent(test2));
	//test2->addComponent()


	test1->addComponent(new messageSendComponent(test1));
	test1->addComponent(new meshRenderComponent(Ogre::Vector3(0, 0, 100), "Ra.mesh", test1, scnMgr));


	addEntity(test1);
	addEntity(test2);*/

}
GamePlayScene::~GamePlayScene(){
	delete light;
	delete cam;
	delete vp;
	delete scnMgr;

}
bool GamePlayScene::run(){
	//Here we would get the time between frames

	//Take messages from input
	InputManager::getInstance().getMessages(_messages);
	//Then we deliver the messages
	dispatch();

	switch (_currState)
	{
		//In this state, we should set up the players God (mesh renderer, habilities, etc) and playing cards
	case GS_SETUP:
		loadOut();
		break;
		//This state should control the gameplay state (Time, rounds, the end, etc)
	case GS_BATTLE:
		battle();
		break;
		//Last state before leave the scene
	case GS_END:
		end();
		break;
	case GS_LOADING :
		break;
	default:
		break;
	}

	//Logic simulation done here
	bool aux = updateEnts(0.025);

	//Clear dispatched messages
	clearMessageQueue();

	return aux;

}

void GamePlayScene::dispatch(){
	gameScene::dispatch();
}

void GamePlayScene::loadOut(){
	/*
		If players are ready, go to the Battle state (charge the map, UI, etc)
		bool ready = true;
		for(int i = 0; i < _nPlayers; i++){
			ready = ready & pReady[i];
		}
	*/

}

void GamePlayScene::battle(){
	/*
		Pick data from the battle (_batState), and control the end of it
	*/


}
void GamePlayScene::end(){

}
#pragma endregion