#include "SaveFile.h"

template <typename T>
bool contains(std::vector<T> vec, const T& elem)
{
    bool result = false;
    if (find(vec.begin(), vec.end(), elem) != vec.end())
    {
        result = true;
    }
    return result;
}

SaveFile::SaveFile()
{
    // default settings
    defaultSettings.push_back(CreateSetting(false, 0, "downscroll", true, 0,1, false, 0, ""));
    defaultSettings.push_back(CreateSetting(true, 810, "scrollspeed", false, 1,5000, false, 1, ""));
    defaultSettings.push_back(CreateSetting(true, -45, "offset", false,-1000,1000,false, 1, ""));
    defaultSettings.push_back(CreateSetting(false, 0, "hitsounds", true,0,1,false, 0, ""));
    defaultSettings.push_back(CreateSetting(true, 1, "Note Size", false, 0.9, 1.5, false, 0.01, ""));
    defaultSettings.push_back(CreateSetting(false, SDLK_d, "Left Key", true, 0, 1, true, 0, ""));
    defaultSettings.push_back(CreateSetting(false, SDLK_f, "Down Key", true, 0, 1, true,0, ""));
    defaultSettings.push_back(CreateSetting(false, SDLK_j, "Up Key", true, 0, 1, true,0, ""));
    defaultSettings.push_back(CreateSetting(false, SDLK_k, "Right Key", true, 0, 1, true,0, ""));
    std::ifstream ifs("settings.avg");
    if (!ifs.good())
    {
        CreateNewFile();
        return;
    }
   
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());
    upd.get().convert(settings);

    // check for new settings
    if (settings.size() != defaultSettings.size())
    {
        for (int i = 0; i < defaultSettings.size(); i++)
        {
            if (i > settings.size() - 1)
            {
                settings.push_back(defaultSettings[i]);
                std::cout << "user didn't have " << defaultSettings[i].name << std::endl;
            }
        }
        Save();
    }

    // check for defaults/max/min's
    for (int i = 0; i < settings.size(); i++)
    {
        setting& set = settings[i];
        setting& dSet = defaultSettings[i];
        if (set.highestValue != dSet.highestValue)
            set.highestValue = dSet.highestValue;
        if (set.lowestValue != dSet.lowestValue)
            set.lowestValue = dSet.lowestValue;
        if (set.takesActive != dSet.takesActive)
            set.takesActive == dSet.takesActive;
        if (set.isKeybind != dSet.isKeybind)
            set.isKeybind == dSet.isKeybind;
    }

}

void SaveFile::CreateNewFile()
{
    for (setting& set : defaultSettings)
        settings.push_back(set);
    Save();
}

void SaveFile::Save()
{
    std::ofstream of("settings.avg");

    std::stringstream bitch;

    msgpack::pack(bitch, settings);

    of << bitch.str();

    of.close();
}

void SaveFile::SetString(std::string sett, std::string value)
{
    int size = value.size();
    if (size > 248)
        size = 248;
    for (setting& set : settings)
    {
        if (set.name == sett)
        {
            memset(set.stringValue, 0, 248);
            memcpy_s(set.stringValue, size, value.c_str(), size);
        }
    }
}

void SaveFile::SetDouble(std::string sett, double value)
{
    for (setting& set : settings)
    {
        if (set.name == sett)
            set.value = value;
    }
}

void SaveFile::SetBool(std::string sett, bool value)
{
    for (setting& set : settings)
    {
        if (set.name == sett)
            set.active = value;
    }
}

std::string SaveFile::GetString(std::string sett)
{
    for (setting& set : settings)
    {
        if (set.name == sett)
            return set.stringValue;
    }
    return "";
}

double SaveFile::GetDouble(std::string sett)
{
    for (setting& set : settings)
    {
        if (set.name == sett)
            return set.value;
    }
    return 0;
}

bool SaveFile::GetBool(std::string sett)
{
    for (setting& set : settings)
    {
        if (set.name == sett)
            return set.active;
    }
    return false;
}

setting SaveFile::CreateSetting(bool defaultActive, double defaultValue, std::string defaultName, bool tA, double lowest, double highest, bool isKeybind, double increm, std::string defaultStringValue)
{
    setting set;
    set.active = defaultActive;
    set.takesActive = tA;
    set.value = defaultValue;
    set.highestValue = highest;
    set.lowestValue = lowest;
    set.isKeybind = isKeybind;
    set.increm = increm;
    memcpy_s(set.stringValue, 248, defaultStringValue.c_str(), 248);
    memcpy_s(set.name, 128, defaultName.c_str(), 128);
    return set;
}
