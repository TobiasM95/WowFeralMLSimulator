#pragma once

#include <string>

enum class BuffCategory {SkillBuff, StatBuff};

class Buff
{
public:
	//int values for stat buffs (rating)
	//float values for resulting stat buff(final value)
	//special effect buff(struct for buff name)
	std::string name;
	BuffCategory category;
	int stacks = 1;
	int max_stacks;
	float duration;
	float max_duration;
	bool can_pandemic;
	float pandemic_window;

	Buff(std::string name, BuffCategory category, int max_stacks, float max_duration, 
		bool can_pandemic, float pandemic_window = 1.3f);
	float update_duration(float time_delta);
private:
};