#pragma once
#include "includes.h"
#include "Object.h"
#include "GL.h"
#include "AvgFrame.h"
#include "Game.h"
#include "Helpers.h"
class AvgSprite : public Object
{
private:
	float animStartTime = 0;
	float animTime = 0;
public:
	Texture* tex;

	int borderSize = 4;

	float accel;
	float veloc;

	//Color borderColor;
	bool customDraw = false;
	bool border = false;
	bool round = false;

	double offsetX = 0;
	double offsetY = 0;

	std::map<std::string, vec2> animOff;

	AvgSparrow* sparrow;
	int frame;
	int fps;

	bool loop;
	bool animationFinished;

	float overrideW, overrideH;
	Rect customSrc;
	Rect clipRect;

	virtual void setAlpha(float _alpha)
	{
		alpha = _alpha;
	}

	bool dontDeleteTex = false;

	void setSparrow(std::string xml)
	{
		sparrow = new AvgSparrow(xml, tex->width, tex->height);
		AvgFrame firstFrame = sparrow->animations.begin()->second.frames[0];
		w = firstFrame.frameRect.w;
		h = firstFrame.frameRect.h;
	}

	void playAnim(std::string name, int _fps, bool _loop)
	{
		animTime = 0;
		animationFinished = false;
		sparrow->playAnim(name);
		fps = _fps;
		loop = _loop;
	}

	AvgSprite(int _x, int _y, std::string path) : Object(x, y) {
		x = _x;
		y = _y;
		tex = Texture::createWithImage(path);
		if (!tex)
			std::cout << "failed to get texture! " + path << std::endl;
		w = tex->width;
		h = tex->height;
		alpha = 1;
	};

	AvgSprite(int _x, int _y, Texture* data) : Object(x, y) {
		x = _x;
		y = _y;
		tex = data;
		if (!tex)
		{
			return;
		}
		w = tex->width;
		h = tex->height;
		alpha = 1;
	};

	~AvgSprite()
	{
		for (Object* obj : children)
		{
			delete obj;
		}
		beforeDeath();
		if (sparrow && !dontDeleteTex)
		{
			delete sparrow;
			sparrow = NULL;
		}
		die();
	}

	void changeOutTexture(std::string path)
	{
		delete tex;
		tex = Texture::createWithImage(path);
		w = tex->width;
		h = tex->height;
	}

	bool centerOrigin = false;

	float velX, velY;
	float acclX, acclY;

	void updateVel()
	{
		float velocityDelta = 0.5  * (Helpers::computeVelocity(velX, acclX, 0, 10000, Game::deltaTime / 1000) - velX);
		velX += velocityDelta;
		float delta = velX * Game::deltaTime / 1000;
		velX += velocityDelta;
		x += delta;

		velocityDelta = 0.5 * (Helpers::computeVelocity(velY, acclY, 0, 10000, Game::deltaTime / 1000) - velY);
		velY += velocityDelta;
		delta = velY * Game::deltaTime / 1000;
		velY += velocityDelta;
		y += delta;
	}

	virtual void draw() {
		if (customDraw)
			return;

		updateVel();
		Rect dstRect;
		Rect srcRect;
		float uX = x, uY = y;

		float mpx = 0;
		float mpy = 0;

		if (centerOrigin && !sparrow)
		{
			mpx = (w * (1 - scale)) / 2;
			mpy = (h * (1 - scale)) / 2;
		}

		dstRect.x = uX + mpx;
		dstRect.y = uY + mpy;

		dstRect.w = w * scale;
		dstRect.h = h * scale;
		
		dstRect.r = 255;
		dstRect.g = 255;
		dstRect.b = 255;
		dstRect.a = alpha;

		if (customSrc.w != 0)
			srcRect = customSrc;
		else
		{
			srcRect.x = 0;
			srcRect.y = 0;
			srcRect.w = 1;
			srcRect.h = 1;
		}

		if (sparrow)
		{
			if (!animationFinished || loop)
			{
				animTime += Game::deltaTime;
				frame = (animTime * fps / 1000);
				int size = sparrow->animations[sparrow->currentAnim].frames.size();
				if (frame > size - 1)
				{
					if (!loop)
					{
						animationFinished = true;
						frame = size - 1;
					}
					else
					{
						animTime = 0;
					}
				}
				if (frame > size - 1)
					frame = 0;
			}
			AvgFrame fr = sparrow->getRectFromFrame(frame);
 			srcRect = fr.srcRect;
			dstRect.x = uX + ((fr.frameRect.x * (scale * Game::mainView->zoom)) + mpx);
			dstRect.y = uY + ((fr.frameRect.y * (scale * Game::mainView->zoom)) + mpy);
			


			dstRect.w = (int)(fr.frameRect.w * scale);
			dstRect.h = (int)(fr.frameRect.h * scale);

			if (centerOrigin)
			{
				offsetX = (fr.frameRect.w / 2) * scale;
				offsetY = (fr.frameRect.h / 2) * scale;
			}

			if (animOff.find(sparrow->currentAnim) != animOff.end())
			{
				offsetX = animOff[sparrow->currentAnim].x;
				offsetY = animOff[sparrow->currentAnim].y;
			}

			if (overrideW != 0)
				dstRect.w = overrideW * scale;
			if (overrideH != 0)
				dstRect.h = overrideH * scale;
			//Game::mainCamera->clipRect = dstRect;
		}

		dstRect.x -= offsetX;
		dstRect.y -= offsetY;

		if (!staticView)
		{
			dstRect.x -= Game::mainView->x * scroll;
			dstRect.y -= Game::mainView->y * scroll;

			dstRect.w *= Game::mainView->zoom;
			dstRect.h *= Game::mainView->zoom;
		}


		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(&clipRect);

		Rendering::setBlend();

		Rendering::PushQuad(&dstRect, &srcRect, tex, GL::genShader,angle);

		if (clipRect.w > 0 || clipRect.h > 0)
			Rendering::SetClipRect(NULL);
	}

	virtual void beforeDeath() {
		if (!dontDeleteTex)
			delete tex;
	}
};


