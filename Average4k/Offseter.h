#pragma once
#include "Menu.h"
#include "includes.h"
#include "Character.h"
#include "Text.h"
class Offseter :
    public Menu
{
public:
    Offseter() {}

    std::vector<Text*> offsets;

    Character* c;
    Character* onion;

    int animIndex = 0;

    bool shift = false;
    SDL_Keycode key;

    void updateOffsetText();
    void keyDown(SDL_KeyboardEvent event) override;
    void keyUp(SDL_KeyboardEvent event) override;
    virtual void create();
    void update(Events::updateEvent event) override;
};

