#pragma once
#include "EventFunction.h"
#include "Game.h"
#include "Gameplay.h"
class SetViewOffset : public EventFunction
{
public:
	bool set;
	~SetViewOffset()
	{

	}

	virtual void update(float elapsed) {
		if (!set)
		{
			Gameplay* instance = (Gameplay*)Game::currentMenu;
			if (args == "")
			{
				set = true;
				std::cout << "setViewOffset_event ERROR : args is empty" << std::endl;
				return;
			}

			float x = std::stod(args.substr(0, args.find(",")));
			float y = std::stod(args.substr(args.find(",") + 1, args.size()));

			instance->viewOffsetX = x;
			instance->viewOffsetY = y;

			set = true;
		}
	}
};