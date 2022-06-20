#pragma once
#include "includes.h"
#include "NoteObject.h"
#include "Chart.h"
#include "Menu.h"
#include "MainMenu.h"
#include "Game.h"
#include "Noteskin.h"
#include "Text.h"
#include "bass.h"
#include "Judge.h"
#include <algorithm>
#include "bass_fx.h"
#include "SoundManager.h"
#include <SDL_image.h>
#include "ReceptorObject.h"
#include "Stage.h"
struct gameplayControl {
	int lane;
	SDL_Keycode code;
};

struct judgementSprite {
	AvgSprite* judgement;
	std::vector<AvgSprite*> numbers;
	float time;
	float numTime;
	bool startJudge;
	bool startNum;
	bool del;
};

struct burnSprite {
	AvgSprite* burn;
	int lane;

	friend bool operator==(const burnSprite& lhs, const burnSprite& rhs)
	{
		return lhs.lane == rhs.lane;
	}
};

class Gameplay :
	public Menu
{
	public:
		std::vector<gameplayControl> controls;

		std::vector<AvgSprite*> receptors;

		std::vector<note> notesToPlay;

		bool playing = true;

		AvgSprite* background;

		static float rate;

		float scaleTime;

		virtual void create();

		int noteId;

		double songLength = 0;

		bool ended = false;

		bpmSegment curSeg;
		double beat;
		bool play = false;

		int startProg = 0;

		int aiPlay[4];

		std::vector<judgementSprite> judgeAnims;
		std::vector<NoteObject*> spawnedNotes;

		std::vector<float> clapped;

		float scaleStart = 0;

		std::vector<burnSprite> smokes;
		Channel* startSound;
		Texture* smoke;
		AvgSprite* startSprite;
		Texture* startTex;
		float startTimeDur = 0;

		AvgSprite* healthBG;
		AvgRect* healthProgP1;
		AvgRect* healthProgP2;

		AvgGroup* camHud; // FNF NAME!!!
		AvgGroup* camGame; // FNF NAME!!!

		float lastIcon = 0;

		AvgSprite* iconP1;
		AvgSprite* iconP2;

		float health = 0.5;

		int lastbeat = 0;

		bool focus = false;
		float ftime = 0;
		bool firstHit = false;

		bool dance = false;

		Easing::easingFunction cameraMove;

		float viewOffsetX, viewOffsetY;

		int lastDanced = 0;

		bool keys[8];
		bool holding[8];
		float startTime = 0;

		AvgGroup* icons;

		Stage* stage;

		int combo = 0;

		double accuracy = 0;
		double notesPlayed = 0;
		double notesHit = 0;

		bool readyToMove = false;
		bool botplay = false;

		bool debug = false;

		std::vector<AvgGroup*> colGroups;
		int score;
		Text* scoreTxt;

		int CB = 0;

		AvgRect* songPosOutline;
		AvgRect* songPosBar;

		float heldEnd[4];

		float positionInSong = 0;
		std::string returnAcc();
		Text* songLengthText;

		Text* positionAndBeats;

		bool downscroll;



		void cleanUp();
		
		Channel* song;
		Channel* clap;

		static Texture* noteskin;
		static Texture* specialNoteskin;
		Gameplay();

		void initControls();

		void removeNote(NoteObject* object);
		void createJudgement(judgement judge);
		void updateAccuracy(double hitWorth);
		void removePhase() override;
		void miss(NoteObject* object);
		void update(Events::updateEvent event) override;
		void keyDown(SDL_KeyboardEvent event) override;
		void keyUp(SDL_KeyboardEvent ev) override;
};

