#pragma once
#include <list>
#include <string>
#include <vector>
#include <array>

#include "buff.h"
#include "target.h"

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

	//Cooldowns and charges, should not be interacted with directly
	//always give max charges (some large number that wont overflow)
	float brutal_slash_cd_timer = 3000;
	float feral_frenzy_cd_timer = 3000;
	float starsurge_cd_timer = 3000;
	float berserk_cd_timer = 3000;
	float tigers_fury_cd_timer = 3000;
	float heart_of_the_wild_cd_timer = 3000;
	float brutal_slash_cd = 8;
	float feral_frenzy_cd = 45;
	float starsurge_cd = 10;
	float berserk_cd = 180;
	float tigers_fury_cd = 30;
	float heart_of_the_wild_cd = 300;
	int brutal_slash_charges = 3;
	int feral_frenzy_charges = 1;
	int starsurge_charges = 1;
	int berserk_charges = 1;
	int tigers_fury_charges = 1;
	int heart_of_the_wild_charges = 1;

	void apply_talent_passive_buffs();
	void update_buffs_and_stats(float time_delta);
	void update_cooldowns(float time_delta);
	void update_bt_tickers(float time_delta);

	void start_action_by_index(int action_index);
public:
	Player(float wep_dps, int agility, int crit_abs, 
		int haste_abs, int versatility_abs, int mastery_abs,
		std::array<int, 7> talents);
	void tick(float time_delta);
	void start_gcd();
	void start_gcd(float manual_gcd);
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

	void perform_action();
	void perform_random_action();

	float get_buffed_attack_power();
	float get_buffed_crit();
	float get_buffed_haste();
	float get_buffed_versatility();
	float get_buffed_mastery();

	int charge_count(std::string name);
	void use_charge(std::string name);

	//only for logging purposes
	int id = 0;

	//Note: Can be list of targets in the future
	std::shared_ptr<Target> target;

	const int max_combopoints = 5;
	const float base_auto_attack_speed = 1;
	const float base_gcd = 1.0f;
	const float feral_aura_buff = 1.2f;

	//wep dps is static but is directly used to calculate auto attack dmg
	float get_wep_dps() const { return wep_dps; }
	//dynamic combat buffs and buffed stats
	std::list<Buff> stat_buffs, skill_buffs;

	//dynamic combat values
	float autoattack_timer = 5.0f, gcd_timer = 0.0f;
	int combopoints = 0;
	float energy = 100;
	float max_energy = 100;
	float base_energy_regen = 10.0f; //TODO: Check base energy regen value
	bool stealth = true;
	bool has_inc_stealth_charge = false;
	bool cat_form = true;

	//misc combat values
	float ooc_icd = 0.0f;

	//bloodtalon tickers, tslu = time since last use
	float tslu_shred = 5.0f;
	float tslu_rake = 5.0f;
	float tslu_thrash = 5.0f;
	float tslu_brutal_slash = 5.0f;

	std::vector<float> get_state_vector();
	std::vector<int> get_valid_action_mask();
};