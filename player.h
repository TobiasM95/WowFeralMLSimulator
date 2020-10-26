#pragma once
#include <list>
#include <string>
#include <vector>
#include <array>

#include "buff.h"
#include "target.h"

//TODO: Prowl management (prowl buff, stop autoattacking, resume when attack is cast)
//TODO: Add variables is_prowl and incarn_prowl_charge
class Player
{
private:
	//static character stat values
	float wep_dps;
	int agility;
	int crit_abs, haste_abs, versatility_abs, mastery_abs;
	std::array<int, 7> talents;

	float base_crit = 0.1f;
	float base_haste = 0.0f;
	float base_versatility = 0.0f;
	float base_mastery = 0.16f;

	float buffed_attack_power = 0;
	float buffed_crit = 0;
	float buffed_haste = 0;
	float buffed_versatility = 0;
	float buffed_mastery = 0;

	bool attack_power_up_to_date = false;
	bool crit_up_to_date = false;
	bool haste_up_to_date = false;
	bool versatility_up_to_date = false;
	bool mastery_up_to_date = false;


	void apply_talent_passive_buffs();
	void update_buffs_and_stats(float time_delta);
public:
	Player(float wep_dps, int agility, int crit_abs, 
		int haste_abs, int versatility_abs, int mastery_abs,
		std::array<int, 7> talents);
	void tick(float time_delta);
	void start_gcd();
	bool gcd_ready();
	void add_combopoints(int num_cps);
	void reset_combopoints();
	void proc_buff(Buff& buff, int stacks, bool reset_duration);
	void consume_buff(std::list<Buff>::iterator buff_it, std::string list_name);
	bool has_talent(std::string talent_name);
	bool has_talent(int row, int col);
	std::list<Buff>::iterator get_skill_buff(std::string name);
	std::list<Buff>::iterator get_stat_buff(std::string name);
	bool has_skill_buff(std::string name);
	bool has_stat_buff(std::string name);
	bool has_buff(std::string name);

	float get_buffed_attack_power();
	float get_buffed_crit();
	float get_buffed_haste();
	float get_buffed_versatility();
	float get_buffed_mastery();

	//Note: Can be list of targets in the future
	Target target;

	const int max_combopoints = 5;
	const float base_auto_attack_speed = 1;
	const float base_gcd = 1.5f; //TODO: Check if this is correct gcd
	const float feral_aura_buff = 1.2f;

	//wep dps is static but is directly used to calculate auto attack dmg
	float get_wep_dps() const { return wep_dps; }
	//dynamic combat buffs and buffed stats
	std::list<Buff> stat_buffs, skill_buffs;

	//dynamic combat values
	float autoattack_timer = 0, gcd_timer = 0;
	int combopoints = 0;
	float energy = 100;
	float max_energy = 100; //TODO: If talent increases this value, implement it (as buff?)
	float base_energy_regen = 10;
	bool stealth = true;
	bool has_inc_stealth_charge = false;
	bool cat_form = true;

	//misc combat values
	float ooc_icd = 0.0f;

	//statevector get_state_vector();
	//action_mask get_valid_action_mask();
};