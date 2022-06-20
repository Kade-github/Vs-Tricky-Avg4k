#pragma once
#include "includes.h"
enum class judgement {
	Judge_sick = 0,
	Judge_good = 1,
	Judge_bad = 2,
	Judge_shit = 3
};

class Judge
{

	public:
		static std::vector<double> hitWindows;
		static std::vector<int> scoreWindows;
		static void initJudge();

		static judgement judgeNote(float difff);
		static int scoreNote(float diff);
};

