#include "buff.h"

//TODO: Implement buff features, see notepad++

float Buff::update_duration(float time_delta)
{
	duration -= time_delta;
	return duration;
}