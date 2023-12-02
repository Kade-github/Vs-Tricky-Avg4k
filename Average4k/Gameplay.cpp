#include "Gameplay.h"
#include "SongSelect.h"
#include <chrono>
#include "TweenManager.h"
#include "EventFunction.h"


Texture* Gameplay::noteskin;

Texture* Gameplay::specialNoteskin;

Texture* graphix;

std::map<std::string, AvgSprite*> avatars;

float Gameplay::rate = 1;

bool hitNote = false;

void CALLBACK bruh(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	BASS_ChannelStop(channel);
}

std::string laneSuff(int lane)
{
	std::string suffix = "";
	switch (lane % 4)
	{
	case 0:
		suffix = "LEFT";
		break;
	case 1:
		suffix = "DOWN";
		break;
	case 2:
		suffix = "UP";
		break;
	case 3:
		suffix = "RIGHT";
		break;
	}
	return suffix;
}


std::string laneSuffLower(int lane)
{
	std::string suffix = "";
	switch (lane % 4)
	{
	case 0:
		suffix = "left";
		break;
	case 1:
		suffix = "down";
		break;
	case 2:
		suffix = "up";
		break;
	case 3:
		suffix = "right";
		break;
	}
	return suffix;
}

void Gameplay::initControls()
{
	controls.clear();

	gameplayControl left;
	left.code = Game::save->GetDouble("Left Key");
	left.lane = 4;
	gameplayControl down;
	down.code = Game::save->GetDouble("Down Key");
	down.lane = 5;
	gameplayControl up;
	up.code = Game::save->GetDouble("Up Key");
	up.lane = 6;
	gameplayControl right;
	right.code = Game::save->GetDouble("Right Key");
	right.lane = 7;

	controls.push_back(left);
	controls.push_back(down);
	controls.push_back(up);
	controls.push_back(right);
}

const float min(const float a, const float b)
{
	return (b < a) ? b : a;
}

std::string Gameplay::returnAcc()
{
	std::string format = std::to_string((double)((int)(accuracy * 100)) / 100);
	format.erase(format.find_last_not_of('0') + 1, std::string::npos);

	if (floor(accuracy) == accuracy)
		format.erase(format.find_last_not_of('.') + 1, std::string::npos);
	return format;
}


void Gameplay::updateAccuracy(double hitWorth)
{
	if (hitWorth != -1)
	{
		notesPlayed++;
		notesHit += hitWorth;
	}
	accuracy = ((notesHit / notesPlayed) * 100);
	if (accuracy < 0)
		accuracy = 0;
	if (notesPlayed == 0)
		accuracy = 0;
	scoreTxt->setText("Score: " + std::to_string(score) + " | Combo Breaks: " + std::to_string(CB) + " | Accuracy: " + returnAcc() + "%");
	scoreTxt->centerX();
}

void Gameplay::removePhase()
{
	EventFunction::destroyEventSystem();
	for (AvgGroup* g : stage->returnUsedLayers())
	{
		for (Object* obj : g->children)
			delete obj;
	}
	delete smoke;
	delete startTex;
	delete graphix;
	graphix = NULL;

	for (Object* obj : camHud->children)
		if (obj)
			delete obj;

	for (Object* obj : camGame->children)
		if (obj)
			delete obj;

	// tex's already deleted
	healthBG->tex->fromSTBI = false;
	iconP1->dontDeleteTex = true;
	iconP2->dontDeleteTex = true;
	delete healthBG;
	delete healthProgP1;
	delete healthProgP2;

	controls.clear();

	receptors.clear();
	colGroups.clear();
	spawnedNotes.clear();
	notesToPlay.clear();

	song->free();
	clap->free();

	SoundManager::removeChannel("gameplaySong");
	SoundManager::removeChannel("clapFx");
}

void Gameplay::removeNote(NoteObject* object)
{
	spawnedNotes.erase(
		std::remove_if(spawnedNotes.begin(), spawnedNotes.end(), [&](NoteObject* const nn) {
			return nn->beat == object->beat && nn->lane == object->lane;
			}),
		spawnedNotes.end());
	removeObj(object);
	delete object;
}

void Gameplay::createJudgement(judgement judge)
{
	AvgSprite* judg = new AvgSprite(0, 0, NULL);
	judg->tex = graphix;
	judg->dontDeleteTex = true;
	judg->setSparrow("assets/graphical/fnfBase/ui.xml");
	switch (judge)
	{
		case judgement::Judge_sick:
			judg->playAnim("sick", 1, false);
			break;
		case judgement::Judge_good:
			judg->playAnim("good", 1, false);
			break;
		case judgement::Judge_bad:
			judg->playAnim("bad", 1, false);
			break;
		case judgement::Judge_shit:
			judg->playAnim("shit", 1, false);
			break;
	}

	judg->scale = 0.7;

	judg->staticView = true;
	judg->x = Game::gameWidth * 0.3;
	judg->y = Game::gameHeight / 2;
	judg->acclY = 550;
	judg->velX -= rand() % ((10 + 1));
	judg->velY -= 140 + rand() % ((175 + 1) - 140);

	std::vector<std::string> endForm;

	std::string cmb = std::to_string(combo);

	if (cmb.size() == 2)
		endForm.push_back("0");

	for (int i = 0; i < cmb.size(); i++)
	{
		char c = cmb[i];
		std::string bru = "";
		bru.push_back(c);
		endForm.push_back(bru);
	}

	int i = 0;

	camHud->add(judg);
	judg->alpha = 1;

	std::vector<AvgSprite*> nums;

	if (combo >= 10)
		for (std::string num : endForm)
		{
			AvgSprite* numb = new AvgSprite(judg->x + (43 * i) - 50, judg->y + 100, NULL);
			numb->scale = 0.5;
			numb->staticView = true;
			numb->dontDeleteTex = true;
			numb->acclY = 200 + rand() % ((300 + 1) - 200);
			numb->velY -= 130 + rand() % ((160 + 1) - 140);
			numb->velX = -5 + rand() % ((5 + 1) - -5);

			numb->alpha = 1;

			camHud->add(numb);

			numb->tex = graphix;
			numb->setSparrow("assets/graphical/fnfBase/ui.xml");

			numb->playAnim("num" + num, 1, false);

			i++;
			nums.push_back(numb);
		}

	judgementSprite spr;

	spr.judgement = judg;
	spr.numbers = nums;
	spr.startJudge = false;
	spr.startNum = false;
	spr.time = 0;
	spr.del = false;
	judgeAnims.push_back(spr);
}

void Gameplay::miss(NoteObject* object)
{
	updateAccuracy(-0.4);
	combo = 0;
	CB++;

	stage->p1->play("sing" + laneSuff(object->lane) + "-MISS");
	if (stage->eventFile)
	{
		for (auto ev : stage->eventFile->events)
		{
			if (ev.beat < beat && (ev.length == -1 || ev.length + ev.beat > beat))
				EventFunction::runMiss(ev.name);
		}
	}
	health -= 0.05;
	if (health < 0)
	{
		// TODO: die
		health = 0;
	}	
}

Gameplay::Gameplay()
{
}

void Gameplay::create() {

	
	addCamera(Game::mainCamera);
	initControls();

	graphix = Texture::createWithImage("assets/graphical/fnfBase/ui.png");

	avatars.clear();

	if (SoundManager::getChannelByName("prevSong") != NULL)
	{
		Channel* c = SoundManager::getChannelByName("prevSong");
		c->stop();
		c->free();
		delete c;
		SoundManager::removeChannel("prevSong");
	}

	downscroll = Game::save->GetBool("downscroll");

	Judge::initJudge();

	smoke = Texture::createWithImage("assets/graphical/tricky/smoke.png");

	//background = new AvgSprite(0, 0, bg);
	//background->w = Game::gameWidth;
	//background->h = Game::gameHeight;
	//add(background);

	camGame = new AvgGroup(0, 0, Game::gameWidth, Game::gameHeight);

	camHud = new AvgGroup(0, 0, Game::gameWidth, Game::gameHeight);

	if (SongSelect::currentChart->meta.folder.ends_with("ImprobableOutset"))
		stage = new Stage("assets/stages/nevada");
	if (SongSelect::currentChart->meta.folder.ends_with("Madness"))
		stage = new Stage("assets/stages/spookyNevada");
	if (SongSelect::currentChart->meta.folder.ends_with("Hellclown"))
		stage = new Stage("assets/stages/hellNevada");
	if (SongSelect::currentChart->meta.folder.ends_with("Expurgation"))
		stage = new Stage("assets/stages/exNevada");
	for (AvgGroup* group : stage->returnUsedLayers())
	{
		camGame->add(group);
	}

	add(camGame);

	add(camHud);

	EventFunction::initEvents();
	EventFunction::sendOutStageVars();

	if (stage->eventFile)
		for (auto ev : stage->eventFile->events)
			EventFunction::sendArg(ev.name, ev.args);

	float bassRate = (rate * 100) - 100;

	std::string path = SongSelect::currentChart->meta.folder + "/" + SongSelect::currentChart->meta.audio;

	std::cout << "playing " << path << std::endl;

	song = SoundManager::createChannel(path.c_str(), "gameplaySong");
	clap = SoundManager::createChannel("assets/sounds/hitSound.mp3", "clapFx");

	song->createFXStream();

	songLength = song->length;

	int diff = SongSelect::selectedDiffIndex;

	noteskin = Noteskin::getNoteskin();
	specialNoteskin = Noteskin::getSpecialNoteskin();

	startTex = Texture::createWithImage("assets/graphical/fnfBase/start.png");

	startSprite = new AvgSprite(Game::gameWidth / 2, Game::gameHeight / 2, startTex);
	startSprite->dontDeleteTex = true;
	startSprite->staticView = true;

	startSprite->alpha = 0;

	startSprite->setSparrow("assets/graphical/fnfBase/start.xml");

	notesToPlay = SongSelect::currentChart->meta.difficulties[diff].notes;

	for (int i = 0; i < 4; i++)
	{
		AvgSprite* r;
		float x = ((Game::gameWidth / 2)) 
			- ((104 * Game::save->GetDouble("Note Size") + 2) * 5) 
			+ ((104 * Game::save->GetDouble("Note Size") + 2) * i);
		if (downscroll)
			r = new AvgSprite(x, (Game::gameHeight / 2) + 250, NULL);
		else
			r = new AvgSprite(x, (Game::gameHeight / 2) - 300, NULL);
		r->dontDeleteTex = true;
		r->tex = noteskin;
		r->setSparrow("assets/graphical/fnfBase/Arrows.xml");

		r->playAnim("arrow" + laneSuff(i),24,false);
		r->staticView = true;

		receptors.push_back(r);
	}

	for (int i = 4; i < 8; i++)
	{
		AvgSprite* r;

		float x = (((Game::gameWidth / 2)) 
			- ((104 * Game::save->GetDouble("Note Size") + 2) * 2) 
			+ ((104 * Game::save->GetDouble("Note Size") + 2) * i)) - 52;

		if (downscroll)
			r = new AvgSprite(x - 52, (Game::gameHeight / 2) + 250, NULL);
		else
			r = new AvgSprite(x - 52, (Game::gameHeight / 2) - 300, NULL);
		r->dontDeleteTex = true;
		r->tex = noteskin;
		r->setSparrow("assets/graphical/fnfBase/Arrows.xml");

		r->playAnim("arrow" + laneSuff(i),24,false);

		r->staticView = true;
		receptors.push_back(r);
	}


	for (int i = 0; i < receptors.size(); i++)
	{
		AvgSprite* r = receptors[i];
		camHud->add(r);
		r->scale = 0.666;
	}

	healthBG = new AvgSprite(Game::gameWidth / 2, Game::gameHeight * 0.9, "assets/graphical/fnfBase/healthBar.png");
	camHud->add(healthBG);

	scoreTxt = new Text(0, 0, "t", 16, "vcr");
	scoreTxt->centerX();
	scoreTxt->staticView = true;
	scoreTxt->y = healthBG->y + 50;
	scoreTxt->border = true;
	scoreTxt->borderSize = 1.2;

	healthBG->x -= healthBG->w / 2;

	healthProgP2 = new AvgRect(healthBG->x + 4, (Game::gameHeight * 0.9) + 4, 0, healthBG->h - 8);
	camHud->add(healthProgP2);
	healthProgP2->c = { 255,0,0 };

	healthProgP1 = new AvgRect(healthBG->x, (Game::gameHeight * 0.9) + 4, 0, healthBG->h - 8);
	camHud->add(healthProgP1);
	healthProgP1->c = { 0,255,0 };

	healthBG->staticView = true;
	healthProgP2->staticView = true;
	healthProgP1->staticView = true;

	camHud->add(scoreTxt);

	iconP1 = new AvgSprite(120, 0, NULL);
	iconP1->tex = stage->p1->icon;
	iconP2 = new AvgSprite(0, 0, NULL);
	iconP2->tex = stage->p2->icon;

	iconP1->dontDeleteTex = true;
	iconP2->dontDeleteTex = true;

	iconP2->offsetX = stage->p2->iconOffset.x;
	iconP2->offsetY = stage->p2->iconOffset.y;
	iconP1->offsetX = stage->p1->iconOffset.x;
	iconP1->offsetY = stage->p1->iconOffset.y;


	add(iconP1);
	add(iconP2);

	add(icons);

	iconP1->w = iconP1->tex->width / 2;
	iconP1->h = iconP1->tex->height;
	iconP2->w = iconP2->tex->width / 2;
	iconP2->h = iconP2->tex->height;
	iconP1->customSrc = stage->p1->getIconSrc(0);
	iconP1->angle = 180;
	iconP1->customSrc.h = -1;
	iconP2->customSrc = stage->p2->getIconSrc(0);

	iconP1->staticView = true;
	iconP2->staticView = true;


	songPosBar = new AvgRect(receptors[0]->x, 24, ((receptors[3]->x + (104 * Game::save->GetDouble("Note Size"))) - receptors[0]->x) * (positionInSong / (songLength)), 24);
	//camHud->add(songPosBar);
	songPosBar->staticView = true;
	songPosBar->c.r = 255;
	songPosBar->c.g = 255;
	songPosBar->c.b = 255;

	if (downscroll)
		songPosBar->y = (receptors[0]->y + (104 * Game::save->GetDouble("Note Size"))) + 12;

	songPosOutline = new AvgRect(songPosBar->x,songPosBar->y, (receptors[3]->x + (104 * Game::save->GetDouble("Note Size"))) - receptors[0]->x, 24);
	songPosOutline->border = true;
	//add(songPosOutline);
	songPosOutline->c.r = 255;
	songPosOutline->c.g = 255;
	songPosOutline->c.b = 255;
	created = true;
	float thing = ((60 / SongSelect::currentChart->getSegmentFromTime(0).bpm) * 1000);
	positionInSong = -(thing * 12);

	startSprite->scale = 0.65;
	camHud->add(startSprite);

	updateAccuracy(-1);

	startSound = SoundManager::createChannel("assets/sounds/intro3.ogg", "startChannel");

	cameraMove = Easing::getEasingFunction(Easing::EaseOutBack);
	startTime = 0;
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

float lastBPM = 0;

void Gameplay::update(Events::updateEvent event)
{
	float thing = ((60 / SongSelect::currentChart->getSegmentFromTime(0).bpm) * 1000);
	if (startTimeDur < thing)
		startTimeDur += Game::deltaTime;
		if (positionInSong > startTime)
		{
			if (!play)
			{
				song->setPos(0);
				song->setVolume(0.6);
				song->play();
				play = true;
				lastBPM = 0;
			}
		}
		else
		{
			if (startTimeDur > thing)
			{
				startTimeDur = 0;
				switch (startProg)
				{
				case 0:
					startSound->play();
					break;
				case 1:
					startSprite->offsetX = -85;
					startSprite->offsetY = -100;
					SoundManager::removeChannel("startChannel");
					startSound = SoundManager::createChannel("assets/sounds/intro2.ogg", "startChannel");
					startSprite->playAnim("ready", 24, false);
					startSound->play();
					break;
				case 2:
					SoundManager::removeChannel("startChannel");
					startSound = SoundManager::createChannel("assets/sounds/intro1.ogg", "startChannel");
					startSprite->playAnim("set", 24, false);
					startSound->play();
					break;
				case 3:
					startSprite->offsetX = -120;
					startSprite->offsetY = -80;
					SoundManager::removeChannel("startChannel");
					startSound = SoundManager::createChannel("assets/sounds/introGo.ogg", "startChannel");
					startSprite->playAnim("go", 24, false);
					startSound->play();
					break;
				case 4:
					SoundManager::removeChannel("startChannel");
					startSound = SoundManager::createChannel("assets/sounds/intro3.ogg", "startChannel");
					positionInSong = 0;
					break;
				}
				if (positionInSong < 0 && startProg > 0 && startProg < 3)
				{
					startSprite->x = (Game::gameWidth / 2) - (startSprite->w / 2);
					startSprite->y = (Game::gameHeight / 2) - (startSprite->h / 2);
					startSprite->alpha = 1;
				}
				startProg++;
			}
			
		}
	if (startSprite->alpha > 0.0)
		startSprite->alpha = lerp(1, 0, startTimeDur / thing);
	

	positionInSong += Game::deltaTime;
	if (play)
		positionInSong -= Game::save->GetDouble("offset");

	if (play)
		if (song->getPos() - Game::save->GetDouble("offset") > positionInSong + 20 || song->getPos() - Game::save->GetDouble("offset") < positionInSong - 20)
		{
			positionInSong = song->getPos() - Game::save->GetDouble("offset");
		}

	if (firstHit)
	{
		if (focus)
		{
			float p1X = stage->p1->x + (stage->p1->w / 2) + stage->bfOff.x + viewOffsetX;
			float p1Y = stage->p1->y + (stage->p1->h / 2) + stage->bfOff.y + viewOffsetY;

			Game::mainView->x += ((p1X - (Game::gameWidth / 2)) - Game::mainView->x) * 0.01;
			Game::mainView->y += ((p1Y - (Game::gameHeight / 2)) - Game::mainView->y) * 0.01;
		}
		else
		{
			float p2X = stage->p2->x + (stage->p2->w / 2) + stage->dadOff.x + viewOffsetX;
			float p2Y = stage->p2->y + (stage->p2->h / 2) + stage->dadOff.y + viewOffsetY;

			Game::mainView->x += ((p2X - (Game::gameWidth / 2)) - Game::mainView->x) * 0.01;
			Game::mainView->y += ((p2Y - (Game::gameHeight / 2)) - Game::mainView->y) * 0.01;
		}

		
	}


	SDL_FRect bruh;
	bruh.x = 0;
	bruh.y = 0;
	bruh.h = 720;
	bruh.w = 1280;

	Rect ll;
	ll.x = 0;
	ll.y = 0;
	ll.w = 104 * Game::save->GetDouble("Note Size");
	ll.h = Game::gameHeight;

	for (int indexG = 0; indexG < 8; indexG++)
	{
		for (NoteObject* obj : spawnedNotes) // reset clips
		{
			if (obj->lane == indexG)
			{
				obj->clipRect.w = 0;
				obj->clipRect.h = 0;
			}
		}
	}

	SDL_FRect laneUnderway;

	laneUnderway.x = receptors[0]->x - 4;
	laneUnderway.y = -200;
	laneUnderway.w = (receptors[3]->x - laneUnderway.x) + (68 * Game::save->GetDouble("Note Size"));
	laneUnderway.h = 1280;

	// gaming for da anims

	for (judgementSprite& spr : judgeAnims)
	{
		// "crochet" is really just bpms

		float crochet = ((curSeg.bpm / 60) * 1000);

		spr.time += Game::deltaTime * 0.28;
		spr.numTime += Game::deltaTime * 0.28;

		if (spr.time > crochet * 0.01 && !spr.startJudge)
		{
			spr.startJudge = true;
			spr.time = 0;
		}

		if (spr.startJudge)
			if (spr.time < 200)
				spr.judgement->alpha = 1 - (spr.time / 200);

		if (spr.numTime > crochet * 0.02 && !spr.startNum)
		{
			spr.startNum = true;
			spr.numTime = 0;
		}

		if (spr.startNum)
			for (AvgSprite* num : spr.numbers)
				if (spr.numTime < 204)
					num->alpha = 1 - (spr.numTime / 204);

		if (spr.time > 200 && spr.numTime > 204)
			spr.del = true;
	}

	judgeAnims.erase(
		std::remove_if(judgeAnims.begin(), judgeAnims.end(), [&](judgementSprite& const nn) {
			if (nn.del)
			{
				camHud->removeObj(nn.judgement);
				for (AvgSprite* num : nn.numbers)
					camHud->removeObj(num);

			}
			return nn.del;
			}),
		judgeAnims.end());
	

	//SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 150);
	//SDL_RenderFillRectF(Game::renderer, &laneUnderway);
	//SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);


	if (!SongSelect::currentChart)
		return;

	curSeg = SongSelect::currentChart->getSegmentFromTime(positionInSong);
	beat = SongSelect::currentChart->getBeatFromTimeOffset(positionInSong, curSeg);
	if (lastBPM != curSeg.bpm && Game::gameplayEvents_DB)
	{
		Game::instance->db_addLine("bpm change to " + std::to_string(curSeg.bpm));
		song->bpm = curSeg.bpm;
		lastBPM = curSeg.bpm;
	}

	// update focus
	if ((int)beat % 4 == 0) // 4/4 only sorry!
	{
		bool storedFocus = focus;

		// if notes exist

		bool notes = false;

		for (NoteObject* object : spawnedNotes)
			if (object->lane >= 4 && object->beat < beat + 4 && (object->active || object->holdsActive > 0))
				notes = true;

		if (stage->p1->currentTime > 0 && notes)
			focus = true;
		else
			readyToMove = true;
	}

	// gf dance

	if ((int)beat % 1 == 0 && (int)beat != lastDanced && stage->gf)
	{
		lastDanced = beat;
		if (!dance)
			stage->gf->play("danceLeft");
		else
			stage->gf->play("danceRight");
		dance = !dance;
	}


	// health bar

	healthProgP2->w = ((healthBG->w) * (1 - health)) - 4;
	healthProgP1->w = (healthBG->w * health) - 6;
	healthProgP1->x = healthProgP2->x + healthProgP2->w;
	healthProgP2->x = healthBG->x + 6;

	iconP2->y = healthProgP2->y - 75;
	iconP2->x = ((healthProgP2->x + healthProgP2->w) - 140);

	iconP1->y = healthProgP1->y - 75;
	iconP1->x = (iconP2->x + stage->p2->iconOffset.x) + 120;

	if ((int)beat % 4 == 0 && lastbeat != (int)beat && (int)beat > 0 && firstHit)
	{
		lastbeat = (int)beat;
		Game::mainCamera->scale += 0.06;
	}

	if (Game::mainCamera->scale > 1.000)
	{
		Game::mainCamera->scale = lerp(1, Game::mainCamera->scale, 0.95);
	}

	if ((int)beat % 1 == 0 && lastIcon != (int)beat)
	{
		lastIcon = (int)beat;
		iconP1->scale = 1.15;
		iconP2->scale = 1.15;
	}

	if (iconP1->scale > 1)
		iconP1->scale = lerp(iconP1->scale, 1, .1);
	if (iconP2->scale > 1)
		iconP2->scale = lerp(iconP2->scale, 1, .1);

	iconP1->y += (150 * (1 - iconP1->scale)) / 2;

	iconP2->x += (150 * (1 - iconP2->scale));
	iconP2->y += (150 * (1 - iconP2->scale)) / 2;

	
	songPosBar->w = ((receptors[3]->x + (104 * Game::save->GetDouble("Note Size"))) - receptors[0]->x) * (positionInSong / (songLength));

	// underlay for accuracy

	SDL_FRect overlayForAccuracy;



	// debug stuff


	//SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 128);
	//SDL_RenderFillRectF(Game::renderer, &overlayForAccuracy);
	//SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);

	// multiplayer shit

	// leaderboard

	//SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);

	//SDL_RenderFillRectF(Game::renderer, &posBar);

	//SDL_RenderDrawRectF(Game::renderer, &outline);

	// spawning shit

	if (notesToPlay.size() > 0)
	{
			note& n = notesToPlay[0];
			if (n.beat < beat + 16 && !n.played && (n.type != Note_Tail)) // if its in 16 beats
			{
				n.played = true;
				NoteObject* object = new NoteObject();
				object->customDraw = true;
				
				object->dontDeleteTex = true;
				object->mine = n.type == Note_Mine;
				if (!object->mine)
				{
					object->overrideH = 104;
					object->overrideW = 104;
				}
				else
				{
					object->scale = 0.6;
					if (stage->convertMinesToDeath)
						object->offsetX = 135;
					else
						object->offsetX = 30;
				}
				if (object->mine)
				{
					object->tex = specialNoteskin;
					object->setSparrow("assets/graphical/tricky/SpecialityNotes.xml");
				}
				else
				{
					object->tex = noteskin;
					object->setSparrow("assets/graphical/fnfBase/Arrows.xml");
				}
				object->connected = &n;
				SDL_FRect rect;
				object->wasHit = false;
				object->clapped = false;
				object->active = true;

				bpmSegment preStopSeg = SongSelect::currentChart->getSegmentFromBeat(n.beat);

				float stopOffset = SongSelect::currentChart->getStopOffsetFromBeat(n.beat);

				double stopBeatOffset = (stopOffset / 1000) * (preStopSeg.bpm / 60);

				object->stopOffset = stopBeatOffset;

				object->beat = (double) n.beat + stopBeatOffset;
				object->lane = n.lane;
				object->staticView = true;

				std::string suffix = " alone";

				if (object->mine)
				{
					if (stage->convertMinesToDeath)
						suffix = " alone death";
					else
						suffix = " alone fire";
				}

				switch (object->lane % 4)
				{
					case 0:
						object->playAnim("purple" + suffix, 24, true);
						break;
					case 1:
						object->playAnim("blue" + suffix, 24, true);
						break;
					case 2:
						object->playAnim("green" + suffix, 24, true);
						break;
					case 3:
						object->playAnim("red" + suffix, 24, true);
						break;
				}

				object->type = n.type;
				object->endTime = -1;
				object->endBeat = -1;

				bpmSegment noteSeg = SongSelect::currentChart->getSegmentFromBeat(object->beat);

				object->time = SongSelect::currentChart->getTimeFromBeatOffset(object->beat, noteSeg);
				rect.y = Game::gameHeight + 400;
				rect.x = 0;
				rect.w = 104 * Game::save->GetDouble("Note Size");
				rect.h = 104 * Game::save->GetDouble("Note Size");
				object->rect = rect;

				note tail;

				bpmSegment bruh = SongSelect::currentChart->getSegmentFromBeat(object->beat);

				float wh = SongSelect::currentChart->getTimeFromBeat(beat, bruh);

				float bps = (Game::save->GetDouble("scrollspeed") / 60) / Gameplay::rate;


				if (object->type == Note_Head)
				{
					for (int i = 0; i < notesToPlay.size(); i++)
					{
						note& nn = notesToPlay[i];
						if (nn.type != Note_Tail)
							continue;
						if (nn.lane != object->lane)
							continue;

						bpmSegment npreStopSeg = SongSelect::currentChart->getSegmentFromBeat(nn.beat);

						float nstopOffset = SongSelect::currentChart->getStopOffsetFromBeat(nn.beat);

						double nstopBeatOffset = (nstopOffset / 1000) * (npreStopSeg.bpm / 60);

						object->endBeat = nn.beat + nstopBeatOffset;
						
						object->endTime = SongSelect::currentChart->getTimeFromBeatOffset(nn.beat + nstopBeatOffset, noteSeg);
						tail = nn;
						break;
					}
				}

				float time = SDL_GetTicks();

				if (object->type == Note_Head)
				{

					for (int i = std::floorf(object->time); i < std::floorf(object->endTime); i++)
					{
						bpmSegment holdSeg = SongSelect::currentChart->getSegmentFromTime(i);

						double beat = SongSelect::currentChart->getBeatFromTimeOffset(i, holdSeg);

						float whHold = SongSelect::currentChart->getTimeFromBeatOffset(beat, holdSeg);

						float diff = whHold - (object->time);

						float noteOffset = (bps * (diff / 1000)) * (104 * Game::save->GetDouble("Note Size"));

						float y = 0;
						float yDiff = 0;
						if (object->heldTilings.size() != 0)
						{
							if (downscroll)
								y = object->rect.y - noteOffset;
							else
								y = object->rect.y + noteOffset;
							yDiff = y - object->heldTilings.back().rect.y;
						}
						else
						{
							if (downscroll)
								y = object->rect.y - noteOffset;
							else
								y = object->rect.y + noteOffset;
							yDiff = y - object->rect.y;
						}

						bool otherOne = false;

						if (downscroll)
							otherOne = yDiff <= -(63 * Game::save->GetDouble("Note Size"));
						else
							otherOne = yDiff >= 63 * Game::save->GetDouble("Note Size");

						if (otherOne || object->heldTilings.size() == 0)
						{
							object->holdHeight += 63 * Game::save->GetDouble("Note Size");
							holdTile tile;
							SDL_FRect rect;
							tile.active = true;
							tile.fucked = false;
							rect.y = y;
							rect.x = 0;
							tile.rect = rect;
							tile.beat = beat;
							tile.time = i;
							object->heldTilings.push_back(tile);
						}
					}
				}
				std::sort(object->heldTilings.begin(), object->heldTilings.end());
				notesToPlay.erase(notesToPlay.begin());
				spawnedNotes.push_back(object);
				add(object);
			}
			else if (n.type == Note_Tail)
			{
				notesToPlay.erase(notesToPlay.begin());
			}
	}
	else
	{
		if (!ended && spawnedNotes.size() == 0 && notesToPlay.size() == 0 && (positionInSong > (songLength) - 1000))
		{
			ended = true;

			SongSelect::currentChart->destroy();
			cleanUp();
			Game::instance->transitionToMenu(new SongSelect());
		}
	}

	
	if (stage->p1->currentTime > 0)
		stage->p1->currentTime -= Game::deltaTime;
	if (stage->p1->currentTime <= 0 && stage->p1->sparrow->currentAnim != stage->p1->setAnims["idle"])
	{
		stage->p1->play("idle");
	}
	
	if (stage->p2->currentTime > 0)
		stage->p2->currentTime -= Game::deltaTime;
	if (stage->p2->currentTime <= 0 && stage->p2->sparrow->currentAnim != stage->p2->setAnims["idle"])
	{
		stage->p2->play("idle");
	}
		for (int i = 0; i < spawnedNotes.size(); i++)
		{
			NoteObject* note = spawnedNotes[i];

			if (!note->destroyed)
			{
				float wh = SongSelect::currentChart->getTimeFromBeat(note->beat, SongSelect::currentChart->getSegmentFromBeat(note->beat));

				if (wh - positionInSong < 2)
				{
					if (Game::save->GetBool("hitsounds") && !note->clapped && note->lane >= 4)
					{
						note->clapped = true;
						if (SoundManager::getChannelByName("clapFx") != NULL)
							clap->play();
					}
					if ((botplay || note->lane < 4 && (!note->mine || (note->mine && (stage->p2->name == "trickyH" || stage->p2->name == "trickyEx")))) && note->active)
					{
						if (note->lane >= 4)
						{
							float diff = (wh - positionInSong);

							std::string format = std::to_string(diff - fmod(diff, 0.01));
							format.erase(format.find_last_not_of('0') + 1, std::string::npos);

							
							score += Judge::scoreNote(diff);
							updateAccuracy(1);
							combo++;
						
							createJudgement(judgement::Judge_sick);

							scaleTime = 350;
							scaleStart = 0;
						
						}
						else
						{
							receptors[note->lane]->playAnim(laneSuffLower(note->lane) + " confirm", 24, false);

							receptors[note->lane]->centerOrigin = false;
							receptors[note->lane]->offsetX = 24;
							receptors[note->lane]->offsetY = 24;
							if (stage->p2->shake)
								Game::mainCamera->shake(20, .2);
							stage->p2->play("sing" + laneSuff(note->lane));
							if (stage->eventFile)
							{
								for (auto ev : stage->eventFile->events)
								{
									if (ev.beat < beat && (ev.length == -1 || ev.length + ev.beat > beat))
										EventFunction::runSing(ev.name, true);
								}
							}
							if (readyToMove)
							{
								readyToMove = false;
								focus = false;
							}
							else if (focus && stage->p1->currentTime < 250)
							{
								focus = true;
							}
						}
						if (!firstHit)
						{
							firstHit = true;
							focus = false;
						}
						note->active = false;


					}

				}

				// BEFORE DRAW, CHECK HELD NOTES!!

				if (note->type == Note_Head)
				{
					for (int i = 0; i < note->heldTilings.size(); i++)
					{
						holdTile& tile = note->heldTilings[i];
						float wh = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat));
						float offset = wh;
						float timeDiff = offset - positionInSong;
						if (holding[note->lane] || (botplay || (note->lane < 4 && !note->mine))) // holding that lane!
						{

							if (timeDiff <= Judge::hitWindows[3] && !tile.fucked)
							{
								if (note->lane < 4 && timeDiff <= 0 && receptors[note->lane]->animationFinished)
								{
									receptors[note->lane]->playAnim(laneSuffLower(note->lane) + " confirm", 24, false);

									receptors[note->lane]->centerOrigin = false;
									receptors[note->lane]->offsetX = 24;
									receptors[note->lane]->offsetY = 24;
								}
								tile.active = false;
								if (note->lane < 4)
									stage->p2->currentTime = 200;
								else
									stage->p1->currentTime = 200;
							}
						}
						if (note->lane < 4)
							continue;
					}

					if (!holding[note->lane] && lastHolding[note->lane]) // just stopped holding
					{
						lastHolding[note->lane] = false;

						if (note->endTime < positionInSong + Judge::hitWindows[1] * 0.7)
						{
							removeNote(note); // might end up being weird if many notes are being held.
							break;
						}
					}
				}

				bool fuckOver = false;

				for (int i = 0; i < note->heldTilings.size(); i++)
				{
					holdTile& tile = note->heldTilings[i];
					if (fuckOver)
						tile.fucked = true;
				}

				if (note->lane >= 0)
				{
					Rect receptorRect;
					receptorRect.w = receptors[note->lane]->w;
					receptorRect.h = receptors[note->lane]->h;
					receptorRect.x = receptors[note->lane]->x;
					receptorRect.y = receptors[note->lane]->y;
					note->debug = debug;

					Rect l;
					l.x = 0;
					l.y = (receptorRect.y + 52) * camHud->scale;
					l.w = Game::gameWidth;
					l.h = Game::gameHeight;
					if (downscroll)
					{
						l.y = 0;
						l.h = receptorRect.y + 52;
					}

					//SDL_SetRenderTarget(Game::renderer, Game::mainCamera->cameraTexture);

					if (holding[note->lane] || botplay || (note->lane < 4 && !note->mine))
					{
						
						for (NoteObject* obj : spawnedNotes) // reset clips
						{
							if (obj->lane == note->lane)
							{
								obj->clipRect = l;
							}
						}
						
					}

				}

				if (wh - positionInSong <= -Judge::hitWindows[2] && note->active && playing && !note->mine)
				{
					note->active = false;
					miss(note);
				}

				bool condition = true;


				if ((wh - positionInSong <= -200 && !note->active) && note->holdsActive == 0 && playing)
				{
					removeNote(note);
				}

				for (int i = 0; i < note->heldTilings.size(); i++)
				{
					holdTile& tile = note->heldTilings[i];

					float whHold = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat));
					float diff = whHold - positionInSong;

					if (diff < -Judge::hitWindows[1] && tile.active && playing)
					{
						miss(note);
						removeNote(note);
						break;
					}
				}
			}
		}

	for (int i = 0; i < receptors.size(); i++)
	{
		AvgSprite* r = receptors[i];
		if (keys[i] || holding[i])
		{
			std::string pref = laneSuffLower(i);
			if (r->sparrow->currentAnim != pref + " confirm" && r->sparrow->currentAnim != pref + " press")
			{
				r->playAnim(pref + " press", 24, false);
				r->offsetX = -2;
				r->offsetY = -2;
			}
		}
		else
		{
			if (r->sparrow->currentAnim != "arrow" + laneSuff(i) && (r->animationFinished && i < 4) || i >= 4)
			{
				r->playAnim("arrow" + laneSuff(i), 24, false);
				r->offsetX = 0;
				r->offsetY = 0;
			}
		}
	}

	if (stage->eventFile)
	{
		for (auto ev : stage->eventFile->events)
		{
			if (ev.beat < beat && (ev.length == -1 || ev.length + ev.beat > beat))
				EventFunction::runUpdate(ev.name);
		}
	}

	for (int i = 0; i < smokes.size(); i++)
	{
		AvgSprite* spr = smokes[i].burn;
		if (spr->animationFinished)
		{
			SoundManager::removeChannel("burnSound " + smokes[i].lane);
			camHud->removeObj(spr);
			smokes.erase(std::remove(smokes.begin(), smokes.end(), smokes[i]), smokes.end());
		}
	}
	
}

void Gameplay::cleanUp()
{
}

void Gameplay::keyDown(SDL_KeyboardEvent event)
{
	

	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE:
			
			
			SongSelect::currentChart->destroy();
			cleanUp();
			Game::instance->transitionToMenu(new MainMenu());
		
			return;
		case SDLK_F1:
			
			botplay = !botplay;
			return;
		case SDLK_F2:
			debug = !debug;
			break;
		case SDLK_BACKQUOTE:
			if (play)
			{
				notesToPlay.clear();
				notesToPlay = SongSelect::currentChart->meta.difficulties[SongSelect::selectedDiffIndex].notes;
				startProg = 0;
				for (NoteObject* obj : spawnedNotes) // reset clips
				{
					if (obj)
					{
						removeObj(obj);
						delete obj;
					}
				}

				song->stop();
				clap->stop();
				float thing = ((60 / SongSelect::currentChart->getSegmentFromTime(0).bpm) * 1000);
				positionInSong = -(thing * 12);

				combo = 0;
				score = 0;
				botplay = false;
				play = false;
				CB = 0;
				ended = false;
				notesHit = 0;
				notesPlayed = 0;
				accuracy = 0;

				firstHit = false;

				Game::mainView->x = stage->viewPointPos.x;
				Game::mainView->y = stage->viewPointPos.y;
				focus = false;

				stage->p1->play("idle");
				stage->p2->play("idle");

				updateAccuracy(0);

				spawnedNotes.clear();
			}
			return;
		case SDLK_SPACE:
			if (Game::instance->flowtime )
			{
				if (playing)
				{
					playing = false;
					song->stop();
				}
				else
				{ 
					Game::instance->db_addLine("Set pos to " + std::to_string(positionInSong));
					song->play();
					song->setPos(positionInSong - 50);
					playing = true;
				}
				
			}
			break;
		case SDLK_UP:
			if (Game::instance->flowtime)
			{
				if (!playing)
					positionInSong -= 10;
				else
				{
					playing = false;
					song->stop();
					positionInSong -= 10;
				}
			}
			break;
		case SDLK_DOWN:
			if (Game::instance->flowtime)
			{
				if (!playing)
					positionInSong += 10;
				else
				{
					playing = false;
					song->stop();
					positionInSong += 10;
				}
			}
			break;
		case SDLK_EQUALS:
			if (Game::instance->flowtime)
			{
				if (playing)
				{
					playing = false;
					song->stop();
				}
				rate += 0.1;
				song->rateChange(rate);
				Game::instance->db_addLine("rate changed to " + std::to_string(rate));
			}
			break;
		case SDLK_MINUS:
			if (Game::instance->flowtime)
			{
				if (playing)
				{
					playing = false;
					song->stop();
				}
				rate -= 0.1;
				song->rateChange(rate);
				Game::instance->db_addLine("rate changed to " + std::to_string(rate));
			}
			break;
	}

	// hit notes

	for (int i = 0; i < 4; i++)
	{
		gameplayControl& control = controls[i];

		if (control.code == event.keysym.sym)
		{
			// note lol

			NoteObject* closestObject = nullptr;
			float currentDiff = Judge::hitWindows[3] + 1;


			for (int n = 0; n < spawnedNotes.size(); n++)
			{
				NoteObject* object = spawnedNotes[n];

				const auto wh = SongSelect::currentChart->getTimeFromBeat(object->beat, SongSelect::currentChart->getSegmentFromBeat(object->beat));

				float diff = std::abs(wh - positionInSong);

				if (object->mine)
				{
					if (diff <= (Judge::hitWindows[2] * 0.8) && diff < currentDiff && object->active && object->lane == control.lane)
					{
						closestObject = object;
						currentDiff = diff;
						break;
					}
				}
				else if (diff <= Judge::hitWindows[3] && diff < currentDiff && object->active && object->lane == control.lane)
				{
					closestObject = object;
					currentDiff = diff;
					break;
				}
			}

			if (!keys[control.lane])
			{
				keys[control.lane] = true;
				if (closestObject)
					if (closestObject->holdsActive > 0)
					{
						holding[control.lane] = true;
						lastHolding[control.lane] = false;
					}

			}
			else
				continue;


			if (!closestObject)
				continue;

			float wh = SongSelect::currentChart->getTimeFromBeat(closestObject->beat, SongSelect::currentChart->getSegmentFromBeat(closestObject->beat));

			float diff = (wh - positionInSong);

			if (closestObject->active && diff <= Judge::hitWindows[3] && diff > -Judge::hitWindows[3])
			{
				if (closestObject->mine)
				{
					health -= 0.2;
					AvgSprite* spr = new AvgSprite(receptors[closestObject->lane]->x, receptors[closestObject->lane]->y, smoke);
					spr->staticView = true;
					spr->dontDeleteTex = true;
					spr->offsetX = 80;
					spr->offsetY = 105;
					spr->scale = 0.9;
					spr->setSparrow("assets/graphical/tricky/smoke.xml");
					camHud->add(spr);
					spr->playAnim("smoke", 24, false);
					burnSprite sprite;
					sprite.burn = spr;
					sprite.lane = closestObject->lane;
					smokes.push_back(sprite);
					closestObject->active = false;
					closestObject->wasHit = true;
					Channel* burn = SoundManager::createChannel("assets/sounds/burnSound.ogg", "burnSound " + closestObject->lane);
					burn->play();
					if (health < 0)
						health = 0;
					return;
				}

				if (stage->p1->shake)
					Game::mainCamera->shake(20, .2);
				stage->p1->play("sing" + laneSuff(closestObject->lane));
				if (stage->eventFile)
				{
					for (auto ev : stage->eventFile->events)
					{
						if (ev.beat < beat && (ev.length == -1 || ev.length + ev.beat > beat))
							EventFunction::runSing(ev.name, false);
					}
				}
				receptors[closestObject->lane]->playAnim(laneSuffLower(closestObject->lane) + " confirm", 24, false);
				
				receptors[closestObject->lane]->centerOrigin = false;
				receptors[closestObject->lane]->offsetX = 24;
				receptors[closestObject->lane]->offsetY = 24;


				if (!focus)
				{
					focus = true;
				}
				if (!firstHit)
				{
					firstHit = true;
				}
				closestObject->active = false;
				closestObject->wasHit = true;

				judgement judge = Judge::judgeNote(diff);
				score += Judge::scoreNote(diff);

				std::string format = std::to_string(diff - fmod(diff, 0.01));
				format.erase(format.find_last_not_of('0') + 1, std::string::npos);

				

				switch (judge)
				{
					case judgement::Judge_sick:
						updateAccuracy(1);
						health += 0.04;
						break;
					case judgement::Judge_good:
						updateAccuracy(0.8);
						health += 0.02;
						break;
					case judgement::Judge_bad:
						updateAccuracy(0.5);
						health += 0.01;
						break;
					case judgement::Judge_shit:
						updateAccuracy(0.1);
						health -= 0.01;
						break;
					}

					if (health > 1)
						health = 1;
					if (health < 0)
						health = 0;
					combo++;
					createJudgement(judge);

					scaleTime = 350;
					scaleStart = 0;
					break;
				}
		}
	}
	
	
}

void Gameplay::keyUp(SDL_KeyboardEvent ev)
{
	for (int i = 0; i < 4; i++)
	{
		gameplayControl& control = controls[i];

		if (control.code == ev.keysym.sym)
		{
			keys[control.lane] = false;
			lastHolding[control.lane] = true;
			holding[control.lane] = false;
		}
	}
}
