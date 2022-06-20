#pragma once
#include "includes.h"
#include "Texture.h"

class Noteskin
{
	public:

		static std::string type;
		static Texture* asset;
		static Texture* specialAsset;
		static void resetNoteskin(Texture* as);
		static Texture* getNoteskin();
		static Texture* getSpecialNoteskin();
};

