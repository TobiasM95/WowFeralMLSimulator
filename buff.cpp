#include "buff.h"

//TODO: Implement buff features, see notepad++
Buff::Buff
(
	std::string name,
	BuffCategory category,
	int init_stacks,
	int max_stacks,
	float init_duration,
	float max_duration,
	bool can_pandemic,
	float pandemic_window
) :
	name(name),
	category(category),
	stacks(init_stacks),
	max_stacks(max_stacks),
	duration(init_duration),
	max_duration(max_duration),
	can_pandemic(can_pandemic),
	pandemic_window(pandemic_window)
{
}

Buff::Buff
(
	std::string name,
	BuffCategory category,
	int init_stacks,
	int max_stacks,
	float max_duration,
	bool can_pandemic,
	float pandemic_window
) :
	Buff(
		name, 
		category,
		init_stacks, 
		max_stacks, 
		max_duration, 
		max_duration, 
		can_pandemic, 
		pandemic_window
	)
{
}

float Buff::update_duration(float time_delta)
{
	duration -= time_delta;
	return duration;
}

void Buff::add_duration(float duration)
{
	//TODO: Test if this works as a oneliner
	this->duration = std::min(
		max_duration + duration,
		max_duration * (1.0f + can_pandemic * pandemic_window)
	);
}

void Buff::add_stack(int stacks)
{
	this->stacks = std::min(this->stacks + stacks, this->max_stacks);
}

bool Buff::remove_stack(int stacks)
{
	this->stacks -= stacks;
	return this->stacks <= 0;
}