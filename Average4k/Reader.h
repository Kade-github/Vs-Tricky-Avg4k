#pragma once
#include "includes.h"
struct readVec2 {
	double x, y;
};

struct valueType {
	std::string name, key, value;
};

class Reader {
private: // no communism today
	std::vector <std::string> lines;
public:
	Reader(std::string path)
	{
		std::ifstream testFile(path + ".avgData");
		std::string line;

		while (getline(testFile, line)) {
			lines.push_back(line);
		}
	}

	readVec2 convertVec(std::string vecString)
	{
		readVec2 v;
		v.x = std::stod(vecString.substr(0, vecString.find(",")));
		v.y = std::stod(vecString.substr(vecString.find(",") + 1, vecString.size()));
		return v;
	}

	std::vector<valueType> getAllOfValue(std::string search)
	{
		std::vector<valueType> types;
		for (std::string s : lines)
		{
			int hash = s.find("#");
			if (hash != -1 || s == "")
				continue;
			std::string name = s.substr(0, s.find(":")); // before ":"
			std::string value = s.substr(s.find(":") + 2); // after ": "

			if (s.find("[") != -1 )
			{
				name = s.substr(0, s.find("[")); // before "["
				int first = s.find("[") + 1;
				int second = s.find("]");
				std::string k = s.substr(first, second-first); // between "[]"
				if (name == search)
				{
					valueType type;
					type.key = k;
					type.name = name;
					type.value = value;
					types.push_back(type);
				}
			}
			else
			{
				if (name == search)
				{
					valueType type;
					type.key = "";
					type.name = name;
					type.value = value;
					types.push_back(type);
				}
			}
		}
		return types;
	}



	std::string getValue(std::string search, std::string key)
	{
		for (std::string s : lines)
		{
			if (s.find("#") != -1 || s == "")
				continue;
			// example:
			// p1: boyfriend
			// sprites[background]: assets/graphical/tricky/nevadaBG.png

			std::string name = s.substr(0, s.find(":")); // before ":"
			std::string value = s.substr(s.find(":") + 2); // after ": "

			if (s.find("[") != -1 && key != "")
			{
				name = s.substr(0, s.find("[")); // before "["
				int first = s.find("[")+ 1;
				int second = s.find("]");
				std::string k = s.substr(first, second-first); // between "[]"
				if (k == key && name == search)
					return value;
			}
			else if (name == search)
			{
				return value;
			}
		}
		return "";
	}
};