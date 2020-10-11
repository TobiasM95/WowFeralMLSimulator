#pragma once
#include <list>
#include <string>
#include <vector>
#include <array>

#include "buff.h"
class Target;

class Player
{
private:
	//static character stat values
	float wep_dps;
	int agility;
	float crit, haste, versatility, mastery;
	std::array<int, 7> talents;
	
	const float base_auto_attack_speed = 1;
	const float base_gcd = 1.5f; //TODO: Check if this is correct gcd




	void update_buffs_and_stats(float time_delta);
public:
	Player(float wep_dps, int agility, float crit, float haste, float versatility, float mastery,
		std::array<int, 7> talents);
	void tick(float time_delta, Target &target);
	void proc_buff(Buff& buff);

	//wep dps is static but is directly used to calculate auto attack dmg
	float get_wep_dps() const { return wep_dps; }
	//dynamic combat buffs and buffed stats
	std::list<Buff> stat_buffs, skill_buffs;
	int buffed_attack_power = 0;
	float buffed_crit = 0, buffed_haste = 0, buffed_versatility = 0, buffed_mastery = 0, buffed_energy = 0;

	//dynamic combat values
	float autoattack_timer = 0, gcd_timer = 0;
	int combopoints = 0;
	float base_energy = 100; //TODO: If talent increases this value, implement it (as buff?)

	//misc combat values
	float ooc_icd = 0.0f;

	//statevector get_state_vector();
	//action_mask get_valid_action_mask();
};