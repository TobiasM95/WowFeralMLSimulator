#pragma once

#include <string>
#include <memory>
#include <list>

class Skill;
class Dot;

class Target
{
private:
	float received_dmg = 0;
	float fight_duration = 0;
	float dps = 0;
	float debuff_multiplier = 1.05f;
	int armor = 471; //TODO: Read the actual value from simc html reports
	//TODO: Update k_value for shadowlands
	float k_value = 860.0f * 1.402f; //Search in simc for k_value for info on that
public:
	std::list<std::shared_ptr<Dot>> active_dots;

	void resolve(Skill& s);
	void resolve(std::shared_ptr<Dot> s);
	void tick(float time_delta);
	bool has_dot(std::string name);
};