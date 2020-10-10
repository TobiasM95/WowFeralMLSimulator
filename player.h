#pragma once
#include <list>
#include <string>
#include <vector>

#include "buff.h"
class Target;

class Player
{
private:
	//static character stat values
	float wep_dps;
	int agility;
	float crit, haste, versatility, mastery;
	
	const float base_auto_attack_speed = 1;
	const float base_gcd = 1.5f; //TODO: Check if this is correct gcd

	//dynamic combat buffs and buffed stats
	std::list<Buff> stat_buffs, skill_buffs;
	int buffed_attack_power = 0;
	float buffed_crit = 0, buffed_haste = 0, buffed_versatility = 0, buffed_mastery = 0;

	//dynamic combat values
	float autoattack_timer = 0, gcd_timer = 0;
	int combopoints = 0;
	float energy = 100; //TODO: If talent increases this value, implement it (as buff?)


	void update_buffs_and_stats(float time_delta);
public:
	Player(float wep_dps, int agility, float crit, float haste, float versatility, float mastery);
	void tick(float time_delta, Target &target);

	//stat getters
	
	float get_wep_dps() const { return wep_dps; }
	std::list<Buff> get_stat_buffs() const { return stat_buffs; }
	std::list<Buff> get_skill_buffs() const { return skill_buffs; }
	int get_buffed_attack_power() const { return buffed_attack_power; }
	float get_buffed_crit() const { return buffed_crit; }
	float get_buffed_haste() const { return buffed_haste; }
	float get_buffed_versatility() const { return buffed_versatility; }
	float get_buffed_master() const { return buffed_mastery; }
	

	//statevector get_state_vector();
	//action_mask get_valid_action_mask();
};