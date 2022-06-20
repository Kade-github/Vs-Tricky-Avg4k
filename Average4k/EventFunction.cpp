#include "DoStatic.h"
#include "SetCharacterPosition.h"
#include "ChangeCharacter.h"
#include "SetViewOffset.h"
#include "PlayAnimation.h"
#include "EventFunction.h"
std::map<std::string, EventFunction*> EventFunction::events;

void EventFunction::initEvents() {
	if (((Gameplay*)Game::currentMenu)->stage->eventFile)
	{
		for (auto ev : ((Gameplay*)Game::currentMenu)->stage->eventFile->events)
		{
			EventFunction* func = NULL;
			if (ev.func == "doStatic")
				func = new DoStatic();
			if (ev.func == "setCharacterPosition")
				func = new SetCharacterPosition();
			if (ev.func == "changeCharacter")
				func = new ChangeCharacter();
			if (ev.func == "setViewOffset")
				func = new SetViewOffset();
			if (ev.func == "playAnimation")
				func = new PlayAnimation();
			createEvent(ev.name, func);
		}
	}

}