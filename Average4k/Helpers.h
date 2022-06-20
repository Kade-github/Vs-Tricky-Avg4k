#pragma once
class Helpers {
public:
	static char asciitolower(char in) {
		if (in <= 'Z' && in >= 'A')
			return in - ('Z' - 'z');
		return in;
	}

	// ripped straight from https://github.com/HaxeFlixel/flixel/blob/27960e3b66c8ebad0052973831960761971168fd/flixel/math/FlxVelocity.hx
	static float computeVelocity(float Velocity, float Acceleration, float Drag, float Max, float Elapsed)
	{
		if (Acceleration != 0)
		{
			Velocity += Acceleration * Elapsed;
		}
		else if (Drag != 0)
		{
			float drag = Drag * Elapsed;
			if (Velocity - drag > 0)
			{
				Velocity -= drag;
			}
			else if (Velocity + drag < 0)
			{
				Velocity += drag;
			}
			else
			{
				Velocity = 0;
			}
		}
		if ((Velocity != 0) && (Max != 0))
		{
			if (Velocity > Max)
			{
				Velocity = Max;
			}
			else if (Velocity < -Max)
			{
				Velocity = -Max;
			}
		}
		return Velocity;
	}

};