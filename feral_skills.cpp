#include "feral_skills.h"

#include <vector>

#include "player.h"
#include "buff.h"
#include "feral_buffs.h"
#include "random_gen.h"


AutoAttack::AutoAttack
(
	Player& player
) :
	Skill("autoattack", player, false)
{
	
}
const float AutoAttack::base_energy_cost = 0;
const bool AutoAttack::need_combopoints = false;

bool AutoAttack::check_rdy(Player& player)
{
	return player.autoattack_timer <= 0;
}

std::pair<float, bool> AutoAttack::process_instant_effects()
{
	//NOTE: Autohit works like currently implemented, a autohit cooldown gets set to weapon speed
	//and decreased per elapsed time, therefore a non cat autoattack can be performed 1s after last
	//cat form autoattack and cat form autoattack can be performed wep_speed s after last attack

	player->autoattack_timer = player->cat_form? 1.0f: player->get_wep_speed();
	player->autoattack_timer /= 1.0f + player->get_buffed_haste();

	//check if buff procced
	double ooc_rng = rng_namespace::getChance();
	bool has_moc = player->has_talent("moment_of_clarity");
	float moc_multiplier = 1.0f + 0.5f * has_moc;
	if (ooc_rng < moc_multiplier * 7.0 / 60.0f && player->ooc_icd < 0) {
		player->ooc_icd = 1.05f;
		if (has_moc)
		{
			MomentOfClarity moc;
			player->proc_buff(moc, 1, true);
		}
		else
		{
			OmenOfClarity ooc;
			player->proc_buff(ooc, 1, true);
		}
	}

	//return damage, check for crit
	double crit_rng = rng_namespace::getChance();
	float crit_multiplier = (float)(crit_rng < player->get_buffed_crit()) + 1.0f;
	float damage_multiplier = 1.0f;
	damage_multiplier *= 1 + 0.4f * player->cat_form;
	damage_multiplier *= 1 + 0.15f * player->has_buff("tigers_fury");
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	damage_multiplier *= 1 + player->get_buffed_versatility();
	float res_damage;
	if (player->cat_form)
	{
		res_damage = damage_multiplier * crit_multiplier * player->get_buffed_attack_power() / 6.0f;
	}
	else
	{
		res_damage = damage_multiplier * crit_multiplier * (player->get_wep_dps() + (player->get_wep_speed() * player->get_buffed_attack_power()) / 6.0f);
	}
	return std::pair<float, bool> {res_damage, crit_multiplier > 1.0f};
}

//Generators

Shred::Shred(Player& player)
	:
	Skill("shred", player, false)
{
}
const float Shred::base_energy_cost = 40;
const bool Shred::need_combopoints = false;

bool Shred::check_rdy(Player& player)
{
	float res_energy_cost = (1 - (player.has_buff("omen_of_clarity")
		|| player.has_buff("moment_of_clarity"))) 
		* (1 - 0.2f * player.has_buff("incarnation")) * base_energy_cost;
	return player.gcd_ready() && player.energy >= res_energy_cost && player.cat_form;
}

std::pair<float, bool> Shred::process_instant_effects()
{
	bool has_stealth_benefits = player->stealth || player->has_buff("berserk");
	int combopoint_generation = 1;
	float res_energy_cost = base_energy_cost;
	//Note: Feral aura makes spell hit 20% harder
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	//Note: no moonfire dot since it doesnt count as bleed
	damage_multiplier *= (1 + (player->target->has_dot("dot_rake") || 
		player->target->has_dot("dot_rip") || 
		player->target->has_dot("dot_thrash")) * 0.2f);
	if (has_stealth_benefits)
	{
		damage_multiplier *= 1.6f;
		combopoint_generation += 1;
		//Note: This is shadowlands
		//combopoint_generation += 2;
	}
	damage_multiplier *= (1 + 0.15f * player->has_buff("tigers_fury"));
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	//resolve buff consumption
	std::list<Buff>::iterator clarity_iterator = player->get_skill_buff("omen_of_clarity");
	if (clarity_iterator != player->skill_buffs.end())
	{
		player->consume_buff(clarity_iterator, "skill_buffs");
		res_energy_cost = 0.0f;
	}
	clarity_iterator = player->get_skill_buff("moment_of_clarity");
	if (clarity_iterator != player->skill_buffs.end())
	{
		player->consume_buff(clarity_iterator, "skill_buffs");
		res_energy_cost = 0.0f;
		damage_multiplier *= 1.15f;
	}
	//start gcd, subtract energy(if no ooc / moc), etc.
	player->start_gcd();
	res_energy_cost *= (1 - 0.2f * player->has_buff("incarnation"));
	player->energy -= res_energy_cost;
	//process damage effects etc (like crits bonuses)
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = false;
	if (crit_rng < player->get_buffed_crit() * (has_stealth_benefits?2.0f:1.0f))
	{
		has_crit = true;
		damage_multiplier *= 2;
		combopoint_generation += 1;
	}

	player->add_combopoints(combopoint_generation);
	player->tslu_shred = 0.0f;

	return std::pair<float, bool> {damage_multiplier* (player->get_buffed_attack_power() * 0.46f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

Rake::Rake(Player& player)
	:
	Dot("rake_dot", player, true, 15.0f, 3.0f, true, 0.3f)
{
}
const float Rake::base_energy_cost = 35;
const bool Rake::need_combopoints = false;

bool Rake::check_rdy(Player& player)
{
	float res_energy_cost = (1 - 0.2f * player.has_buff("incarnation")) * base_energy_cost;
	return player.gcd_ready() && player.energy >= res_energy_cost && player.cat_form;
}

std::pair<float, bool> Rake::process_instant_effects()
{
	//set snapshot bools or
	stealth_snapshotted = player->stealth || player->has_buff("berserk");
	tigers_fury_snapshotted = player->has_buff("tigers_fury");
	//stealth benefits
	int combopoint_generation = 1;
	float res_energy_cost = base_energy_cost;
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	//do base skill modifiers (e.g. shred deals 20% more damage to bleeding targets)
	if (stealth_snapshotted)
	{
		damage_multiplier *= 1.6f;
		combopoint_generation += 1;
	}
	damage_multiplier *= (1 + 0.15f * tigers_fury_snapshotted);
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	//check for buffs and process each existing buff (call player.has_buff for each buff)
	//resolve buff consumption
	std::list<Buff>::iterator clarity_iterator = player->get_skill_buff("omen_of_clarity");
	if (clarity_iterator != player->skill_buffs.end())
	{
		player->consume_buff(clarity_iterator, "skill_buffs");
		res_energy_cost = 0.0f;
	}
	clarity_iterator = player->get_skill_buff("moment_of_clarity");
	if (clarity_iterator != player->skill_buffs.end())
	{
		player->consume_buff(clarity_iterator, "skill_buffs");
		res_energy_cost = 0.0f;
		damage_multiplier *= 1.15f;
	}
	//start gcd, subtract energy (if no ooc/moc), etc.
	player->start_gcd();
	res_energy_cost *= (1 - 0.2f * player->has_buff("incarnation"));
	player->energy -= res_energy_cost;
	//proc buffs
	//process damage effects etc (like crits)
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = false;
	if (crit_rng < player->get_buffed_crit())
	{
		has_crit = true;
		damage_multiplier *= 2;
		combopoint_generation += 1;
	}
	damage_multiplier *= (1 + player->get_buffed_mastery());

	player->add_combopoints(combopoint_generation);
	player->tslu_rake = 0.0f;
	//return dmg
	return std::pair<float, bool> {damage_multiplier* (player->get_buffed_attack_power() * 0.18225f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

std::pair<float, bool> Rake::process_dot_tick()
{
	float dot_ratio = std::min(1.0f, tick_timer / tick_every);
	tick_timer -= tick_every;
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	damage_multiplier *= (1 + 0.15f * tigers_fury_snapshotted);
	damage_multiplier *= (1 + 0.6f * stealth_snapshotted);
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = (crit_rng < player->get_buffed_crit());
	damage_multiplier *= (1 + 1 * has_crit);
	damage_multiplier *= (1 + player->get_buffed_mastery());
	return std::pair<float, bool> {dot_ratio* damage_multiplier* (player->get_buffed_attack_power() * 0.15561f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

Thrash::Thrash(Player& player)
	:
	Dot("thrash_dot", player, true, 15.0f, 3.0f, true, 0.3f)
{
}
const float Thrash::base_energy_cost = 40;
const bool Thrash::need_combopoints = false;

bool Thrash::check_rdy(Player& player)
{
	float res_energy_cost = (1 - (player.has_buff("omen_of_clarity")
		|| player.has_buff("moment_of_clarity")))
		* (1 - 0.2f * player.has_buff("incarnation")) * base_energy_cost;
	return player.gcd_ready() && player.energy >= res_energy_cost && player.cat_form;
}

std::pair<float, bool> Thrash::process_instant_effects()
{
	//set snapshot bools or
	stealth_snapshotted = player->stealth || player->has_buff("berserk");
	tigers_fury_snapshotted = player->has_buff("tigers_fury");
	moment_of_clarity_snapshot = player->has_talent("moment_of_clarity");
	//stealth benefits
	int combopoint_generation = 1;
	float res_energy_cost = base_energy_cost;
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	//do base skill modifiers (e.g. shred deals 20% more damage to bleeding targets)
	damage_multiplier *= (1 + 0.15f * tigers_fury_snapshotted);
	damage_multiplier *= (1 + 0.15f * moment_of_clarity_snapshot);
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	//check for buffs and process each existing buff (call player.has_buff for each buff)
	//resolve buff consumption
	std::list<Buff>::iterator clarity_iterator = player->get_skill_buff("omen_of_clarity");
	if (clarity_iterator != player->skill_buffs.end())
	{
		player->consume_buff(clarity_iterator, "skill_buffs");
		res_energy_cost = 0.0f;
	}
	clarity_iterator = player->get_skill_buff("moment_of_clarity");
	if (clarity_iterator != player->skill_buffs.end())
	{
		player->consume_buff(clarity_iterator, "skill_buffs");
		res_energy_cost = 0.0f;
		damage_multiplier *= 1.15f;
	}
	//start gcd, subtract energy (if no ooc/moc), etc.
	player->start_gcd();
	res_energy_cost *= (1 - 0.2f * player->has_buff("incarnation"));
	player->energy -= res_energy_cost;
	//proc buffs
	//process damage effects etc (like crits)
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = false;
	if (crit_rng < player->get_buffed_crit())
	{
		has_crit = true;
		damage_multiplier *= 2;
		combopoint_generation += 1;
	}
	damage_multiplier *= (1 + player->get_buffed_mastery());

	player->add_combopoints(combopoint_generation);
	player->tslu_thrash = 0.0f;
	//return dmg
	return std::pair<float, bool> {damage_multiplier* (player->get_buffed_attack_power() * 0.055f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

std::pair<float, bool> Thrash::process_dot_tick()
{
	float dot_ratio = std::min(1.0f, tick_timer / tick_every);
	tick_timer -= tick_every;
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	damage_multiplier *= (1 + 0.15f * tigers_fury_snapshotted);
	damage_multiplier *= (1 + 0.15f * moment_of_clarity_snapshot);
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = (crit_rng < player->get_buffed_crit());
	damage_multiplier *= (1 + 1 * has_crit);
	damage_multiplier *= (1 + player->get_buffed_mastery());
	return std::pair<float, bool> {dot_ratio* damage_multiplier* (player->get_buffed_attack_power() * 0.035f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

Brutal_Slash::Brutal_Slash(Player& player)
	:
	Skill("brutal_slash", player, false)
{
}
const float Brutal_Slash::base_energy_cost = 25;
const bool Brutal_Slash::need_combopoints = false;

bool Brutal_Slash::check_rdy(Player& player)
{
	if (!player.has_talent("brutal_slash"))
		return false;
	float res_energy_cost = (1 - (player.has_buff("omen_of_clarity")
		|| player.has_buff("moment_of_clarity")))
		* (1 - 0.2f * player.has_buff("incarnation")) * base_energy_cost;
	return player.gcd_ready() && player.energy >= res_energy_cost && player.cat_form;
}

std::pair<float, bool> Brutal_Slash::process_instant_effects()
{
	int combopoint_generation = 1;
	float res_energy_cost = base_energy_cost;
	//Note: Feral aura makes spell hit 20% harder
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	damage_multiplier *= (1 + 0.15f * player->has_buff("tigers_fury"));
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	//resolve buff consumption
	std::list<Buff>::iterator clarity_iterator = player->get_skill_buff("omen_of_clarity");
	if (clarity_iterator != player->skill_buffs.end())
	{
		player->consume_buff(clarity_iterator, "skill_buffs");
		res_energy_cost = 0.0f;
	}
	clarity_iterator = player->get_skill_buff("moment_of_clarity");
	if (clarity_iterator != player->skill_buffs.end())
	{
		player->consume_buff(clarity_iterator, "skill_buffs");
		res_energy_cost = 0.0f;
		damage_multiplier *= 1.15f;
	}
	//start gcd, subtract energy(if no ooc / moc), etc.
	player->start_gcd();
	res_energy_cost *= (1 - 0.2f * player->has_buff("incarnation"));
	player->energy -= res_energy_cost;
	//process damage effects etc (like crits bonuses)
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = false;
	if (crit_rng < player->get_buffed_crit())
	{
		has_crit = true;
		damage_multiplier *= 2;
		combopoint_generation += 1;
	}

	player->add_combopoints(combopoint_generation); 
	player->use_charge("brutal_slash");
	player->tslu_brutal_slash = 0.0f;

	return std::pair<float, bool> {damage_multiplier* (player->get_buffed_attack_power() * 0.69f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

//Note: 8s-2*haste dot duration, 2s ticks is very janky way of doing FF, apparently its true (empirically)
//NOTE: Apaprently, neither init dmg nor tick dmg ignores armor, check this, calculations not always correct
//Weird damage calcs: 0.15*5*3*(agi+wep_dps*6)*vers*mastery*feral_aura*haste matches tooltip
//Without haste and the *3 this matches dummy damage if armor is taken into account for one equip set
//but only closely (deviation of 4%) for other gear sets, let this stay for now
Feral_Frenzy::Feral_Frenzy(Player& player)
	:
	Dot("feral_frenzy_dot", player, false, 8.0f - 2.0f * player.get_buffed_haste(), 2.0f, true, 0.3f)
{
}
const float Feral_Frenzy::base_energy_cost = 25;
const bool Feral_Frenzy::need_combopoints = false;

bool Feral_Frenzy::check_rdy(Player& player)
{
	if (!player.has_talent("feral_frenzy"))
		return false;
	float res_energy_cost = (1 - 0.2f * player.has_buff("incarnation")) * base_energy_cost;
	return player.gcd_ready() && player.energy >= res_energy_cost && player.cat_form
		&& player.charge_count("feral_frenzy");
}

std::pair<float, bool> Feral_Frenzy::process_instant_effects()
{
	//set snapshot bools or
	tigers_fury_snapshotted = player->has_buff("tigers_fury");
	//stealth benefits
	int combopoint_generation = 5;
	float res_energy_cost = base_energy_cost;
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	//do base skill modifiers (e.g. shred deals 20% more damage to bleeding targets)
	damage_multiplier *= (1 + 0.15f * tigers_fury_snapshotted);
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	//check for buffs and process each existing buff (call player.has_buff for each buff)
	//start gcd, subtract energy (if no ooc/moc), etc.
	player->start_gcd();
	res_energy_cost *= (1 - 0.2f * player->has_buff("incarnation"));
	player->energy -= res_energy_cost;
	//proc buffs
	//process damage effects etc (like crits)
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = false;
	if (crit_rng < player->get_buffed_crit())
	{
		has_crit = true;
		damage_multiplier *= 2;
	}
	damage_multiplier *= (1 + player->get_buffed_mastery());

	player->add_combopoints(combopoint_generation); 
	player->use_charge("feral_frenzy");
	//Note: Apparently, for feral frenzy additive damage is applied before versatility
	return std::pair<float, bool> {damage_multiplier* (player->get_buffed_attack_power() * 0.375f + additive_dmg)* (1 + player->get_buffed_versatility()), has_crit};
}

std::pair<float, bool> Feral_Frenzy::process_dot_tick()
{
	float dot_ratio = std::min(1.0f, tick_timer / tick_every);
	tick_timer -= tick_every;
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	damage_multiplier *= (1 + 0.15f * tigers_fury_snapshotted);
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = (crit_rng < player->get_buffed_crit());
	damage_multiplier *= (1 + 1 * has_crit);
	damage_multiplier *= (1 + player->get_buffed_mastery());
	//Note: See at constructor, this calc does not always give correct damage
	return std::pair<float, bool> {dot_ratio* damage_multiplier* (player->get_buffed_attack_power() * 0.75f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

Moonfire_Feral::Moonfire_Feral(Player& player)
	:
	Dot("moonfire_feral_dot", player, true, 16.0f, 2.0f, true, 0.3f)
{
}
const float Moonfire_Feral::base_energy_cost = 30;
const bool Moonfire_Feral::need_combopoints = false;

bool Moonfire_Feral::check_rdy(Player& player)
{
	if (!player.has_talent("lunar_inspiration"))
		return false;
	float res_energy_cost = (1 - 0.2f * player.has_buff("incarnation")) * base_energy_cost;
	return player.gcd_ready() && player.energy >= res_energy_cost && player.cat_form;
}

std::pair<float, bool> Moonfire_Feral::process_instant_effects()
{
	//set snapshot bools or
	tigers_fury_snapshotted = player->has_buff("tigers_fury");
	//stealth benefits
	int combopoint_generation = 1;
	float res_energy_cost = base_energy_cost;
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	//do base skill modifiers (e.g. shred deals 20% more damage to bleeding targets)
	damage_multiplier *= (1 + 0.15f * tigers_fury_snapshotted);
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	//start gcd, subtract energy (if no ooc/moc), etc.
	player->start_gcd();
	res_energy_cost *= (1 - 0.2f * player->has_buff("incarnation"));
	player->energy -= res_energy_cost;
	//proc buffs
	//process damage effects etc (like crits)
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = false;
	if (crit_rng < player->get_buffed_crit())
	{
		has_crit = true;
		damage_multiplier *= 2;
		combopoint_generation += 1;
	}
	//Moonfire not affected by mastery
	//damage_multiplier *= (1 + player->get_buffed_mastery());

	player->add_combopoints(combopoint_generation);
	//return dmg
	return std::pair<float, bool> {damage_multiplier* (player->get_buffed_attack_power() * 0.15f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

std::pair<float, bool> Moonfire_Feral::process_dot_tick()
{
	float dot_ratio = std::min(1.0f, tick_timer / tick_every);
	tick_timer -= tick_every;
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	damage_multiplier *= (1 + 0.15f * tigers_fury_snapshotted);
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = (crit_rng < player->get_buffed_crit());
	damage_multiplier *= (1 + 1 * has_crit);
	return std::pair<float, bool> {dot_ratio* damage_multiplier* (player->get_buffed_attack_power() * 0.15f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

Sunfire::Sunfire(Player& player)
	:
	Dot("sunfire_dot", player, true, 12.0f, 2.0f, true, 0.3f)
{
}
const float Sunfire::base_energy_cost = 0;
const bool Sunfire::need_combopoints = false;

bool Sunfire::check_rdy(Player& player)
{
	if (!player.has_talent("balance_affinity"))
		return false;
	return player.gcd_ready() && !player.cat_form;
}

std::pair<float, bool> Sunfire::process_instant_effects()
{
	int additive_dmg = 0;
	//start gcd, subtract energy (if no ooc/moc), etc.
	//sunfire is a regular caster spell so gcd is 1.5f*haste
	player->start_gcd(1.5f / (1.0f + player->get_buffed_haste()));
	//this is empirically tested to be the ap to sp conversion rate
	float damage_multiplier = 0.963057f;
	if (player->has_buff("heart_of_the_wild"))
	{
		damage_multiplier *= 1.3f;
	}
	//process damage effects etc (like crits)
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = false;
	if (crit_rng < player->get_buffed_crit())
	{
		has_crit = true;
		damage_multiplier *= 2;
	}

	//return dmg
	return std::pair<float, bool> {damage_multiplier* (player->get_buffed_attack_power() * 0.20f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

std::pair<float, bool> Sunfire::process_dot_tick()
{
	float dot_ratio = std::min(1.0f, tick_timer / tick_every);
	tick_timer -= tick_every;
	int additive_dmg = 0;
	//this is empirically tested to be the ap to sp conversion rate
	float damage_multiplier = 0.963957f;
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = (crit_rng < player->get_buffed_crit());
	damage_multiplier *= (1 + 1 * has_crit);
	return std::pair<float, bool> {dot_ratio* damage_multiplier* (player->get_buffed_attack_power() * 0.174f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

Starsurge::Starsurge(Player& player)
	:
	Skill("starsurge", player, true)
{
}
const float Starsurge::base_energy_cost = 0;
const bool Starsurge::need_combopoints = false;

bool Starsurge::check_rdy(Player& player)
{
	if (!player.has_talent("balance_affinity"))
		return false;
	return player.gcd_ready() && !player.cat_form && player.charge_count("starsurge");
}

std::pair<float, bool> Starsurge::process_instant_effects()
{
	int additive_dmg = 0;
	//this is empirically tested to be the ap to sp conversion rate
	float damage_multiplier = 0.963057f;
	//start gcd, subtract energy (if no ooc/moc), etc.
	//starsurge is a casted spell, instead of cast system just take cast time as gcd time
	if (player->has_buff("heart_of_the_wild"))
	{
		damage_multiplier *= 1.3f;
		player->start_gcd(1.5f / (1.0f + player->get_buffed_haste()));
	}
	else
		player->start_gcd(2.0f / (1.0f + player->get_buffed_haste()));
	//process damage effects etc (like crits)
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = false;
	if (crit_rng < player->get_buffed_crit())
	{
		has_crit = true;
		damage_multiplier *= 2;
	}
	player->use_charge("starsurge");
	//return dmg
	return std::pair<float, bool> {damage_multiplier* (player->get_buffed_attack_power() * 1.62f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

//Finishers
Rip::Rip(Player& player)
	:
	Dot("rip_dot", player, true, 4.0f * (player.combopoints + 1), 24.0f, 2.0f, true, 0.3f)
{
}
const float Rip::base_energy_cost = 20;
const bool Rip::need_combopoints = true;

bool Rip::check_rdy(Player& player)
{
	float res_energy_cost = (1 - 0.2f * player.has_buff("incarnation")) * base_energy_cost;
	return player.gcd_ready() && player.energy >= res_energy_cost && player.cat_form
		&& player.combopoints > 0;
}

std::pair<float, bool> Rip::process_instant_effects()
{
	//set snapshot bools or
	stealth_snapshotted = player->stealth || player->has_buff("berserk");
	tigers_fury_snapshotted = player->has_buff("tigers_fury");
	//stealth benefits
	float res_energy_cost = base_energy_cost;
	int additive_dmg = 0;
	//check for buffs and process each existing buff (call player.has_buff for each buff
	//resolve buff consumption
	std::list<Buff>::iterator bt_iterator = player->get_skill_buff("bloodtalons");
	if (bt_iterator != player->skill_buffs.end())
	{
		player->consume_buff(bt_iterator, "skill_buffs");
		bloodtalons_snapshotted = true;
	}
	//start gcd, subtract energy (if no ooc/moc), etc.
	player->start_gcd();
	res_energy_cost *= (1 - 0.2f * player->has_buff("incarnation"));
	player->energy -= res_energy_cost;
	if (player->has_talent("soul_of_the_forest"))
		player->energy = std::min(player->max_energy, player->energy + 5 * player->combopoints);
	player->combopoints = 0;
	return std::pair<float, bool> {0.0f, false};
}

std::pair<float, bool> Rip::process_dot_tick()
{
	float dot_ratio = std::min(1.0f, tick_timer / tick_every);
	tick_timer -= tick_every;
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	damage_multiplier *= (1 + 0.15f * tigers_fury_snapshotted);
	damage_multiplier *= (1 + 0.30f * bloodtalons_snapshotted);
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	damage_multiplier *= (1 + 0.05f * player->has_talent("soul_of_the_forest"));
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = (crit_rng < player->get_buffed_crit());
	damage_multiplier *= (1 + 1 * has_crit);
	damage_multiplier *= (1 + player->get_buffed_mastery());
	return std::pair<float, bool> {dot_ratio* damage_multiplier* (player->get_buffed_attack_power() * 0.14f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

Ferocious_Bite::Ferocious_Bite(Player& player)
	:
	Skill("ferocious_bite", player, false)
{
}
const float Ferocious_Bite::base_energy_cost = 25;
const bool Ferocious_Bite::need_combopoints = false;

bool Ferocious_Bite::check_rdy(Player& player)
{
	float res_energy_cost = (1 - 0.2f * player.has_buff("incarnation")) * base_energy_cost;
	return player.gcd_ready() && player.energy >= res_energy_cost && player.cat_form
		&& player.combopoints > 0;
}

std::pair<float, bool> Ferocious_Bite::process_instant_effects()
{
	float res_energy_cost = base_energy_cost;
	//Note: Feral aura makes spell hit 20% harder
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	damage_multiplier *= (1 + 0.15f * player->has_buff("tigers_fury"));
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	damage_multiplier *= (1 + 0.15f * player->has_talent("soul_of_the_forest"));
	damage_multiplier *= (player->combopoints / 5.0f);
	//start gcd, subtract energy(if no ooc / moc), etc.
	player->start_gcd();
	res_energy_cost = std::min(50.0f, player->energy);
	damage_multiplier *= (1 + (res_energy_cost-25.0f) / 25.0f);
	res_energy_cost *= (1 - 0.2f * player->has_buff("incarnation"));
	player->energy -= res_energy_cost;
	if (player->has_talent("soul_of_the_forest"))
		player->energy = std::min(player->max_energy, player->energy + 5 * player->combopoints);

	//process damage effects etc (like crits bonuses)
	float crit_rng = (float)rng_namespace::getChance();
	bool has_crit = false;
	if (crit_rng < player->get_buffed_crit())
	{
		has_crit = true;
		damage_multiplier *= 2;
	}
	damage_multiplier *= (1 + player->get_buffed_mastery());

	if (player->has_talent("sabertooth"))
	{
		std::shared_ptr<Dot> rip_dot = player->target->get_dot("rip_dot");
		if (rip_dot)
			rip_dot->add_duration(player->combopoints * 1.0f);
	}

	player->combopoints = 0;

	//TODO_SL: Find out why FB has this 2% multiplier weirdness that doesn't match the tooltip
	return std::pair<float, bool> {0.98f * damage_multiplier * (player->get_buffed_attack_power() * 0.9828f * (1 + player->get_buffed_versatility()) + additive_dmg), has_crit};
}

Savage_Roar::Savage_Roar(Player& player)
	:
	Skill("savage_roar", player, false)
{
}
const float Savage_Roar::base_energy_cost = 25;
const bool Savage_Roar::need_combopoints = true;

bool Savage_Roar::check_rdy(Player& player)
{
	if (!player.has_talent("savage_roar"))
		return false;
	float res_energy_cost = (1 - 0.2f * player.has_buff("incarnation")) * base_energy_cost;
	return player.gcd_ready() && player.energy >= res_energy_cost && player.cat_form
		&& player.combopoints > 0;
}

std::pair<float, bool> Savage_Roar::process_instant_effects()
{
	float res_energy_cost = base_energy_cost;
	Savage_Roar_Buff srb {6.0f * (player->combopoints + 1)};
	player->proc_buff(srb, 1, true);
	//start gcd, subtract energy(if no ooc / moc), etc.
	player->start_gcd();
	res_energy_cost *= (1 - 0.2f * player->has_buff("incarnation"));
	player->energy -= res_energy_cost;
	if (player->has_talent("soul_of_the_forest"))
		player->energy = std::min(player->max_energy, player->energy + 5 * player->combopoints);
	player->combopoints = 0;

	return std::pair<float, bool> {0.0f, false};
}

//Cooldowns
Tigers_Fury::Tigers_Fury(Player& player)
	:
	Skill("tigers_fury", player, false)
{
}
const float Tigers_Fury::base_energy_cost = 0;
const bool Tigers_Fury::need_combopoints = false;

bool Tigers_Fury::check_rdy(Player& player)
{
	return player.charge_count("tigers_fury");
}

std::pair<float, bool> Tigers_Fury::process_instant_effects()
{
	player->cat_form = true;
	player->energy = std::min(player->max_energy, player->energy + 50);
	Tigers_Fury_Buff tfb = {player->has_talent("predator")};
	player->proc_buff(tfb, 1, true); 
	player->use_charge("tigers_fury");
	return std::pair<float, bool> {0.0f, false};
}

Berserk::Berserk(Player& player)
	:
	Skill("berserk", player, false)
{
}
const float Berserk::base_energy_cost = 0;
const bool Berserk::need_combopoints = false;

bool Berserk::check_rdy(Player& player)
{
	return player.charge_count("berserk");
}

std::pair<float, bool> Berserk::process_instant_effects()
{
	player->cat_form = true;
	if (player->has_talent("incarnation"))
	{
		Berserk_Buff bb = { true };
		player->proc_buff(bb, 1, true);
		Incarnation_Buff ib = { };
		player->proc_buff(ib, 1, true);
	}
	else
	{
		Berserk_Buff bb = { false };
		player->proc_buff(bb, 1, true);
	}
	player->use_charge("berserk");
	return std::pair<float, bool> {0.0f, false};
}

Heart_Of_The_Wild::Heart_Of_The_Wild(Player& player)
	:
	Skill("heart_of_the_wild", player, false)
{
}
const float Heart_Of_The_Wild::base_energy_cost = 0;
const bool Heart_Of_The_Wild::need_combopoints = false;

bool Heart_Of_The_Wild::check_rdy(Player& player)
{
	return player.gcd_ready() && player.charge_count("heart_of_the_wild");
}

std::pair<float, bool> Heart_Of_The_Wild::process_instant_effects()
{
	Heart_Of_The_Wild_Buff bb = { };
	player->proc_buff(bb, 1, true);
	player->use_charge("heart_of_the_wild");
	return std::pair<float, bool> {0.0f, false};
}

//Misc
Cat_Form::Cat_Form(Player& player)
	:
	Skill("cat_form", player, false)
{
}
const float Cat_Form::base_energy_cost = 0;
const bool Cat_Form::need_combopoints = false;

bool Cat_Form::check_rdy(Player& player)
{
	return player.gcd_ready() && !player.cat_form;
}

std::pair<float, bool> Cat_Form::process_instant_effects()
{
	player->start_gcd(1.5f / (1.0f + player->get_buffed_haste()));
	player->cat_form = true;
	return std::pair<float, bool> {0.0f, false};
}

Moonkin_Form::Moonkin_Form(Player& player)
	:
	Skill("moonkin_form", player, false)
{
}
const float Moonkin_Form::base_energy_cost = 0;
const bool Moonkin_Form::need_combopoints = false;

bool Moonkin_Form::check_rdy(Player& player)
{
	return player.gcd_ready() && player.cat_form;
}

std::pair<float, bool> Moonkin_Form::process_instant_effects()
{
	player->start_gcd(1.5f / (1.0f + player->get_buffed_haste()));
	player->cat_form = false;
	return std::pair<float, bool> {0.0f, false};
}