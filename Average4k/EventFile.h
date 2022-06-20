#pragma once
#include "includes.h"
#include "Reader.h"

struct Event {
	std::string func;
	float length;
	float beat;
	std::string args;
	std::string name;
};

class EventFile {
private:
	Reader* read;

public:
	std::vector<Event> events;
	EventFile(std::string path)
	{
		read = new Reader(path);

		for (valueType t : read->getAllOfValue("createEvent"))
		{
			Event e;
			e.beat = std::stof(read->getValue("eventBeat",t.value));
			e.func = read->getValue("eventFunction", t.value);
			if (read->getValue("eventLength", t.value) == "inf")
				e.length = -1;
			else
				e.length = std::stof(read->getValue("eventLength", t.value));

			if (read->getValue("eventArgs", t.value) != "")
				e.args = read->getValue("eventArgs", t.value);

			e.name = t.value;

			events.push_back(e);
		}
	}


	~EventFile()
	{
		events.clear();
		delete read;
	}
};