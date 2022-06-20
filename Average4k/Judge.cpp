#include "Judge.h"


std::vector<double> Judge::hitWindows;

std::vector<int> Judge::scoreWindows;

void Judge::initJudge()
{
	// in ms btw!
	hitWindows = {45.0, 90.0, 135.0, 160};
	// score
	scoreWindows = {350, 200, 0, -300};
}

int Judge::scoreNote(float difff)
{
	float diff = std::abs(difff);

	for (int i = 0; i < 4; i++)
	{
		double window = hitWindows[i];
		double nextWindow = 0;
		if (i != 0)
			nextWindow = hitWindows[i - 1];

		if (diff >= nextWindow && diff <= window)
			return scoreWindows[i];
	}
	return -1;
}

judgement Judge::judgeNote(float difference) {
	float diff = std::abs(difference);

	for (int i = 0; i < 4; i++)
	{
		double window = hitWindows[i];
		double nextWindow = 0;
		if (i != 0)
			nextWindow = hitWindows[i - 1];
		
		if (diff >= nextWindow && diff <= window)
			return (judgement)i;
	}
	
	return (judgement)-1;
}