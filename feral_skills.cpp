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
	Skill(player, false)
{
	
}
const float AutoAttack::base_energy_cost = 0;
const bool AutoAttack::need_combopoints = false;

bool AutoAttack::check_rdy(Player& player)
{
	return player.autoattack_timer > player.base_auto_attack_speed;
}

float AutoAttack::process_instant_effects()
{
	//TODO: Check how auto attack cooldown gets handled when switching stances and when casting spells
	//NOTE: Autohit works like currently implemented, a autohit cooldown gets set to weapon speed
	//and decreased per elapsed time, therefore a non cat autoattack can be performed 1s after last
	//cat form autoattack and cat form autoattack can be performed wep_speed s after last attack
	//TODO: Implement non catform autoattack with wep speed etc

	player->autoattack_timer = 0.0f;

	//check if buff procced
	//TODO: Check if 1.5 multiplier for MoC is correct
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
	//TODO: Implement miss values
	double crit_rng = rng_namespace::getChance();
	float crit_multiplier = (float)(crit_rng < player->get_buffed_crit()) + 1.0f;
	float damage_multiplier = 1.0f;
	damage_multiplier *= 1 + 0.4f * player->has_buff("cat_form");
	damage_multiplier *= 1 + 0.15f * player->has_buff("tigers_fury");
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	damage_multiplier *= 1 + player->get_buffed_versatility();
	return damage_multiplier * crit_multiplier * player->get_buffed_attack_power() / 6.0f;
}

Shred::Shred(Player& player)
	:
	Skill(player, false)
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

float Shred::process_instant_effects()
{
	bool has_stealth_benefits = player->stealth || player->has_buff("berserk");
	int combopoint_generation = 1;
	float res_energy_cost = base_energy_cost;
	//Note: Feral aura makes spell hit 20% harder
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	//Note: no moonfire dot since it doesnt count as bleed
	damage_multiplier *= (1 + (player->target.has_dot("dot_rake") || 
		player->target.has_dot("dot_rip") || 
		player->target.has_dot("dot_thrash")) * 0.2f);
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
	if (crit_rng < player->get_buffed_crit() * (has_stealth_benefits?2.0f:1.0f))
	{
		damage_multiplier *= 2;
	}


	return damage_multiplier * (player->get_buffed_attack_power() * 0.46f * (1 + player->get_buffed_versatility()) + additive_dmg);
}

Rake::Rake(Player& player)
	:
	Dot(player, false, "rake_dot", 15.0f, 3.0f, true, 0.3f)
{
}
const float Rake::base_energy_cost = 35;
const bool Rake::need_combopoints = false;

bool Rake::check_rdy(Player& player)
{
	float res_energy_cost = (1 - 0.2f * player.has_buff("incarnation")) * base_energy_cost;
	return player.gcd_ready() && player.energy >= res_energy_cost && player.cat_form;
}

float Rake::process_instant_effects()
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
	//start gcd, subtract energy (if no ooc/moc), etc.
	player->start_gcd();
	res_energy_cost *= (1 - 0.2f * player->has_buff("incarnation"));
	player->energy -= res_energy_cost;
	//proc buffs
	//process damage effects etc (like crits)
	float crit_rng = (float)rng_namespace::getChance();
	damage_multiplier *= (1 + 1 * (crit_rng < player->get_buffed_crit()));
	damage_multiplier *= (1 + player->get_buffed_mastery());
	//return dmg
	return damage_multiplier * (player->get_buffed_attack_power() * 0.18225f * (1 + player->get_buffed_versatility()) + additive_dmg);
}

float Rake::process_dot_tick()
{
	float dot_ratio = std::min(1.0f, tick_timer / tick_every);
	tick_timer -= tick_every;
	int additive_dmg = 0;
	float damage_multiplier = player->feral_aura_buff;
	damage_multiplier *= (1 + 0.15f * tigers_fury_snapshotted);
	damage_multiplier *= (1 + 0.6f * stealth_snapshotted);
	damage_multiplier *= (1 + 0.15f * player->has_buff("savage_roar"));
	float crit_rng = (float)rng_namespace::getChance();
	damage_multiplier *= (1 + 1 * (crit_rng < player->get_buffed_crit()));
	damage_multiplier *= (1 + player->get_buffed_mastery());
	return dot_ratio * damage_multiplier * (player->get_buffed_attack_power() * 0.15561f * (1 + player->get_buffed_versatility()) + additive_dmg);
}

