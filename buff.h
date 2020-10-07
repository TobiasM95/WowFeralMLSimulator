#pragma once

class Buff
{
public:
	float update_duration(float time_delta);
private:
	int stacks;
	float duration;
	//int values for stat buffs (rating)
	//float values for resulting stat buff(final value)
	//special effect buff(struct for buff name)
};