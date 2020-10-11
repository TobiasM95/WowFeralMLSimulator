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
	Skill(player, {}, {}, {})
{
	
}

//TODO: Rename to compute_instant_effects that returns dmg (or something similar)
float AutoAttack::calc_instant_dmg()
{
	//TODO: Implement stuff below
	//check for buffs in given list (loop through vector and implement player function check_buff?)
	//account for all buffs
	//resolve buff consumption

	//check if buff procced
	//TODO: implement moment of clarity talent (as buff?)
	double ooc_rng = rng_namespace::getChance();
	if (ooc_rng < 7.0 / 60.0f && player.ooc_icd < 0) {
		player.ooc_icd = 1.05f;
		OmenOfClarity ooc;
		player.proc_buff(ooc);
	}

	//return damage, check for crit
	//TODO: Implement miss values
	double crit_rng = rng_namespace::getChance();
	float crit_multiplier = (float)(crit_rng < player.buffed_crit) + 1.0f;
	return crit_multiplier * (player.buffed_attack_power / 6.0f + player.get_wep_dps());
}

/*
float Dot::tick(float time_delta)
{
	duration_left -= time_delta;
	//check for buffs (haste could speed up the tick)
	//check if dot ticks for damage
	//calculate tick dmg
	//return tick dmg
	return 0;
}*/