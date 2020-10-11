#include "player.h"
#include "feral_skills.h"
#include "target.h"
#include "buff.h"

#include <iostream>

Player::Player(float wep_dps, int agility, float crit, float haste, float mastery, 
	float versatility, std::array<int, 7> talents) :
	wep_dps(wep_dps), agility(agility), crit(crit), haste(haste), mastery(mastery), 
	versatility(versatility), talents(talents)
{
	//TODO: Cast buffs that represent talent choices

}

void Player::tick(float time_delta, Target &t)
{
	autoattack_timer += time_delta;
	gcd_timer = std::max(gcd_timer - time_delta, 0.0f);
	update_buffs_and_stats(time_delta);
	
	//DEBUG
	std::cout << "Number of buffs: " << skill_buffs.size() + stat_buffs.size() << "\n";
	for (Buff b : skill_buffs) {
		std::cout << b.name << " " << b.duration << "/" << b.max_duration
			<< "  " << b.stacks << "/" << b.max_stacks << "\n";
	}
	for (Buff b : stat_buffs) {
		std::cout << b.name << " " << b.duration << "/" << b.max_duration
			<< "  " << b.stacks << "/" << b.max_stacks << "\n";
	}
	//

	//TODO: Energy update with buffed haste

	if (autoattack_timer > base_auto_attack_speed) 
	{
		autoattack_timer = 0.0f;
		AutoAttack a(*this);
		t.resolve(a);
	}
	
	//TODO: Perform action
}

void Player::update_buffs_and_stats(float time_delta) 
{
	//Update buff durations and delete expired buffs
	for (std::list<Buff>::iterator i = stat_buffs.begin(); i != stat_buffs.end();) {
		if (i->update_duration(time_delta) <= 0)
			i = stat_buffs.erase(i);
		else
			++i;
	}
	for (std::list<Buff>::iterator i = skill_buffs.begin(); i != skill_buffs.end();) {
		if (i->update_duration(time_delta) <= 0)
			i = skill_buffs.erase(i);
		else
			++i;
	}
	//decrement ooc internal cooldown
	ooc_icd -= time_delta;
	//TODO: Update stats finish, for now, return base values
	buffed_attack_power = agility;
	buffed_crit = crit;
	buffed_haste = haste;
	buffed_versatility = versatility;
	buffed_mastery = mastery;
	buffed_energy = base_energy;
}

void Player::proc_buff(Buff& buff) 
{
	Buff *existing_buff = nullptr;
	for (Buff &b : skill_buffs)
	{
		if (b.name == buff.name)
			existing_buff = &b;
	}
	for (Buff &b : stat_buffs)
	{
		if (b.name == buff.name)
			existing_buff = &b;
	}
	if (existing_buff && existing_buff != NULL)
	{
		existing_buff->stacks = std::min(existing_buff->stacks + 1, existing_buff->max_stacks);
		if (existing_buff->can_pandemic)
		{
			existing_buff->duration = std::min(
				existing_buff->duration + existing_buff->max_duration,
				existing_buff->max_duration * existing_buff->pandemic_window
				);
		}
		else
			existing_buff->duration = existing_buff->max_duration;
	}
	else 
	{
		if (buff.category == BuffCategory::SkillBuff)
			skill_buffs.push_back(buff);
		else
			stat_buffs.push_back(buff);
	}
}