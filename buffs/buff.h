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
	int stacks;
	int max_stacks;
	float duration;
	float max_duration;
	bool can_pandemic;
	float pandemic_window;

	Buff(std::string name, BuffCategory category, int init_stacks, int max_stacks, float max_duration,
		bool can_pandemic, float pandemic_window = 0.3f);
	Buff(std::string name, BuffCategory category, int init_stacks, int max_stacks, 
		float init_duration, float max_duration, bool can_pandemic, float pandemic_window = 0.3f);
	float update_duration(float time_delta);
	void add_duration(float duration);
	void add_stack(int stacks);
	bool remove_stack(int stacks);

	bool operator==(const std::string name) 
	{
		return this->name == name;
	}
	bool operator!=(const std::string name)
	{
		return !(*this == name);
	}
private:
};