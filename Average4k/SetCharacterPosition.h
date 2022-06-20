#pragma once
#include "EventFunction.h"
#include "Game.h"
#include "Gameplay.h"
class SetCharacterPosition : public EventFunction
{
public:
	bool set;
	~SetCharacterPosition()
	{

	}

	virtual void update(float elapsed) {
		if (!set)
		{
			Gameplay* instance = (Gameplay*)Game::currentMenu;
			if (args == "")
			{
				set = true;
				std::cout << "setCharacterPosition_event ERROR : args is empty" << std::endl;
				return;
			}

			bool p2 = false;

			if (args.starts_with("p2|"))
				p2 = true;

			std::string value = args.substr(args.find("|") + 1, args.size());

			float x = std::stod(value.substr(0, value.find(",")));
			float y = std::stod(value.substr(value.find(",") + 1, value.size()));

			if (!p2)
			{
				instance->stage->p1->x = x;
				instance->stage->p1->y = y;
			}
			else
			{
				instance->stage->p2->x = x;
				instance->stage->p2->y = y;
			}

			set = true;
		}
	}
};