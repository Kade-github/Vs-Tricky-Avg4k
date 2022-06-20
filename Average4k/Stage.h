#pragma once
#include "includes.h"
#include "Reader.h"
#include "Character.h"
#include "AvgGroup.h"
#include "EventFile.h"
#include <sys/stat.h>
class Stage {
private:
	Reader* read;
public:
	Character* p1;
	Character* p2;
	Character* gf;

	AvgGroup* layers[12];

	EventFile* eventFile;

	readVec2 bfOff;
	readVec2 dadOff;
	readVec2 viewPointPos;

	bool convertMinesToDeath = false;

	float defaultZoom = 1;

	Stage(std::string stagePath)
	{
		read = new Reader(stagePath);
		std::vector<valueType> sprites = read->getAllOfValue("sprites");
		
		viewPointPos = read->convertVec(read->getValue("viewPointPos", ""));

		Game::mainView->x = viewPointPos.x;
		Game::mainView->y = viewPointPos.y;

		if (read->getValue("p1ViewOffset", "") != "")
			bfOff = read->convertVec(read->getValue("p1ViewOffset", ""));
		if (read->getValue("p2ViewOffset", "") != "")
			dadOff = read->convertVec(read->getValue("p2ViewOffset", ""));


		Game::mainView->zoom = std::stof(read->getValue("viewPointZoom",""));
		defaultZoom = Game::mainView->zoom;
		for (valueType t : sprites)
		{
			int layer = std::stoi(read->getValue("spritesLayer", t.key));
			readVec2 vec = read->convertVec(read->getValue("spritesPos", t.key));
			std::string path = t.value;
			std::string scaleT = read->getValue("spritesScale", t.key);
			float scale = 1;
			if (scaleT != "")
				scale = std::stof(scaleT);
			if (layer > 12 || layer < 0)
			{
				std::cout << "layer for " << t.key << " is incorect. " << layer << " received." << std::endl;
				continue;
			}

			if (layer == 6)
			{
				std::cout << "you cannot use layer 6 for " << t.key << " as it's not a character!" << std::endl;
				continue;
			}
			struct stat info; // it requires this so uh
			if (stat(t.value.c_str(), &info) != 0)
			{
				std::cout << "I cannot find " << t.value << " for " << t.key << std::endl;
				continue;
			}

			if (layers[layer] == NULL)
				layers[layer] = new AvgGroup(0, 0, Game::gameWidth, Game::gameHeight);

			AvgSprite* sprite = new AvgSprite(vec.x, vec.y, t.value);
			if (read->getValue("spritesScroll", t.key) != "")
				sprite->scroll = std::stof(read->getValue("spritesScroll", t.key));
			if (read->getValue("spritesAlpha", t.key) != "")
				sprite->alpha = std::stof(read->getValue("spritesAlpha", t.key));
			sprite->scale = scale;
			layers[layer]->add(sprite);

			// ANIMATION???

			if (read->getValue("spritesSparrow", t.key) != "")
				sprite->setSparrow(read->getValue("spritesSparrow", t.key));

			if (sprite->sparrow)
			{
				for (valueType tt : read->getAllOfValue("spritesSetAnim"))
				{
					if (tt.key != t.key)
						continue;

					std::string name = tt.value.substr(0, tt.value.find("|"));
					std::string replace = tt.value.substr(tt.value.find("|") + 1, tt.value.size());

					// rename anims
					auto node = sprite->sparrow->animations.extract(replace);
					node.key() = name;
					sprite->sparrow->animations.insert(std::move(node));
				}

				sprite->fps = std::stoi(read->getValue("spritesSetFPS", t.key));

				// offsets

				for (valueType tt : read->getAllOfValue("spritesSetAnimOffset"))
				{
					if (tt.key != t.key)
						continue;

					std::string name = tt.value.substr(0, tt.value.find("|"));
					readVec2 offset = read->convertVec(tt.value.substr(tt.value.find("|") + 1, tt.value.size()));
					sprite->animOff[name].x = offset.x;
					sprite->animOff[name].y = offset.y;
				}


				sprite->playAnim(read->getValue("spritesDefaultAnim", t.key), sprite->fps, true);
			}
		}

		readVec2 p1Pos = read->convertVec(read->getValue("p1Pos", ""));
		readVec2 p2Pos = read->convertVec(read->getValue("p2Pos", ""));
		readVec2 gfPos = read->convertVec(read->getValue("gfPos", ""));

		layers[6] = new AvgGroup(0, 0, Game::gameWidth, Game::gameHeight);

		p1 = new Character(p1Pos.x, p1Pos.y, "assets/graphical/characters/" + read->getValue("p1", ""));
		std::string p1ScaleT = read->getValue("p1Scale", "");
		if (p1ScaleT != "")
			p1->scale = std::stof(p1ScaleT);
		p2 = new Character(p2Pos.x, p2Pos.y, "assets/graphical/characters/" + read->getValue("p2", ""));
		std::string p2ScaleT = read->getValue("p2Scale", "");
		if (p2ScaleT != "")
			p2->scale = std::stof(p2ScaleT);
		gf = new Character(gfPos.x,gfPos.y,"assets/graphical/characters/" + read->getValue("gf", ""));
		std::string gfScaleT = read->getValue("gfScale", "");
		if (gfScaleT != "")
			gf->scale = std::stof(gfScaleT);

		p1->name = read->getValue("p1", "");
		p2->name = read->getValue("p2", "");
		gf->name = read->getValue("gf", "");

		layers[6]->add(gf);
		layers[6]->add(p2);
		layers[6]->add(p1);

		if (read->getValue("camTintR", "") != "")
		{
			Game::mainCamera->tint.r = std::stoi(read->getValue("camTintR", ""));
			Game::mainCamera->tint.g = std::stoi(read->getValue("camTintG", ""));
			Game::mainCamera->tint.b = std::stoi(read->getValue("camTintB", ""));
		}

		if (read->getValue("eventFile", "") != "")
		{
			eventFile = new EventFile("assets/stages/Events/" + read->getValue("eventFile", ""));
		}

		if (read->getValue("minesToDeath", "") != "")
			convertMinesToDeath = read->getValue("minesToDeath", "") == "true" ? true : false;

	}

	std::vector<AvgGroup*> returnUsedLayers()
	{
		std::vector<AvgGroup*> grps;
		for (int i = 0; i < 12; i++)
		{
			if (layers[i] != NULL)
				grps.push_back(layers[i]);
		}
		return grps;
	}

	~Stage()
	{
		delete p1;
		delete p2;
		delete gf;
	}
};