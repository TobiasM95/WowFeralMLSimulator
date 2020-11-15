#pragma once

#include <string>
#include <memory>
#include <list>

class Skill;
class Dot;
class Simulator;

class Target
{
private:
	float debuff_multiplier = 1.05f;
	int armor = 452;
	//TODO_SL: Update k_value for shadowlands
	float k_value = 860.0f * 1.402f; //Search in simc for k_value for info on that
public:
	Target(Simulator* simulator);

	Simulator* simulator;
	float received_dmg = 0;
	float fight_duration = 0;
	std::list<std::shared_ptr<Dot>> active_dots;

	void reset();
	void resolve(Skill& s);
	void resolve(std::shared_ptr<Dot> s);
	void tick(float time_delta);
	bool has_dot(std::string name);
	void apply_dot(std::shared_ptr<Dot> s);
	std::shared_ptr<Dot> get_dot(std::string name);
};