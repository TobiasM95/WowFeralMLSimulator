#pragma once

#include <string>
#include <vector>

class Player;

class Skill
{
public:
	Player* player;
	//NOTE: energy cost and cp requirement has to be static for rdy check and therefore in child
	bool ignore_armor;

	Skill
	(
		Player& player,
		bool ignore_armor
	);

	virtual float process_instant_effects() = 0;
};

class Dot : public Skill
{
public:
	std::string name;
	float max_duration;
	float duration_left;
	float tick_every;
	bool can_pandemic;
	float pandemic_window;
	float tick_timer = 0.0f;
	bool stealth_snapshotted = false;
	bool tigers_fury_snapshotted = false;
	bool bloodtalons_snapshotted = false;

	Dot
	(
		Player& player,
		bool ignore_armor,
		std::string name,
		float max_duration, 
		float tick_every,
		bool can_pandemic,
		float pandemic_window
	);

	virtual float process_instant_effects() = 0;
	virtual float process_dot_tick() = 0;
	bool tick(float time_delta);
};