#include "NoteObject.h"

#include "Gameplay.h"

#include <mutex>

#include "Judge.h"

#include "SongSelect.h"


void NoteObject::draw() {
    Gameplay* instance = (Gameplay*)Game::currentMenu;

    float position = instance->positionInSong;

    Rect receptor;

    AvgSprite* obj = instance->receptors[lane];

    receptor.x = obj->x;
    receptor.y = obj->y;
    receptor.w = obj->w;
    receptor.h = obj->h;

    bpmSegment bruh = SongSelect::currentChart->getSegmentFromBeat(beat);

    float wh = SongSelect::currentChart->getTimeFromBeat(beat, bruh);

    float diff = (wh)-(position);

    float bps = (Game::save->GetDouble("scrollspeed") / 60) / Gameplay::rate;

    float noteOffset = (bps * (diff / 1000)) * (104 * Game::save->GetDouble("Note Size"));

    bool downscroll = Game::save->GetBool("downscroll");

    if (downscroll)
        rect.y = (receptor.y - noteOffset);
    else
        rect.y = (receptor.y + noteOffset);


    // get quant
    // haha get the FULL version of average4k at https://store.steampowered.com/app/1828580/Average4k/

    /*float beatRow = (beat - stopOffset) * 48;

    if (fmod(beatRow, (192 / 4)) == 0)
        texture = Gameplay::noteskin->fourth;
    else if (fmod(beatRow, (192 / 8)) == 0)
        texture = Gameplay::noteskin->eighth;
    else if (fmod(beatRow, (192 / 12)) == 0)
        texture = Gameplay::noteskin->twelfth;
    else if (fmod(beatRow, (192 / 16)) == 0)
        texture = Gameplay::noteskin->sixteenth;
    else if (fmod(beatRow, (192 / 32)) == 0)
        texture = Gameplay::noteskin->thirty2nd;
    else
        texture = Gameplay::noteskin->none;*/

    Rect clipThingy;

    clipThingy.x = 0;
    clipThingy.y = rect.y + 52;
    clipThingy.w = 104 * Game::save->GetDouble("Note Size");
    clipThingy.h = holdHeight;
    if (downscroll) {
        clipThingy.y -= holdHeight;
    }

    Rect dstRect;
    Rect srcRect;

    dstRect.r = 255;
    dstRect.g = 255;
    dstRect.b = 255;
    dstRect.a = 1;

    x = receptor.x;
    y = rect.y;
    
    Rect clipRectCopy = clipRect;

    if (holdsActive > 2 && !mine && !death)
        for (int i = 0; i < heldTilings.size(); i++) {
            holdTile& tile = heldTilings[i];
            float time = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat));

            float diff2 = time - position;

            Rect nDstRect;
            nDstRect.x = receptor.x;

            float offsetFromY = (bps * (diff2 / 1000)) * (104 * Game::save->GetDouble("Note Size"));
            tile.rect.y = receptor.y + offsetFromY;
            if (downscroll)
                tile.rect.y = receptor.y - offsetFromY;
            AvgFrame holdFrame;
            std::string suffix = " hold";
            if (i == heldTilings.size() - 1)
                suffix = " tail";
            switch (lane % 4)
            {
            case 0:
                    holdFrame = sparrow->getRectFromFrame("purple" + suffix, 0);
                break;
            case 1:
                    holdFrame = sparrow->getRectFromFrame("blue" + suffix, 0);
                break;
            case 2:
                    holdFrame = sparrow->getRectFromFrame("green" + suffix, 0);
                break;
            case 3:
                    holdFrame = sparrow->getRectFromFrame("red" + suffix, 0);
                break;
            }
            nDstRect.r = 255;
            nDstRect.g = 255;
            nDstRect.b = 255;

            nDstRect.x = (receptor.x + 34);
            nDstRect.h = 65;
            nDstRect.w = 36;
            nDstRect.a = 0.6;

            srcRect = holdFrame.srcRect;

            nDstRect.y = tile.rect.y;

            if (tile.rect.y < receptor.y - 63 && !tile.active && !downscroll)
                continue;

            if (tile.rect.y > receptor.y + 63 && !tile.active && downscroll)
                continue;


            if (clipRectCopy.w > 0 || clipRectCopy.h > 0)
                Rendering::SetClipRect(&clipRectCopy);
            if (i != heldTilings.size() - 1) {
                if (!downscroll)
                {
                    //srcRect.h = -1;
                    Rendering::PushQuad(&nDstRect, &srcRect, Gameplay::noteskin, GL::genShader);
                    //srcRect.h = 1;
                }
                else
                {
                    Rendering::PushQuad(&nDstRect, &srcRect, Gameplay::noteskin, GL::genShader);
                }
            }
            else {
                nDstRect.h = 32;
                if (!downscroll)
                {

                    //srcRect.h = -1;
                    Rendering::PushQuad(&nDstRect, &srcRect, Gameplay::noteskin, GL::genShader);
                    //srcRect.h = 1;
                }
                else
                {
                    nDstRect.y += 52;
                    Rendering::PushQuad(&nDstRect, &srcRect, Gameplay::noteskin, GL::genShader, 90);
                }
            }
            if (clipRectCopy.w > 0 || clipRectCopy.h > 0)
                Rendering::SetClipRect(NULL);
        }

    dstRect.h = rect.h;

    dstRect.y = rect.y;


   

    int activeH = 0;

    for (int i = 0; i < heldTilings.size(); i++) {
        holdTile& tile = heldTilings[i];
        float time = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat));

        float diff2 = time - position;
        bool condition = diff2 > -Judge::hitWindows[3];

        if (heldTilings[i].active || condition)
            activeH++;
    }
    if (activeH != holdsActive)
        holdsActive = activeH;

    dstRect.x = receptor.x;
    dstRect.y = y;

    dstRect.w = w * scale;
    dstRect.h = h * scale;

    dstRect.r = 255;
    dstRect.g = 255;
    dstRect.b = 255;
    dstRect.a = alpha;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = 1;
    srcRect.h = 1;

    if (!animationFinished || loop)
    {
        animTime += Game::deltaTime;
        frame = (animTime * fps / 1000);
        int size = sparrow->animations[sparrow->currentAnim].frames.size();
        if (frame > size - 1)
        {
            if (!loop)
                animationFinished = true;
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
    dstRect.x = ((x - offsetX) + ((fr.frameRect.x * scale * Game::mainView->zoom)));
    dstRect.y = (y + ((fr.frameRect.y * scale * Game::mainView->zoom)));

    dstRect.w = (fr.frameRect.w * scale);
    dstRect.h = (fr.frameRect.h * scale);
    if (overrideW != 0)
        dstRect.w = overrideW * scale;
    if (overrideH != 0)
        dstRect.h = overrideH * scale;
   


    if (!staticView)
    {
        dstRect.x -= Game::mainView->x * scroll;
        dstRect.y -= Game::mainView->y * scroll;
        dstRect.w *= Game::mainView->zoom;
        dstRect.h *= Game::mainView->zoom;
    }

    if (active)
    {

        Rendering::PushQuad(&dstRect, &srcRect, tex, GL::genShader);

    }

    if (clipRect.w > 0 || clipRect.h > 0)
        Rendering::SetClipRect(NULL);

}