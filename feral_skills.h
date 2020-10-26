#pragma once

#include "skill.h"

class AutoAttack : public Skill
{
public:
	AutoAttack
	(
		Player& player
	); 
	
	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	float process_instant_effects() override;
};

class Shred : public Skill
{
public:
	Shred
	(
		Player& player
	);

	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	float process_instant_effects() override;
};

class Rake : public Dot
{
public:
	Rake
	(
		Player& player
	);


	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	float process_instant_effects();
	float process_dot_tick();
};
