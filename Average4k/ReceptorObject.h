#pragma once
#include "includes.h"
#include "Game.h"
#include "AvgSprite.h"
class ReceptorObject : public AvgSprite {
public:
	int x;
	int y;

	int type;
	
	float lightUpTimer;

	ReceptorObject(int _x, int _y, int _type) : AvgSprite(_x,_y,NULL) {
		x = _x;
		y = _y;
		// this does the same thing but im lazy and too lazy to check
		w = (64 * Game::save->GetDouble("Note Size"));
		h = (64 * Game::save->GetDouble("Note Size"));
		type = _type;
		scale = 1;
	}

	void light()
	{
		lightUpTimer = 1;
	}

};