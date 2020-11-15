#include "player.h"
#include "../skills/feral_skills.h"
#include "../entity/target.h"
#include "../buffs/buff.h"
#include "../buffs/feral_buffs.h"
#include "../utils/random_gen.h"
#include "../simulator/simulator.h"

//is needed for exception in has_talents
#include <iostream>
#include <algorithm>
#include <numeric>

Player::Player(float wep_speed, float wep_dps, int agility, int crit_abs, int haste_abs, int mastery_abs, 
	int versatility_abs, std::array<int, 7> talents) :
	wep_speed(wep_speed), wep_dps(wep_dps), agility(agility), crit_abs(crit_abs), haste_abs(haste_abs), 
	mastery_abs(mastery_abs), versatility_abs(versatility_abs), talents(talents)
{
	apply_talent_passive_buffs();
}

void Player::reset()
{
	buffed_attack_power = 0;
	buffed_crit = 0;
	buffed_haste = 0;
	buffed_versatility = 0;
	buffed_mastery = 0;
	attack_power_up_to_date = false;
	crit_up_to_date = false;
	haste_up_to_date = false;
	versatility_up_to_date = false;
	mastery_up_to_date = false;
	brutal_slash_cd_timer = 3000;
	feral_frenzy_cd_timer = 3000;
	starsurge_cd_timer = 3000;
	berserk_cd_timer = 3000;
	tigers_fury_cd_timer = 3000;
	heart_of_the_wild_cd_timer = 3000;
	brutal_slash_charges = 3;
	feral_frenzy_charges = 1;
	starsurge_charges = 1;
	berserk_charges = 1;
	tigers_fury_charges = 1;
	heart_of_the_wild_charges = 1;
	stat_buffs.clear();
	skill_buffs.clear();
	autoattack_timer = 5.0f, gcd_timer = 0.0f;
	combopoints = 0;
	energy = max_energy;
	stealth = true;
	cat_form = true;
	ooc_icd = 0.0f;
	tslu_shred = 5.0f;
	tslu_rake = 5.0f;
	tslu_thrash = 5.0f;
	tslu_brutal_slash = 5.0f;
}

void Player::tick(float time_delta)
{
	autoattack_timer -= time_delta;
	gcd_timer -= time_delta;
	update_buffs_and_stats(time_delta);
	update_cooldowns(time_delta);

	//TODO: Check energy regen rates
	energy = std::min(max_energy, energy + base_energy_regen * time_delta * (1.0f + get_buffed_haste()) * (1.0f + 0.1f * has_buff("savage_roar")));

	perform_action();

	if (!stealth && AutoAttack::check_rdy(*this))
	{
		AutoAttack a(*this);
		target->resolve(a);
	}

	target->tick(time_delta);
	update_bt_tickers(time_delta);

#ifdef _DEBUG
	std::cout << "________PLAYER SUMMARY________\n";
	std::cout << "Wep swing timer: " << autoattack_timer << " GCD timer: " << gcd_timer << "\n";
	std::cout << "Energy: " << energy << "\n";
	std::cout << "Number of buffs: " << skill_buffs.size() + stat_buffs.size() << "\n";
	for (Buff b : skill_buffs) {
		std::cout << b.name << " " << b.duration << "/" << b.max_duration
			<< "  " << b.stacks << "/" << b.max_stacks << "\n";
	}
	for (Buff b : stat_buffs) {
		std::cout << b.name << " " << b.duration << "/" << b.max_duration
			<< "  " << b.stacks << "/" << b.max_stacks << "\n";
	}
#endif
}

void Player::start_gcd()
{
	gcd_timer = base_gcd;
}

void Player::start_gcd(float manual_gcd)
{
	gcd_timer = manual_gcd;
}

bool Player::gcd_ready()
{
	return gcd_timer <= 0;
}

void Player::add_combopoints(int num_cps)
{
	combopoints = std::min(max_combopoints, combopoints + num_cps);
}

void Player::reset_combopoints()
{
	combopoints = 0;
}

void Player::apply_talent_passive_buffs()
{
	//NOTE: Only additional talent effects are changed here, do talent checking with has_talent instead
	if (has_talent("moment_of_clarity")) {
		max_energy = 130;
		energy = 130;
	}
		
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

	//set these flags to false
	//these get set to true if they are calculated once per tick
	//on every subsequent call to get_buffed_(stat) only the already calculated value is returned
	//with this we only need to calculated the buffed value when needed and only once even when
	//need twice per tick at different places
	attack_power_up_to_date = false;
	crit_up_to_date = false;
	haste_up_to_date = false;
	versatility_up_to_date = false;
	mastery_up_to_date = false;
}

void Player::update_cooldowns(float time_delta)
{
	//Brutal slash is affected by haste
	brutal_slash_cd_timer += (1.0f + get_buffed_haste()) * time_delta;
	feral_frenzy_cd_timer += time_delta;
	starsurge_cd_timer += time_delta;
	berserk_cd_timer += time_delta;
	tigers_fury_cd_timer += time_delta;
}

void Player::update_bt_tickers(float time_delta)
{
	if ((tslu_shred < 4.0f) + (tslu_rake < 4.0f) + (tslu_thrash < 4.0f) + (tslu_brutal_slash < 4.0f) >= 3)
	{
		tslu_shred = 5.0f;
		tslu_rake = 5.0f;
		tslu_thrash = 5.0f;
		tslu_brutal_slash = 5.0f;
		Bloodtalons_Buff btb = {};
		proc_buff(btb, 2, true);
	}
	tslu_shred += time_delta;
	tslu_rake += time_delta;
	tslu_thrash += time_delta;
	tslu_brutal_slash += time_delta;
}

void Player::proc_buff(Buff& buff, int stacks, bool reset_duration) 
{
	Buff *existing_buff = nullptr;
	//Search through existing buffs if ex buff matches the procced one
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
	//If buff exists just add a stack and reset duration
	if (existing_buff && existing_buff != NULL)
	{
		existing_buff->add_stack(stacks);
		if(reset_duration)
			existing_buff->add_duration(buff.duration);
	}
	else //If buff does not exist, add buff to list
	{
		if (buff.category == BuffCategory::SkillBuff)
			skill_buffs.push_back(buff);
		else
			stat_buffs.push_back(buff);
	}
}

void Player::consume_buff(std::list<Buff>::iterator buff_it, std::string list_name)
{
	std::list<Buff>* buff_list;
	if (list_name == "skill" || list_name == "skill_buffs")
		buff_list = &skill_buffs;
	else if (list_name == "stat" || list_name == "stat_buffs")
		buff_list = &stat_buffs;
	else
		throw std::invalid_argument("This type of Buff list does not exist: " + list_name);
	if (buff_it->stacks == 1)
		buff_list->erase(buff_it);
	else
		buff_it->remove_stack(1);
		
}

bool Player::has_talent(std::string talent_name)
{
	if (talent_name == "predator")
		return has_talent(0, 0);
	if (talent_name == "sabertooth")
		return has_talent(0, 1); 
	if (talent_name == "lunar_inspiration")
		return has_talent(0, 2);
	if (talent_name == "tiger_dash")
		return has_talent(1, 0);
	if (talent_name == "renewal")
		return has_talent(1, 1);
	if (talent_name == "wild_charge")
		return has_talent(1, 2);
	if (talent_name == "balance_affinity")
		return has_talent(2, 0);
	if (talent_name == "guardian_affinity")
		return has_talent(2, 1);
	if (talent_name == "restoration_affinity")
		return has_talent(2, 2);
	if (talent_name == "mighty_bash")
		return has_talent(3, 0);
	if (talent_name == "mass_entanglement")
		return has_talent(3, 1);
	if (talent_name == "heart_of_the_wild")
		return has_talent(3, 2);
	if (talent_name == "soul_of_the_forest")
		return has_talent(4, 0);
	if (talent_name == "savage_roar")
		return has_talent(4, 1);
	if (talent_name == "incarnation")
		return has_talent(4, 2);
	if (talent_name == "scent_of_blood")
		return has_talent(5, 0);
	if (talent_name == "brutal_slash")
		return has_talent(5, 1);
	if (talent_name == "primal_wrath")
		return has_talent(5, 2);
	if (talent_name == "moment_of_clarity")
		return has_talent(6, 0);
	if (talent_name == "bloodtalons")
		return has_talent(6, 1);
	if (talent_name == "feral_frenzy")
		return has_talent(6, 2);
	throw std::invalid_argument("Talent does not exist: " + talent_name);
}

bool Player::has_talent(int row, int col)
{
	return talents.at(row) == col;
}

//NOTE: Change Buff list to list of pointers?
std::list<Buff>::iterator Player::get_skill_buff(std::string name)
{
	return std::find(skill_buffs.begin(), skill_buffs.end(), name);
}

std::list<Buff>::iterator Player::get_stat_buff(std::string name)
{
	return std::find(stat_buffs.begin(), stat_buffs.end(), name);
}

bool Player::has_skill_buff(std::string name)
{
	return get_skill_buff(name) != skill_buffs.end();
}
bool Player::has_stat_buff(std::string name)
{
	return get_stat_buff(name) != stat_buffs.end();
}
bool Player::has_buff(std::string name)
{
	return has_skill_buff(name) || has_stat_buff(name);
}

float Player::get_buffed_attack_power()
{
	if (!attack_power_up_to_date)
	{
		attack_power_up_to_date = true;
		//NOTE: Implement attack power and agility buffs
		buffed_attack_power =  agility + 6 * wep_dps;
	}
	return buffed_attack_power;
}

float Player::get_buffed_crit()
{
	if (!crit_up_to_date)
	{
		crit_up_to_date = true;
		float crit_rating = (float)crit_abs;
		//NOTE: Apply rating buffs here
		//TODO_SL: With Shadowlands apply penalty here 
		//Penalty: 0-30%:0%, 30-39%:10%, 39-47%:30%, 47-54%:30%, 54-66%:40%, 66-126%:50%, >126%:100%
		//Crit rating conversion: 1 rating = 0,00093718%
		//NOTE: Apply percentage buffs here, but also change crit_rel
		buffed_crit = crit_rating * 0.00093718f + base_crit;
	}
	return buffed_crit;
}

float Player::get_buffed_haste()
{
	if (!haste_up_to_date)
	{
		haste_up_to_date = true;
		float haste_rating = (float)haste_abs;
		//NOTE: Apply rating buffs here
		//TODO_SL: With Shadowlands apply penalty here 
		//Penalty: 0-30%:0%, 30-39%:10%, 39-47%:30%, 47-54%:30%, 54-66%:40%, 66-126%:50%, >126%:100%
		//haste rating conversion: 1 rating = 0,00093718%
		//NOTE: Apply percentage buffs here, but also change haste_rel
		buffed_haste =  haste_rating * 0.00099359f + base_haste;
	}
	return buffed_haste;
}

float Player::get_buffed_versatility()
{
	if (!versatility_up_to_date)
	{
		versatility_up_to_date = true;
		float versatility_rating = (float)versatility_abs;
		//NOTE: Apply rating buffs here
		//TODO_SL: With Shadowlands apply penalty here 
		//Penalty: 0-30%:0%, 30-39%:10%, 39-47%:30%, 47-54%:30%, 54-66%:40%, 66-126%:50%, >126%:100%
		//versatility rating conversion: 1 rating = 0,00093718%
		//NOTE: Apply percentage buffs here, but also change versatility_rel
		buffed_versatility = versatility_rating * 0.00081875f + base_versatility;
	}
	return buffed_versatility;
}

float Player::get_buffed_mastery()
{
	if (!mastery_up_to_date)
	{
		mastery_up_to_date = true;
		float mastery_rating = (float)mastery_abs;
		//NOTE: Apply rating buffs here
		//TODO_SL: With Shadowlands apply penalty here 
		//Penalty: 0-30%:0%, 30-39%:10%, 39-47%:30%, 47-54%:30%, 54-66%:40%, 66-126%:50%, >126%:100%
		//mastery rating conversion: 1 rating = 0,00093718%
		//NOTE: Apply percentage buffs here, but also change mastery_rel
		buffed_mastery = mastery_rating * 0.00187422f + base_mastery;
	}
	return buffed_mastery;
}

int Player::charge_count(std::string name)
{
	if (name == "brutal_slash")
		return std::min(
			(int)(brutal_slash_cd_timer / brutal_slash_cd),
			brutal_slash_charges
		);
	else if (name == "feral_frenzy")
		return std::min(
			(int)(feral_frenzy_cd_timer / feral_frenzy_cd),
			feral_frenzy_charges
		);
	else if (name == "starsurge")
		return std::min(
			(int)(starsurge_cd_timer / starsurge_cd),
			starsurge_charges
		);
	else if (name == "berserk")
		return std::min(
			(int)(berserk_cd_timer / berserk_cd),
			berserk_charges
		);
	else if (name == "tigers_fury")
		return std::min(
			(int)(tigers_fury_cd_timer / tigers_fury_cd),
			tigers_fury_charges
		);
	else if (name == "heart_of_the_wild")
		return std::min(
			(int)(heart_of_the_wild_cd_timer / heart_of_the_wild_cd),
			heart_of_the_wild_charges
		);
	else
		throw std::invalid_argument("Skill with charges does not exist: " + name);
}

void Player::use_charge(std::string name)
{
	if (name == "brutal_slash")
	{
		if (brutal_slash_cd_timer > brutal_slash_cd * brutal_slash_charges)
			brutal_slash_cd_timer = brutal_slash_cd * (brutal_slash_charges - 1);
		else
			brutal_slash_cd_timer -= brutal_slash_cd;
	}
	else if (name == "feral_frenzy")
	{
		if (feral_frenzy_cd_timer > feral_frenzy_cd * feral_frenzy_charges)
			feral_frenzy_cd_timer = feral_frenzy_cd * (feral_frenzy_charges - 1);
		else
			feral_frenzy_cd_timer -= feral_frenzy_cd;
	}
	else if (name == "starsurge")
	{
		if (starsurge_cd_timer > starsurge_cd * starsurge_charges)
			starsurge_cd_timer = starsurge_cd * (starsurge_charges - 1);
		else
			starsurge_cd_timer -= starsurge_cd;
	}
	else if (name == "berserk")
	{
		if (berserk_cd_timer > berserk_cd * berserk_charges)
			berserk_cd_timer = berserk_cd * (berserk_charges - 1);
		else
			berserk_cd_timer -= berserk_cd;
	}
	else if (name == "tigers_fury")
	{
		if (tigers_fury_cd_timer > tigers_fury_cd * tigers_fury_charges)
			tigers_fury_cd_timer = tigers_fury_cd * (tigers_fury_charges - 1);
		else
			tigers_fury_cd_timer -= tigers_fury_cd;
	}
	else if (name == "heart_of_the_wild")
	{
		if (heart_of_the_wild_cd_timer > heart_of_the_wild_cd * heart_of_the_wild_charges)
			heart_of_the_wild_cd_timer = heart_of_the_wild_cd * (heart_of_the_wild_charges - 1);
		else
			heart_of_the_wild_cd_timer -= heart_of_the_wild_cd;
	}
	else
		throw std::invalid_argument("Skill with charges does not exist: " + name);
}

void Player::perform_action()
{
	perform_model_action();
	//perform_random_action();
}

void Player::perform_model_action()
{
	//delayed reward here, so just call logger with 0 reward
	std::vector<int> val_act_mask = get_valid_action_mask();
	if (std::accumulate(val_act_mask.begin(), val_act_mask.end(), 0) == 1)
		return;
	std::vector<float> state = get_state_vector();

	//action selection routine, placeholder start
	std::vector<int> action_indices;
	action_indices.reserve(val_act_mask.size());
	for (size_t ai = 0; ai < val_act_mask.size(); ai++)
	{
		if (val_act_mask.at(ai) == 1)
			action_indices.push_back(ai);
	}

	int act_ind = rng_namespace::getRandomInt(0, action_indices.size() - 1);
	int selected_action = action_indices.at(act_ind);
	//placeholder end

	start_action_by_index(selected_action);
	std::vector<float> new_state = get_state_vector();
	if (target->simulator->log_events)
	{
		target->simulator->logger.log_transitions(
			id, state, selected_action, val_act_mask, 0, new_state
		);
	}
}

void Player::perform_random_action()
{
	if (!gcd_ready())
		return;
	std::vector<float> sv = get_state_vector();

	std::vector<int> valid_action_mask = get_valid_action_mask();

	std::vector<int> action_indices;
	action_indices.reserve(valid_action_mask.size());
	for (size_t ai = 0; ai < valid_action_mask.size(); ai++)
	{
		if(valid_action_mask.at(ai) == 1)
			action_indices.push_back(ai);
	}

	int act_ind = rng_namespace::getRandomInt(0, action_indices.size()-1);
	int selected_action = action_indices.at(act_ind);

	start_action_by_index(selected_action);
}

std::vector<float> Player::get_state_vector()
{
	/* State vector description
	* 0: Energy divided by 100
	* all CDs are divided by base max length
	* 1: CD: Brutal slash cd = std::max(0, cd - cd_timer), gives value > 0 only if no charge available
	* 2: CD: Feral frenzy cd = std::max(0, cd - cd_timer), gives value > 0 only if no charge available
	* 3: CD: Starsurge cd = std::max(0, cd - cd_timer), gives value > 0 only if no charge available
	* 4: CD: Berserk slash cd = std::max(0, cd - cd_timer), gives value > 0 only if no charge available
	* 5: CD: Tigers fury slash cd = std::max(0, cd - cd_timer), gives value > 0 only if no charge available
	* 6: CD: Heart of the wild slash cd = std::max(0, cd - cd_timer), gives value > 0 only if no charge available
	* 7: Charge: Brutal slash charges
	* all dots are divided by base max length
	* 8: DOT: Rake dot duration has_dot?duration_left:0
	* 9: DOT: Thrash dot duration has_dot?duration_left:0
	* 10: DOT: Feral frenzy dot duration has_dot?duration_left:0
	* 11: DOT: moonfire feral dot duration has_dot?duration_left:0
	* 12: DOT: Sunfire dot duration has_dot?duration_left:0
	* 13: DOT: Rip dot duration has_dot?duration_left:0
	* all bufs are divided by base max length
	* 14: BUFF: Tigers Fury duration left has_buff?duration_left:0
	* 15: BUFF: Berserk duration left has_buff?duration_left:0
	* 16: BUFF: Heart of the wild duration left has_buff?duration_left:0
	* 17: BUFF: Savage roar duration left has_buff?duration_left:0
	* 18: BUFF: Bloodtalons duration left has_buff?duration_left:0
	* 19: BUFF: OoC/MoC duration left has_buff?duration_left:0
	* misc
	* 20: cat form, 1 for true
	* 21: combopoints / 5.0f
	*/
	std::vector<float> state_vector(22);
	state_vector.at(0) = energy / 100.0f;
	//cooldowns
	state_vector.at(1) = std::max(0.0f, brutal_slash_cd - brutal_slash_cd_timer) / brutal_slash_cd;
	state_vector.at(2) = std::max(0.0f, feral_frenzy_cd - feral_frenzy_cd_timer) / feral_frenzy_cd;
	state_vector.at(3) = std::max(0.0f, starsurge_cd - starsurge_cd_timer) / starsurge_cd;
	state_vector.at(4) = std::max(0.0f, berserk_cd - berserk_cd_timer) / berserk_cd;
	state_vector.at(5) = std::max(0.0f, tigers_fury_cd - tigers_fury_cd_timer) / tigers_fury_cd;
	state_vector.at(6) = std::max(0.0f, heart_of_the_wild_cd - heart_of_the_wild_cd_timer) / heart_of_the_wild_cd;
	//charges
	state_vector.at(7) = charge_count("brutal_slash") * 1.0f / brutal_slash_charges;
	//dots
	std::shared_ptr<Dot> rake_dot = target->get_dot("rake_dot");
	state_vector.at(8) = rake_dot ? rake_dot->duration_left / rake_dot->max_duration : 0.0f;
	std::shared_ptr<Dot> thrash_dot = target->get_dot("thrash_dot");
	state_vector.at(9) = thrash_dot ? thrash_dot->duration_left / thrash_dot->max_duration : 0.0f;
	std::shared_ptr<Dot> feral_frenzy_dot = target->get_dot("feral_frenzy_dot");
	state_vector.at(10) = feral_frenzy_dot ? feral_frenzy_dot->duration_left / feral_frenzy_dot->max_duration : 0.0f;
	std::shared_ptr<Dot> moonfire_feral_dot = target->get_dot("moonfire_feral_dot");
	state_vector.at(11) = moonfire_feral_dot ? moonfire_feral_dot->duration_left / moonfire_feral_dot->max_duration : 0.0f;
	std::shared_ptr<Dot> sunfire_dot = target->get_dot("sunfire_dot");
	state_vector.at(12) = sunfire_dot ? sunfire_dot->duration_left / sunfire_dot->max_duration : 0.0f;
	std::shared_ptr<Dot> rip_dot = target->get_dot("rip_dot");
	state_vector.at(13) = rip_dot ? rip_dot->duration_left / rip_dot->max_duration : 0.0f;
	//buffs
	std::list<Buff>::iterator tigers_fury_buff = get_skill_buff("tigers_fury");
	state_vector.at(14) = (tigers_fury_buff != skill_buffs.end()) ? tigers_fury_buff->duration / tigers_fury_buff->max_duration : 0.0f;
	std::list<Buff>::iterator berserk_buff = get_skill_buff("berserk");
	state_vector.at(15) = (berserk_buff != skill_buffs.end()) ? berserk_buff->duration / berserk_buff->max_duration : 0.0f;
	std::list<Buff>::iterator heart_of_the_wild_buff = get_skill_buff("heart_of_the_wild");
	state_vector.at(16) = (heart_of_the_wild_buff != skill_buffs.end()) ? heart_of_the_wild_buff->duration / heart_of_the_wild_buff->max_duration : 0.0f;
	std::list<Buff>::iterator savage_roar_buff = get_skill_buff("savage_roar");
	state_vector.at(17) = (savage_roar_buff != skill_buffs.end()) ? savage_roar_buff->duration / savage_roar_buff->max_duration : 0.0f;
	std::list<Buff>::iterator bloodtalons_buff = get_skill_buff("bloodtalons");
	state_vector.at(18) = (bloodtalons_buff != skill_buffs.end()) ? bloodtalons_buff->duration / bloodtalons_buff->max_duration : 0.0f;
	std::list<Buff>::iterator omen_of_clarity_buff = get_skill_buff("omen_of_clarity");
	std::list<Buff>::iterator moment_of_clarity_buff = get_skill_buff("moment_of_clarity");
	state_vector.at(19) = std::max(
		(omen_of_clarity_buff != skill_buffs.end()) ? omen_of_clarity_buff->duration / omen_of_clarity_buff->max_duration : 0.0f,
		(moment_of_clarity_buff != skill_buffs.end()) ? moment_of_clarity_buff->duration / moment_of_clarity_buff->max_duration : 0.0f
		);
	//misc
	state_vector.at(20) = 1.0f * cat_form;
	state_vector.at(21) = combopoints / 5.0f;
	return state_vector;
}

std::vector<int> Player::get_valid_action_mask()
{
	/*action index positions
	*  0: shred
	*  1: rake
	*  2: thrash
	*  3: brutal_slash
	*  4: feral_frenzy
	*  5: moonfire_feral
	*  6: sunfire
	*  7: starsurge
	*  8: rip
	*  9: ferocious_bite
	* 10: savage_roar
	* 11: tigers_fury
	* 12: berserk
	* 13: heart_of_the_wild
	* 14: cat_form
	* 15: moonkin_form
	* 16: idle
	*/
	std::vector<int> valid_action_mask(17);
	if (gcd_ready())
	{
		if (Shred::check_rdy(*this))
			valid_action_mask.at(0) = 1;
		if (Rake::check_rdy(*this))
			valid_action_mask.at(1) = 1;
		if (Thrash::check_rdy(*this))
			valid_action_mask.at(2) = 1;
		if (Brutal_Slash::check_rdy(*this))
			valid_action_mask.at(3) = 1;
		if (Feral_Frenzy::check_rdy(*this))
			valid_action_mask.at(4) = 1;
		if (Moonfire_Feral::check_rdy(*this))
			valid_action_mask.at(5) = 1;
		if (Sunfire::check_rdy(*this))
			valid_action_mask.at(6) = 1;
		if (Starsurge::check_rdy(*this))
			valid_action_mask.at(7) = 1;
		if (Rip::check_rdy(*this))
			valid_action_mask.at(8) = 1;
		if (Ferocious_Bite::check_rdy(*this))
			valid_action_mask.at(9) = 1;
		if (Savage_Roar::check_rdy(*this))
			valid_action_mask.at(10) = 1;
		if (Tigers_Fury::check_rdy(*this))
			valid_action_mask.at(11) = 1;
		if (Berserk::check_rdy(*this))
			valid_action_mask.at(12) = 1;
		if (Heart_Of_The_Wild::check_rdy(*this))
			valid_action_mask.at(13) = 1;
		if (Cat_Form::check_rdy(*this))
			valid_action_mask.at(14) = 1;
		if (Moonkin_Form::check_rdy(*this))
			valid_action_mask.at(15) = 1;
		//idle is always ready
		valid_action_mask.at(16) = 1;
	}
	else
	{
		if (Tigers_Fury::check_rdy(*this))
			valid_action_mask.at(11) = 1;
		if (Berserk::check_rdy(*this))
			valid_action_mask.at(12) = 1;
		valid_action_mask.at(16) = 1;
	}
	return valid_action_mask;
}

void Player::start_action_by_index(int action_index)
{
#ifdef _DEBUG
	switch (action_index)
	{
	case 0: {Shred a(*this); target->resolve(a); std::cout << "Perform Shred\n"; }break;
	case 1: {target->resolve(std::shared_ptr<Dot>(new Rake(*this))); std::cout << "Perform Rake\n"; }break;
	case 2: {target->resolve(std::shared_ptr<Dot>(new Thrash(*this))); std::cout << "Perform Thrash\n"; }break;
	case 3: {Brutal_Slash a(*this); target->resolve(a); std::cout << "Perform Brutal Slash\n"; }break;
	case 4: {target->resolve(std::shared_ptr<Dot>(new Feral_Frenzy(*this))); std::cout << "Perform Feral Frenzy\n"; }break;
	case 5: {target->resolve(std::shared_ptr<Dot>(new Moonfire_Feral(*this))); std::cout << "Perform Moonfire (Feral)\n"; }break;
	case 6: {target->resolve(std::shared_ptr<Dot>(new Sunfire(*this))); std::cout << "Perform Sunfire\n"; }break;
	case 7: {Starsurge a(*this); target->resolve(a); std::cout << "Perform Starsurge\n"; }break;
	case 8: {target->resolve(std::shared_ptr<Dot>(new Rip(*this))); std::cout << "Perform Rip\n"; }break;
	case 9: {Ferocious_Bite a(*this); target->resolve(a); std::cout << "Perform Ferocious Bite\n"; }break;
	case 10: {Savage_Roar a(*this); target->resolve(a); std::cout << "Perform Savage Roar\n"; }break;
	case 11: {Tigers_Fury a(*this); target->resolve(a); std::cout << "Perform Tiger's Fury\n"; }break;
	case 12: {Berserk a(*this); target->resolve(a); std::cout << "Perform Berserk\n"; }break;
	case 13: {Heart_Of_The_Wild a(*this); target->resolve(a); std::cout << "Perform Heart of the Wild\n"; }break;
	case 14: {Cat_Form a(*this); target->resolve(a); std::cout << "Perform Cat Form\n"; }break;
	case 15: {Moonkin_Form a(*this); target->resolve(a); std::cout << "Perform Moonkin Form\n"; }break;
	case 16: {std::cout << "Perform Idle\n"; }break;
	}
#else
	switch (action_index)
	{
	case 0: {Shred a(*this); target->resolve(a); }break;
	case 1: {target->resolve(std::shared_ptr<Dot>(new Rake(*this))); }break;
	case 2: {target->resolve(std::shared_ptr<Dot>(new Thrash(*this))); }break;
	case 3: {Brutal_Slash a(*this); target->resolve(a); }break;
	case 4: {target->resolve(std::shared_ptr<Dot>(new Feral_Frenzy(*this))); }break;
	case 5: {target->resolve(std::shared_ptr<Dot>(new Moonfire_Feral(*this))); }break;
	case 6: {target->resolve(std::shared_ptr<Dot>(new Sunfire(*this))); }break;
	case 7: {Starsurge a(*this); target->resolve(a); }break;
	case 8: {target->resolve(std::shared_ptr<Dot>(new Rip(*this))); }break;
	case 9: {Ferocious_Bite a(*this); target->resolve(a); }break;
	case 10: {Savage_Roar a(*this); target->resolve(a); }break;
	case 11: {Tigers_Fury a(*this); target->resolve(a); }break;
	case 12: {Berserk a(*this); target->resolve(a); }break;
	case 13: {Heart_Of_The_Wild a(*this); target->resolve(a); }break;
	case 14: {Cat_Form a(*this); target->resolve(a); }break;
	case 15: {Moonkin_Form a(*this); target->resolve(a); }break;
	case 16: {};
	}
#endif
}