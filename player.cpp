#include <vector>

#include "player.h"
#include "feral_skills.h"
#include "target.h"
#include "buff.h"

Player::Player(float wep_dps, int agility, float crit, float haste, float mastery, 
	float versatility) :
	wep_dps(wep_dps), agility(agility), crit(crit), haste(haste), mastery(mastery), 
	versatility(versatility)
{


}

void Player::tick(float time_delta, Target* t)
{
	autoattack_timer += time_delta;
	gcd_timer = std::max(gcd_timer - time_delta, 0.0f);
	update_buffs_and_stats(time_delta);

	//TODO: Energy update with buffed haste

	if (autoattack_timer > base_auto_attack_speed) 
	{
		autoattack_timer = 0.0f;
		AutoAttack a(this);
		t->resolve(a);
	}
	
	//TODO: Perform action
}

void Player::update_buffs_and_stats(float time_delta) 
{
	//Update buff durations and delete expired buffs
	for (std::list<Buff*>::iterator i = stat_buffs.begin(); i != stat_buffs.end();) {
		if ((*i)->update_duration(time_delta) <= 0)
			i = stat_buffs.erase(i);
		else
			++i;
	}
	//TODO: Update stats finish
}