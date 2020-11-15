#pragma once

#include <string>
#include <vector>

class Player;

class Skill
{
public:
	std::string name;
	Player* player;
	//NOTE: energy cost and cp requirement has to be static for rdy check and therefore in child
	bool ignore_armor;

	Skill
	(
		std::string name,
		Player& player,
		bool ignore_armor
	);

	virtual std::pair<float, bool> process_instant_effects() = 0;
};

class Dot : public Skill
{
public:
	float duration_left;
	float max_duration;
	float tick_every;
	bool can_pandemic;
	float pandemic_window;
	float tick_timer = 0.0f;
	bool stealth_snapshotted = false;
	bool tigers_fury_snapshotted = false;
	bool bloodtalons_snapshotted = false;
	bool moment_of_clarity_snapshot = false;

	Dot
	(
		std::string name,
		Player& player,
		bool ignore_armor,
		float max_duration, 
		float tick_every,
		bool can_pandemic,
		float pandemic_window
	);

	Dot
	(
		std::string name,
		Player& player,
		bool ignore_armor,
		float duration_left,
		float max_duration,
		float tick_every,
		bool can_pandemic,
		float pandemic_window
	);

	virtual std::pair<float, bool> process_instant_effects() = 0;
	virtual std::pair<float, bool> process_dot_tick() = 0;
	bool tick(float time_delta);
	void add_duration(float duration);
};