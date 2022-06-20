#pragma once
#include "EventFunction.h"
#include "AvgSprite.h"
#include "Text.h"
#include "Gameplay.h"
#include "SoundManager.h"
class DoStatic : public EventFunction
{
public:
	AvgSprite* staticSprite;
	Text* staticText;

	float time = 0;

	Channel* staticSound;

	std::string TrickyLinesSing[13] = { "SUFFER", "INCORRECT", "INCOMPLETE", "INSUFFICIENT", "INVALID", "CORRECTION", "MISTAKE", "REDUCE", "ERROR", "ADJUSTING", "IMPROBABLE", "IMPLAUSIBLE", "MISJUDGED" };
	std::string ExTrickyLinesSing[13] = { "YOU AREN'T HANK", "WHERE IS HANK", "HANK???", "WHO ARE YOU", "WHERE AM I", "THIS ISN'T RIGHT", "MIDGET", "SYSTEM UNRESPONSIVE", "WHY CAN'T I KILL?????" };
	std::string TrickyLinesMiss[13] = { "TERRIBLE", "WASTE", "MISS CALCULTED", "PREDICTED", "FAILURE", "DISGUSTING", "ABHORRENT", "FORESEEN", "CONTEMPTIBLE", "PROGNOSTICATE", "DISPICABLE", "REPREHENSIBLE" };

	~DoStatic()
	{
		delete staticSprite;
		delete staticText;
	}

	virtual void create() {
		staticSprite = new AvgSprite(0, 0, "assets/graphical/tricky/TrickyStatic.png");
		staticSprite->staticView = true;
		staticSprite->alpha = 0;
		Game::currentMenu->add(staticSprite);
		staticText = new Text(0,0,"",32,"impact");
		staticText->alpha = 0;
		staticText->color = { 255,0,0 };
		staticText->scale = 2.1;
		Game::currentMenu->add(staticText);

		staticSound = SoundManager::createChannel("assets/sounds/staticSound.ogg", "event_doStatic_sound");
	}

	virtual void update(float elapsed) {
		if (time > 0)
			time -= elapsed;

		if (time <= 0)
		{
			staticSprite->alpha = 0;
			staticText->alpha = 0;
		}
		else
		{
			staticText->angle = -5 + rand() % ((5 + 1) - (-5));
			staticSprite->alpha = 0.1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.5 - 0.1)));
		}
	}

	void createSpookyText(bool miss)
	{
		Gameplay* instance = (Gameplay*)Game::currentMenu;
		int ran = rand() % 13;
		std::string line = "";
		if (char2.starts_with("trickyEx"))
			line = ExTrickyLinesSing[ran];
		else
			line = TrickyLinesSing[ran];

		if (miss)
			line = TrickyLinesMiss[ran];

		staticSound->play();

		staticText->setText(line);
		staticText->x = instance->stage->p2->x + (instance->stage->p2->w / 2) + (40 + rand() % ((120 + 1) - 40));
		staticText->y = instance->stage->p2->y + (instance->stage->p2->h / 2) + (200 + rand() % ((300 + 1) - 200));
	}

	virtual void onMiss()
	{
		if ((rand() % 20 <= 8) && time <= 0)
		{
			time = 250;
			staticSprite->alpha = 1;
			staticText->alpha = 1;

			createSpookyText(true);
		}
	}

	virtual void onSing(bool p2) {
		if (p2 && (rand() % 20 <= 4) && time <= 0)
		{
			time = 250;
			staticSprite->alpha = 1;
			staticText->alpha = 1;

			createSpookyText(false);
		}
	}
};