#pragma once
#include "includes.h"
#include "Game.h"
#include "Gameplay.h"
enum eventRun {
	er_Update = 0,
	er_Sing = 1,
	er_Miss = 2,
};
class EventFunction {
public:

	static std::map<std::string,EventFunction*> events;

	static void createEvent(std::string name, EventFunction* func) {
		events[name] = func;
		events[name]->create();
		std::cout << "EVENT MANAGER -> CREATED " << name << std::endl;
	}

	static void sendOutStageVars()
	{
		Gameplay* instance = (Gameplay*)Game::currentMenu;
		std::map<std::string, EventFunction*>::iterator it;
		for (it = events.begin(); it != events.end(); it++)
		{
			it->second->char1 = instance->stage->p1->name;
			it->second->char2 = instance->stage->p2->name;
		}
			
	}

	static void sendArg(std::string eventName, std::string arg)
	{
		events[eventName]->args = arg;
	}

	std::string char1, char2;
	std::string args;
	std::string name;

	static void destroyEventSystem()
	{
		for (auto const& x : events)
		{
			delete x.second;
		}
		events.clear();
	}

	static void initEvents();

	static void runMiss(std::string name)
	{
		events[name]->onMiss();
	}

	static void runSing(std::string name, bool p2)
	{
		events[name]->onSing(p2);
	}

	static void runUpdate(std::string name)
	{
		events[name]->update(Game::deltaTime);
	}

	virtual void create() {}
	virtual void onMiss() {};
	virtual void update(float elapsed) {}
	virtual void onSing(bool p2) {}
};