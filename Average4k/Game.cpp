#include "includes.h"
#include <algorithm>
#include "Game.h"
#include "MainMenu.h"
#include "Text.h"
#include "AvgRect.h"
#include "msgpack.hpp"
#include "TweenManager.h"
#include "Helpers.h"
#include "Offseter.h"
using namespace std;



float Game::multiplierx = 1;
float Game::multipliery = 1;

bool printPackets_DB = false;
bool Game::gameplayEvents_DB = false;

AvgRect* __transRect;

vector<Object*>* objects;

AvgCamera* Game::mainCamera = NULL;
Viewpoint* Game::mainView = NULL;
Menu* Game::currentMenu = NULL;
Menu* Game::toGoTo = NULL;
SaveFile* Game::save = NULL;
Game* Game::instance = NULL;
chartMeta Game::loadedChart;

std::string Game::version;

bool debug_takingInput;

std::string debug_string;

AvgRect* consoleBG;
AvgRect* consoleCMDBar;

bool debugConsole;
Text* debugText;
Text* consoleLog;
Text* cmdPrompt;
Text* fpsText;
Text* watermark;

map<int, bool> Game::controls = {
	{SDLK_d, false},
	{SDLK_f, false},
	{SDLK_j, false},
	{SDLK_k, false},
	{SDLK_UP, false},
	{SDLK_DOWN, false},
	{SDLK_RETURN, false}
};

SDL_Renderer* Game::renderer;
SDL_Window* Game::window;

double Game::gameFPS = 0.0;

float Game::deltaTime = 0;

float Game::startTick = 0;

int Game::gameWidth = 0;
int Game::gameHeight = 0;

bool Game::startConnect = false;

bool transCompleted = false;

HANDLE multiThreadHandle;

void Game::setViewpointX(float x)
{
	mainView->x = (x - (gameWidth / 2)) - mainView->x;
}

void Game::setViewpointY(float y)
{
	mainView->y = (y - (gameWidth / 2)) - mainView->y;
}

void Game::GetMousePos(int* mx, int* my)
{
	SDL_GetMouseState(mx, my);
	(*mx) *= multiplierx;
	(*my) *= multipliery;
}

std::vector<Text*> lines;

void Game::db_addLine(std::string s) {
	for (Text* t : lines)
		t->y -= 16;
	Text* newLine = new Text(0, 204, s, 16, "vcr");
	newLine->staticView = true;
	newLine->border = true;
	newLine->borderSize = 2;
	newLine->borderColor = { 0,0,0 };
	lines.push_back(newLine);
}

void transCall() {
	Game::currentMenu->removeAll();
	Game::currentMenu->removePhase();
	Game::mainCamera->children.clear();
	delete Game::currentMenu;
	Game::instance->mainView->x = 0;
	Game::instance->mainView->y = 0;
	Game::instance->mainView->zoom = 1;
	Game::currentMenu = Game::toGoTo;
	Game::currentMenu->create();
	__transRect->alpha = 0;
	transCompleted = true;
}

void Game::transitionToMenu(Menu* m)
{
	transCompleted = false;
	toGoTo = m;
	Tweening::tweenCallback callback = (Tweening::tweenCallback)transCall;
	transitioning = true;
	Tweening::TweenManager::activeTweens.clear();
	Tweening::TweenManager::createNewTween("_trans", __transRect, Tweening::tt_Alpha, 235, 0, 1, callback, Easing::EaseInSine);
}

void Game::switchMenu(Menu* m)
{
	for (Object* obj : Game::mainCamera->children)
	{
		delete obj;
	}
	Game::mainCamera->children.clear();
	currentMenu->removeAll();
	delete currentMenu;
	currentMenu = m;
}

void Game::createGame()
{
	objects = new std::vector<Object*>();

	__transRect = new AvgRect(0,0,Game::gameWidth, Game::gameHeight);
	__transRect->staticView = true;
	__transRect->create();
	__transRect->alpha = 0;

	mainCamera = new AvgCamera(0, 0, 1280, 720);

	mainView = new Viewpoint();
	mainView->x = 0;
	mainView->y = 0;
	mainView->zoom = 1;

	// to start
	currentMenu = new MainMenu();
	currentMenu->create();

	consoleBG = new AvgRect(0, 0, Game::gameWidth, 220);
	consoleBG->alpha = 0.4;
	consoleBG->staticView = true;

	consoleCMDBar = new AvgRect(0, 220, Game::gameWidth, 25);
	consoleCMDBar->alpha = 0.4;
	consoleCMDBar->staticView = true;
	save = new SaveFile();

	fpsText = new Text(4, 4, "FPS: 0", 14, "vcr");
	fpsText->staticView = true;
	fpsText->border = true;
	fpsText->borderSize = 2;
	fpsText->borderColor = { 0,0,0 };


}


void Game::mouseButtonDown()
{
	if (objects != nullptr)
	{
		for (int i = 0; i < objects->size(); i++)
		{
			Object* bruh = objects->at(i);
			bruh->mouseDown();
		}
	}
}



void Game::update(Events::updateEvent update)
{

	//SDL_SetRenderTarget(renderer, NULL);

	//SDL_RenderClear(update.renderer);

	//SDL_SetRenderTarget(renderer, mainCamera->cameraTexture);

	//SDL_RenderClear(update.renderer);

	if (transitioning && transCompleted)
	{
		transCompleted = false;
		Tweening::TweenManager::createNewTween("_trans", __transRect, Tweening::tt_Alpha, 235, 255, 0, []()->void {
			Game::instance->transitioning = false;
			std::cout << "no more" << std::endl;
			}, Easing::EaseInSine);
	}

	mainCamera->update(update);

	if (currentMenu != nullptr && currentMenu->created)
		currentMenu->update(update);

	if (!transitioning)
		fpsText->setText("FPS: " + std::to_string((int)gameFPS));

	for (int i = 0; i < objects->size(); i++)
	{
		try
		{
			Object* bruh = (*objects)[i];
			bruh->update(update);
		}
		catch (...)
		{

		}
	}



	SDL_Rect DestR;

	DestR.x = mainCamera->x;
	DestR.y = mainCamera->y;
	DestR.w = mainCamera->w;
	DestR.h = mainCamera->h;


	if (SDL_GetTicks() > 1000)
	{
		for (Tweening::Tween& tw : Tweening::TweenManager::activeTweens)
		{
			Tweening::TweenManager::updateTween(tw, Game::deltaTime);
		}

		for (Tweening::Tween& tw : Tweening::TweenManager::tweenRemove)
		{
			Tweening::TweenManager::activeTweens.erase(std::remove(Tweening::TweenManager::activeTweens.begin(), Tweening::TweenManager::activeTweens.end(), tw), Tweening::TweenManager::activeTweens.end());
		}
		Tweening::TweenManager::tweenRemove.clear();
	}
	if (currentMenu != NULL)
	{
		if (currentMenu->created)
			for (int i = 0; i < currentMenu->children.size(); i++)
			{
				Object* obj = currentMenu->children[i];
				obj->draw();

			}
	}

	if (currentMenu != nullptr && currentMenu->created)
		currentMenu->postUpdate(update);

	if (fpsText && !debugConsole)
	{
		fpsText->draw();
	}


	if (debugConsole)
	{
		SDL_FRect topBar;
		topBar.x = 0;
		topBar.y = 0;
		topBar.w = Game::gameWidth;
		topBar.h = 25;
		if (!debugText)
		{
			debugText = new Text(0, 0, "Debug Console", 16, "vcr");
			debugText->staticView = true;
			debugText->border = true;
			debugText->borderSize = 2;
			debugText->borderColor = { 0,0,0 };
			consoleLog = new Text(0, 195, "", 16, "vcr");
			consoleLog->staticView = true;
			consoleLog->border = true;
			consoleLog->borderSize = 2;
			consoleLog->borderColor = { 0,0,0 };
			cmdPrompt = new Text(0, 220, ">", 16, "vcr");
			cmdPrompt->staticView = true;
			cmdPrompt->border = true;
			cmdPrompt->borderSize = 2;
			cmdPrompt->borderColor = { 0,0,0 };
		}

		debugText->setText("Debug Console | FPS: " + std::to_string((int)gameFPS) + " (CTRL to start typing, F11 again to close it)");

		if (debug_takingInput)
			cmdPrompt->setText(">" + debug_string + "_");
		else
			cmdPrompt->setText(">");



		consoleBG->draw();
		consoleCMDBar->draw();

		Rect clip;

		clip.x = 0;
		clip.y = 25;
		clip.w = Game::gameWidth;
		clip.h = 220;

		Rendering::SetClipRect(&clip);

		for (Text* t : lines)
			t->draw();

		Rendering::SetClipRect(NULL);

		debugText->draw();
		cmdPrompt->draw();
	}

	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	//SDL_SetRenderTarget(renderer, NULL);

	//SDL_RenderCopyEx(renderer, mainCamera->cameraTexture, NULL, &DestR, mainCamera->angle, NULL, SDL_FLIP_NONE);


	if (transitioning)
		__transRect->draw();


	//SDL_RenderPresent(renderer);
}

void Game::keyDown(SDL_KeyboardEvent ev)
{
	if (transitioning)
		return;

	if (ev.keysym.sym == SDLK_ESCAPE && debug_takingInput)
	{
		debug_takingInput = false;
		return;
	}
	if (ev.keysym.sym == SDLK_RETURN && debug_takingInput)
	{
		debug_takingInput = false;
		// cmds

		std::transform(debug_string.begin(), debug_string.end(), debug_string.begin(), Helpers::asciitolower);
		db_addLine(">" + debug_string);

		if (debug_string == "help")
		{
			db_addLine("dumpVar - dumps a lot of variables");
			db_addLine("gameplayEvents - prints out gameplay events when they happen");
			db_addLine("offset - open the offseting tool");
		}
		
		else if (debug_string == "xg")
		{
			db_addLine("xg is a dumbass lol! (jkjkjkjk 187384089228214273)");
		}
		else if (debug_string == "dumpvar")
		{
			db_addLine("Menu Variables:");
			db_addLine("Transitioning: " + std::to_string(transitioning));
			db_addLine("Children: " + std::to_string(currentMenu->cam->children.size()));
		}
		else if (debug_string == "gameplayevents")
		{
			gameplayEvents_DB = !gameplayEvents_DB;
			db_addLine("set to " + std::to_string(gameplayEvents_DB));
		}
		else if (debug_string == "offset")
		{
			transitionToMenu(new Offseter());
		}
		else
		{
			db_addLine("No command found");
		}

		debug_string = "";

		return;
	}

	if (ev.keysym.sym == SDLK_BACKSPACE && debug_takingInput)
	{
		if (debug_string.size() > 0)
			debug_string.pop_back();
	}
	if (debug_takingInput)
		return;

	if (ev.keysym.sym == SDLK_LCTRL && debugConsole)
	{
		debug_string = "";
		debug_takingInput = true;
		return;
	}

	if (controls.count(ev.keysym.sym) == 1)
		controls[ev.keysym.sym] = true;

	if (currentMenu != NULL)
		currentMenu->keyDown(ev);

	if (ev.keysym.sym == SDLK_F4)
	{
		fullscreen = !fullscreen;

		auto w = 0;
		auto h = 0;

		if (fullscreen)
		{
			SDL_SetWindowFullscreen(Game::window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			SDL_DisplayMode DM;
			SDL_GetCurrentDisplayMode(0, &DM);
		
			w = DM.w;
			h = DM.h;
		}
		else
		{
			SDL_SetWindowFullscreen(Game::window, 0);
			w = 1280;
			h = 720;

		}

		std::cout << "bruh " << w << " " << h << std::endl;
		glViewport(0, 0, w, h);

		multiplierx = (float)1280 / (float)w;
		multipliery = (float)720 / (float)h;

		std::cout << "FULLSCREEN MULTIPLIERS: " << multiplierx << " " << multipliery << std::endl;

		for (Object* obj : currentMenu->children)
			obj->resChange(w, h);
	}

	if (ev.keysym.sym == SDLK_F11)
	{
		debugConsole = !debugConsole;
	}

	if (objects != nullptr)
	{
		for (int i = 0; i < objects->size(); i++)
		{
			Object* bruh = (*objects)[i];
			bruh->keyDown(ev);
		}
	}
}

void Game::keyUp(SDL_KeyboardEvent ev)
{
	if (transitioning)
		return;
	if (controls.count(ev.keysym.sym) == 1)
		controls[ev.keysym.sym] = false;

	currentMenu->keyUp(ev);

	for (int i = 0; i < objects->size(); i++)
	{
		Object* bruh = (*objects)[i];
		bruh->keyDown(ev);
	}
}

std::vector<Object*>* Game::getGlobalObjects()
{
	return objects;
}

void Game::addGlobalObject(Object* obj)
{
	objects->push_back(obj);
}

void Game::removeGlobalObject(Object* obj)
{
	objects->erase(std::remove(objects->begin(), objects->end(), obj), objects->end());
}

bool Game::getKey(int code)
{
	if (controls.count(code) != 1)
		return false;
	return controls[code];
}

void Game::textInput(SDL_TextInputEvent event)
{
	if (debug_takingInput)
	{
		debug_string += event.text;
		return;
	}
	for (int i = 0; i < objects->size(); i++)
	{
		Object* btuh = (*objects)[i];
		btuh->textInput(event);
	}
}