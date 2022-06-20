#pragma once
#include "includes.h"
#include "AvgSprite.h"
#include "Reader.h"
struct specialAnim {
	std::vector<int> frames;
	std::string actualAnim;
};


class Character : public AvgSprite {
private:
	Reader* read;

	// helper
	std::vector<std::string> split(std::string s, std::string delimiter) {
		size_t pos_start = 0, pos_end, delim_len = delimiter.length();
		std::string token;
		std::vector<std::string> res;

		while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
			token = s.substr(pos_start, pos_end - pos_start);
			pos_start = pos_end + delim_len;
			res.push_back(token);
		}

		res.push_back(s.substr(pos_start));
		return res;
	}
public:
	std::map<std::string, std::string> setAnims;
	std::map<std::string, std::string> setAnimsReverse;
	std::map<std::string, specialAnim> setAnimsFrames;
	std::map<std::string,readVec2> offsets;

	bool shake;

	readVec2 iconOffset;

	float offsetScale;
	float applyAgainScale;

	std::string name;

	float currentTime;

	Texture* icon;

	Rect getIconSrc(int frame)
	{
		Rect r;
		r.w = icon->width / 2;
		r.h = 1;
		r.x = frame == 1 ? r.w / 2 : 0;
		r.y = 0;

		// normalize

		r.w /= icon->width;
		r.x /= icon->width;

		return r;
	}
	Character(float x, float y, std::string charPath, Texture* toCopy) : AvgSprite(x, y, NULL)
	{
		read = new Reader(charPath);

		tex = toCopy;
		setSparrow("assets/graphical/characters/" + read->getValue("sheet", "") + ".xml");

		icon = Texture::createWithImage("assets/graphical/characters/Icons/" + read->getValue("icon", "") + ".png");

		if (read->getValue("offsetScale", "") != "")
			offsetScale = std::stof(read->getValue("offsetScale", ""));
		else
			offsetScale = 1;

		if (read->getValue("applyOffsetAgain", "") != "")
			applyAgainScale = std::stof(read->getValue("applyOffsetAgain", ""));
		else
			applyAgainScale = 1;

		if (read->getValue("shake", "") == "true")
			shake = true;
		else
			shake = false;

		std::vector<valueType> setAnimsS = read->getAllOfValue("setAnim");

		for (valueType t : setAnimsS)
		{
			setAnims[t.key] = t.value;
			setAnimsReverse[t.value] = t.key;
		}

		std::vector<valueType> setAnimFramesS = read->getAllOfValue("setAnimFrames");

		for (valueType t : setAnimFramesS)
		{
			specialAnim sp;
			std::string after = t.value.substr(t.value.find("|") + 1);
			std::vector<std::string> fr = split(after, ",");
			sp.actualAnim = t.value.substr(0, t.value.find("|"));
			for (std::string fram : fr)
				sp.frames.push_back(std::stoi(fram));
			setAnimsFrames[t.key] = sp;
		}


		std::vector<valueType> offsetsS = read->getAllOfValue("animOffset");

		for (valueType t : offsetsS)
		{
			readVec2 vec = read->convertVec(t.value);
			if (vec.x != 0)
				vec.x *= offsetScale;
			if (vec.y != 0)
				vec.y *= offsetScale;
			// kinda jank but lol
			if (vec.x != 0)
				vec.x *= applyAgainScale;
			if (vec.y != 0)
				vec.y *= applyAgainScale;
			offsets[setAnims[t.key]] = vec;
		}

		if (read->getValue("iconOffset", "") != "")
		{
			iconOffset = read->convertVec(read->getValue("iconOffset", ""));
			std::cout << "Icon offset: " << iconOffset.x << "," << iconOffset.y << std::endl;
		}

		if (read->getValue("loop", "") == "")
			loop = true;
		else
			loop = read->getValue("loop", "") == "false" ? false : true;
		fps = std::stoi(read->getValue("fps", ""));
		play(read->getValue("defaultAnim", ""));
		std::cout << "loaded " + read->getValue("sheet", "") << std::endl;
	}

	Character(float x, float y, std::string charPath) : AvgSprite(x,y,NULL)
	{
		read = new Reader(charPath);

		tex = Texture::createWithImage("assets/graphical/characters/" + read->getValue("sheet", "") + ".png");
		setSparrow("assets/graphical/characters/" + read->getValue("sheet", "") + ".xml");

		icon = Texture::createWithImage("assets/graphical/characters/Icons/" + read->getValue("icon", "") + ".png");

		if (read->getValue("offsetScale", "") != "")
			offsetScale = std::stof(read->getValue("offsetScale", ""));
		else
			offsetScale = 1;

		if (read->getValue("applyOffsetAgain", "") != "")
			applyAgainScale = std::stof(read->getValue("applyOffsetAgain", ""));
		else
			applyAgainScale = 1;

		if (read->getValue("shake", "") == "true")
			shake = true;
		else
			shake = false;

		std::vector<valueType> setAnimsS = read->getAllOfValue("setAnim");

		for (valueType t : setAnimsS)
		{
			setAnims[t.key] = t.value;
			setAnimsReverse[t.value] = t.key;
		}

		std::vector<valueType> setAnimFramesS = read->getAllOfValue("setAnimFrames");

		for (valueType t : setAnimFramesS)
		{
			specialAnim sp;
			std::string after = t.value.substr(t.value.find("|") + 1);
			std::vector<std::string> fr = split(after, ",");
			sp.actualAnim = t.value.substr(0, t.value.find("|"));
			for (std::string fram : fr)
				sp.frames.push_back(std::stoi(fram));
			setAnimsFrames[t.key] = sp;
		}


		std::vector<valueType> offsetsS = read->getAllOfValue("animOffset");

		for (valueType t : offsetsS)
		{
			readVec2 vec = read->convertVec(t.value);
			if (vec.x != 0)
				vec.x *= offsetScale;
			if (vec.y != 0)
				vec.y *= offsetScale;
			// kinda jank but lol
			if (vec.x != 0)
				vec.x *= applyAgainScale;
			if (vec.y != 0)
				vec.y *= applyAgainScale;
			offsets[setAnims[t.key]] = vec;
		}

		if (read->getValue("iconOffset", "") != "")
		{
			iconOffset = read->convertVec(read->getValue("iconOffset", ""));
			std::cout << "Icon offset: " << iconOffset.x << "," << iconOffset.y << std::endl;
		}


		if (read->getValue("loop", "") == "")
			loop = true;
		else
			loop = read->getValue("loop", "") == "false" ? false : true;
		fps = std::stoi(read->getValue("fps", ""));
		play(read->getValue("defaultAnim", ""));
		AvgFrame firstFrame = sparrow->animations[sparrow->currentAnim].frames[0];
		w = firstFrame.frameRect.w;
		h = firstFrame.frameRect.h;
		std::cout << "loaded " + read->getValue("sheet", "") << std::endl;
	}

	void play(std::string anim)
	{
		std::string actualAnim = "";
		if (setAnims[anim] == "")
		{
			// check setFrames
			if (setAnimsFrames.find(anim) == setAnimsFrames.end())
			{
				std::cout << "unable to play that animation! it doesn't exist. Anim in question: " << anim << std::endl;
				return;
			}
			else
			{
				actualAnim = setAnimsFrames[anim].actualAnim;
				sparrow->realFrame = 0;
				sparrow->lastFrame = 1;
				sparrow->framesToPlay = setAnimsFrames[anim].frames;
			}
		}
		else
		{
			actualAnim = setAnims[anim];
			sparrow->framesToPlay = {}; // play em all
		}
		playAnim(actualAnim, fps, loop);
		offsetX = offsets[actualAnim].x * scale;
		offsetY = offsets[actualAnim].y * scale;
		currentTime = 500;
	}

	void play(std::string anim, bool skipFinding)
	{
		std::string actualAnim = "";
		if (!skipFinding)
		{
			if (setAnims[anim] == "")
			{
				// check setFrames
				if (setAnimsFrames.find(anim) == setAnimsFrames.end())
				{
					std::cout << "unable to play that animation! it doesn't exist. Anim in question: " << anim << std::endl;
					return;
				}
				else
				{
					actualAnim = setAnimsFrames[anim].actualAnim;
					sparrow->realFrame = 0;
					sparrow->lastFrame = 1;
					sparrow->framesToPlay = setAnimsFrames[anim].frames;
				}
			}
			else
			{
				actualAnim = setAnims[anim];
				sparrow->framesToPlay = {}; // play em all
			}
		}
		else
			actualAnim = anim;
		playAnim(actualAnim, fps, loop);
		offsetX = offsets[actualAnim].x * scale;
		offsetY = offsets[actualAnim].y * scale;
		currentTime = 500;
	}


	~Character()
	{
		delete read;
		if (icon)
		{
			if (icon->width > 0 && icon->height > 0)
			{
				delete icon;
				icon = NULL;
			}
		}
		offsets.clear();
		setAnims.clear();
		setAnimsReverse.clear();
		setAnimsFrames.clear();
	}
};