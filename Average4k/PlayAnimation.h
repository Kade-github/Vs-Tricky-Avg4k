#pragma once
#include "EventFunction.h"
#include "Game.h"
#include "Gameplay.h"
class PlayAnimation : public EventFunction
{
public:
	bool played;
	~PlayAnimation()
	{

	}

	virtual void update(float elapsed) {
		if (!played)
		{
			Gameplay* instance = (Gameplay*)Game::currentMenu;
			if (args == "")
			{
				played = true;
				std::cout << "playAnimation_event ERROR : args is empty" << std::endl;
				return;
			}

			bool p2 = false;

			if (args.starts_with("p2|"))
				p2 = true;

			std::string value = args.substr(args.find("|") + 1, args.size());

			if (!p2)
				instance->stage->p1->play(value);
			else
				instance->stage->p2->play(value);

			played = true;
		}
	}
};