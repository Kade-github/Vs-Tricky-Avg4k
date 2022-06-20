#pragma once
#include "EventFunction.h"
#include "Game.h"
#include "Gameplay.h"
class ChangeCharacter : public EventFunction
{
public:
	bool changed;
	~ChangeCharacter()
	{

	}

	virtual void update(float elapsed) {
		if (!changed)
		{
			Gameplay* instance = (Gameplay*)Game::currentMenu;
			if (args == "")
			{
				changed = true;
				std::cout << "changeCharacter_event ERROR : args is empty" << std::endl;
				return;
			}

			bool p2 = false;

			if (args.starts_with("p2|"))
				p2 = true;

			std::string newChar = args.substr(args.find("|") + 1, args.size());

			if (!p2)
			{
				float x = instance->stage->p1->x;
				float y = instance->stage->p1->y;
				instance->stage->layers[6]->removeObj(instance->stage->p1);
				delete instance->stage->p1;
				instance->stage->p1 = new Character(x, y, "assets/graphical/characters/" + newChar);
				instance->stage->layers[6]->add(instance->stage->p1);
			}
			else
			{
				float x = instance->stage->p2->x;
				float y = instance->stage->p2->y;
				instance->stage->layers[6]->removeObj(instance->stage->p2);
				delete instance->stage->p2;
				instance->stage->p2 = new Character(x, y, "assets/graphical/characters/" + newChar);
				instance->stage->layers[6]->add(instance->stage->p2);
			}

			changed = true;
		}
	}
};