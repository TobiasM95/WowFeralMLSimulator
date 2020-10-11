#include "buff.h"

//TODO: Implement buff features, see notepad++
Buff::Buff
(
	std::string name, 
	BuffCategory category, 
	int max_stacks, 
	float max_duration,
	bool can_pandemic,
	float pandemic_window
) :
	name(name), 
	category(category), 
	max_stacks(max_stacks), 
	max_duration(max_duration),
	can_pandemic(can_pandemic),
	pandemic_window(pandemic_window)
{
	duration = max_duration;
}

float Buff::update_duration(float time_delta)
{
	duration -= time_delta;
	return duration;
}