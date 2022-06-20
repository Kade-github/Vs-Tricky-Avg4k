#pragma once 

class Easing {
	public:
	enum easing_functions
	{
			EaseInSine,
			EaseOutSine,
			EaseInOutSine,
			EaseInQuad,
			EaseOutQuad,
			EaseInOutQuad,
			EaseInCubic,
			EaseOutCubic,
			EaseInOutCubic,
			EaseInQuart,
			EaseOutQuart,
			EaseInOutQuart,
			EaseInQuint,
			EaseOutQuint,
			EaseInOutQuint,
			EaseInExpo,
			EaseOutExpo,
			EaseInOutExpo,
			EaseInCirc,
			EaseOutCirc,
			EaseInOutCirc,
			EaseInBack,
			EaseOutBack,
			EaseInOutBack,
			EaseInElastic,
			EaseOutElastic,
			EaseInOutElastic,
			EaseInBounce,
			EaseOutBounce,
			EaseInOutBounce
	};

	typedef double(*easingFunction)(double);

	static easingFunction getEasingFunction(easing_functions function);
};
