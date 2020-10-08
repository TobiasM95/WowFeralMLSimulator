#pragma once

class Skill;

class Target
{
private:
	float received_dmg = 1;
	float fight_duration = 0;
	float dps = 0;
public:
	void resolve(Skill& s);
	void tick(float time_delta);
};