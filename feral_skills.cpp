#include "feral_skills.h"
#include "player.h"

AutoAttack::AutoAttack
(
	Player* player
) :
	Skill(player, {}, {}, {})
{
	
}


float AutoAttack::calc_instant_dmg()
{
	//TODO: Implement stuff below
	//check for buffs in given list (loop through vector and implement player function check_buff?)
	//account for all buffs
	//resolve buff consumption
	//check if buff procced
	//return damage
	return (*player).get_buffed_attack_power() / 6 + (*player).get_wep_dps();
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