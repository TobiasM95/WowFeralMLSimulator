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
	std::pair<float, bool> process_instant_effects() override;
};

//Generators
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
	std::pair<float, bool> process_instant_effects() override;
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
	std::pair<float, bool> process_instant_effects();
	std::pair<float, bool> process_dot_tick();
};

class Thrash : public Dot
{
public:
	Thrash
	(
		Player& player
	);

	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects();
	std::pair<float, bool> process_dot_tick();
};

class Brutal_Slash : public Skill
{
public:
	Brutal_Slash
	(
		Player& player
	);

	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects() override;
};

class Feral_Frenzy : public Dot
{
public:
	Feral_Frenzy
	(
		Player& player
	);


	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects();
	std::pair<float, bool> process_dot_tick();
};

class Moonfire_Feral : public Dot
{
public:
	Moonfire_Feral
	(
		Player& player
	);


	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects();
	std::pair<float, bool> process_dot_tick();
};

class Sunfire : public Dot
{
public:
	Sunfire
	(
		Player& player
	);


	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects();
	std::pair<float, bool> process_dot_tick();
};

class Starsurge : public Skill
{
public:
	Starsurge
	(
		Player& player
	);

	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects() override;
};

//Finishers
class Rip : public Dot
{
public:
	Rip
	(
		Player& player
	);


	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects();
	std::pair<float, bool> process_dot_tick();
};

class Ferocious_Bite : public Skill
{
public:
	Ferocious_Bite
	(
		Player& player
	);

	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects() override;
};

class Savage_Roar : public Skill
{
public:
	Savage_Roar
	(
		Player& player
	);

	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects() override;
};

//Cooldowns
class Tigers_Fury : public Skill
{
public:
	Tigers_Fury
	(
		Player& player
	);

	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects() override;
};

class Berserk : public Skill
{
public:
	Berserk
	(
		Player& player
	);

	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects() override;
};

class Heart_Of_The_Wild : public Skill
{
public:
	Heart_Of_The_Wild
	(
		Player& player
	);

	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects() override;
};

//Misc

class Cat_Form : public Skill
{
public:
	Cat_Form
	(
		Player& player
	);

	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects() override;
};

class Moonkin_Form : public Skill
{
public:
	Moonkin_Form
	(
		Player& player
	);

	static const float base_energy_cost;
	static const bool need_combopoints;

	static bool check_rdy(Player& player);
	std::pair<float, bool> process_instant_effects() override;
};