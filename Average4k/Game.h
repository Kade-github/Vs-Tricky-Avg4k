#pragma once
#include "includes.h"
#include <map>
#include "SaveFile.h"
#include "Camera.h"
#include "Menu.h"
#include "Chart.h"
#include "AvgCamera.h"

class Viewpoint {
public:
	float x, y, zoom;
};

class Game
{
public:
	void createGame();

	static float multiplierx;
	static float multipliery;

	bool flowtime = false;

	Menu* menuToDelete;

	bool transitioning;


	static bool startConnect;
	static Menu* toGoTo;

	void setViewpointX(float x);
	void setViewpointY(float y);

	static void GetMousePos(int* mx, int* my);

	void mouseButtonDown();
	void update(Events::updateEvent updateEvent);
	void keyDown(SDL_KeyboardEvent event);
	void keyUp(SDL_KeyboardEvent event);

	void textInput(SDL_TextInputEvent event);
	static AvgCamera* mainCamera;
	static Viewpoint* mainView;
	static Menu* currentMenu;
	static SaveFile* save;
	static chartMeta loadedChart;
	static std::string version;

	static Game* instance;

	bool fullscreen = false;

	static int gameWidth;
	static int gameHeight;
	static bool gameplayEvents_DB;
	static std::map<int, bool> controls;
	void db_addLine(std::string s);

	void transitionToMenu(Menu* m);
	void switchMenu(Menu* m);

	static std::vector<Object*>* getGlobalObjects();
	static void addGlobalObject(Object* obj);
	static void removeGlobalObject(Object* obj);

	static bool getKey(int code);

	static SDL_Renderer* renderer;
	static SDL_Window* window;
	static float startTick;
	static double gameFPS;
	static float deltaTime;
};

