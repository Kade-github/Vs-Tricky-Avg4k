#include "Offseter.h"
#include <Windows.h>
#include <shobjidl.h> 
#include <atlstr.h>
#include "MainMenu.h"


std::string utf8_encode(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

void Offseter::updateOffsetText()
{
    for (Text* t : offsets)
        removeObj(t);
    offsets.clear();

    int index = 0;
    for (std::map<std::string, readVec2>::iterator ii = c->offsets.begin(); ii != c->offsets.end(); ++ii)
    {
        Text* of = new Text(4, 64 + (24 * index), ii->first + ": " + std::to_string((int)ii->second.x) + "," + std::to_string((int)ii->second.y), 14, "vcr");
        index++;
        offsets.push_back(of);
        if (c->sparrow->currentAnim == ii->first)
        {
            of->color = { 128,128,255 };
        }
        of->staticView = true;
        add(of);
    }
}



void Offseter::keyDown(SDL_KeyboardEvent event)
{
    if (event.keysym.sym == SDLK_ESCAPE)
        Game::instance->transitionToMenu(new MainMenu());
    key = event.keysym.sym;
    if (event.keysym.mod & KMOD_LCTRL)
    {
        if (key == SDLK_s)
        {
            std::string stuff = "";
            float nScale = 1 / c->offsetScale;
            for (std::map<std::string, readVec2>::iterator ii = c->offsets.begin(); ii != c->offsets.end(); ++ii)
            {
                stuff += "animOffset[" + c->setAnimsReverse[ii->first] + "]: " + std::to_string((int)((ii->second.x * nScale))) + "," + std::to_string((int)((ii->second.y * nScale))) + "\n";
            }
            OpenClipboard(0);
            EmptyClipboard();
            HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, stuff.size());
            if (!hg) {
                CloseClipboard();
                return;
            }
            memcpy(GlobalLock(hg), stuff.c_str(), stuff.size());
            GlobalUnlock(hg);
            SetClipboardData(CF_TEXT, hg);
            CloseClipboard();
            GlobalFree(hg);
        }
    }
    if (event.keysym.mod & KMOD_LSHIFT)
        shift = true;
    else
    {
        if (key == SDLK_LEFT)
            c->offsets[c->sparrow->currentAnim].x += 1;
        if (key == SDLK_RIGHT)
            c->offsets[c->sparrow->currentAnim].x -= 1;

        if (key == SDLK_DOWN)
            c->offsets[c->sparrow->currentAnim].y -= 1;
        if (key == SDLK_UP)
            c->offsets[c->sparrow->currentAnim].y += 1;

        if (key == SDLK_1)
        {
            animIndex++;
            if (animIndex > c->offsets.size() - 1)
                animIndex = 0;
            auto it = c->offsets.begin();
            std::advance(it, animIndex);
            c->play(it->first, true);
        }

        if (key == SDLK_2)
        {
            animIndex--;
            if (animIndex < 0)
                animIndex = c->offsets.size() - 1;
            auto it = c->offsets.begin();
            std::advance(it, animIndex);
            c->play(it->first, true);
        }


        c->offsetX = c->offsets[c->sparrow->currentAnim].x;
        c->offsetY = c->offsets[c->sparrow->currentAnim].y;
        updateOffsetText();
    }

}

void Offseter::keyUp(SDL_KeyboardEvent event)
{
    if (event.keysym.sym == SDLK_LSHIFT)
        shift = false;
    else
        key = SDL_FLIP_NONE;
}

void Offseter::create()
{
    addCamera(Game::mainCamera);

    AvgRect* rect = new AvgRect(0, 0, 1280, 720);
    rect->c = { 25,25,25 };
    rect->staticView = true;
    add(rect);

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);

    Text* offset = new Text(4, 40, "Offseting: N/A", 14, "vcr");
    add(offset);
    offset->staticView = true;

	IFileOpenDialog* pFileOpen;

	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
		IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    COMDLG_FILTERSPEC fileTypes[] =
    {
        { L"All supported files", L"*.avgData" },
    };

    pFileOpen->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);

	hr = pFileOpen->Show(NULL);

    if (SUCCEEDED(hr))
    {
        IShellItem* pItem;
        hr = pFileOpen->GetResult(&pItem);
        if (SUCCEEDED(hr))
        {
            PWSTR pszFilePath;
            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

            if (SUCCEEDED(hr))
            {
                std::string path = utf8_encode(pszFilePath);
                path = path.substr(0, path.size() - 8);
                c = new Character(100,100, path);
                c->play("idle");
                onion = new Character(100, 100, path, c->tex);
                onion->dontDeleteTex = true;
                onion->play("idle");
                add(onion);
                add(c);
                onion->alpha = 0.7;
                offset->setText("Offseting: " + path + ".avgData | PRESS CTRL S TO SAVE TO CLIPBOARD");
                updateOffsetText();
                CoTaskMemFree(pszFilePath);
            }
            pItem->Release();
        }
    }
    pFileOpen->Release();
    CoUninitialize();
    created = true;
}

void Offseter::update(Events::updateEvent event)
{
    if (shift)
    {
        if (key == SDLK_LEFT)
            Game::mainView->x -= 2;
        if (key == SDLK_RIGHT)
            Game::mainView->x += 2;

        if (key == SDLK_DOWN)
            Game::mainView->y += 2;
        if (key == SDLK_UP)
            Game::mainView->y -= 2;
    }
}
