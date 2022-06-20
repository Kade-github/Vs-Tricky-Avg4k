#include "Noteskin.h"
#include "Game.h"
#include <windows.h>
#include <SDL_image.h>
#include "Helpers.h"

std::string Noteskin::type = "arrow";

Texture* Noteskin::asset = NULL;
Texture* Noteskin::specialAsset = NULL;

Texture* getAsset(std::string path)
{
	// stbi cannot find these for some reason??
	SDL_Surface* s = IMG_Load(path.c_str());
	Texture* texture = Texture::createFromSurface(s, true);

	return texture;
}

void Noteskin::resetNoteskin(Texture* as)
{
	delete as;
}


bool convertStringBool(std::string text) {
	return text == "false" ? false : true;
}

Texture* Noteskin::getNoteskin()
{
	if (asset)
		delete asset;
	asset = Texture::createWithImage("assets/graphical/fnfBase/Arrows.png");

	return asset;
}

Texture* Noteskin::getSpecialNoteskin()
{
	if (specialAsset)
		delete specialAsset;
	specialAsset = Texture::createWithImage("assets/graphical/tricky/SpecialityNotes.png");

	return specialAsset;
}

