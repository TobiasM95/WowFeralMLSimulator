#include "player.h"
#include "feral_skills.h"
#include "target.h"
#include "buff.h"
#include "feral_buffs.h"

//is needed for exception in has_talents
#include <iostream>
#include <algorithm>

Player::Player(float wep_dps, int agility, int crit_abs, int haste_abs, int mastery_abs, 
	int versatility_abs, std::array<int, 7> talents) :
	wep_dps(wep_dps), agility(agility), crit_abs(crit_abs), haste_abs(haste_abs), 
	mastery_abs(mastery_abs), versatility_abs(versatility_abs), talents(talents)
{
	
	apply_talent_passive_buffs();
}

void Player::tick(float time_delta)
{
	autoattack_timer += time_delta;
	gcd_timer -= time_delta;
	update_buffs_and_stats(time_delta);

	//TODO: Energy update with buffed haste
	energy = std::min(max_energy, energy + base_energy_regen * time_delta * (1.0f + get_buffed_haste()) * (1.0f + 0.1f * has_buff("savage_roar")));

	if (!stealth && AutoAttack::check_rdy(*this)) 
	{
		AutoAttack a(*this);
		target.resolve(a);
	}

	//TODO: Perform action
	if (!target.has_dot("rake_dot") && Rake::check_rdy(*this)) 
	{
		std::shared_ptr<Dot> r(new Rake(*this));
		target.resolve(r);
	}
	else if (Shred::check_rdy(*this))
	{
		Shred s(*this);
		target.resolve(s);
	}

	target.tick(time_delta);

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
	gcd_timer = base_gcd / (1.0f + get_buffed_haste());
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
	/*
	if (has_talent("predator"))
	{
		TalentPredator t; skill_buffs.push_back(t);
	}
	if (has_talent("sabertooth"))
	{
		TalentSabertooth t; skill_buffs.push_back(t);
	}
	if (has_talent("lunar_inspiration"))
	{
		TalentLunarInspiration t; skill_buffs.push_back(t);
	}
	if (has_talent("heart_of_the_wild"))
	{
		TalentHeartOfTheWild t; skill_buffs.push_back(t);
	}
	if (has_talent("soul_of_the_forest"))
	{
		TalentSoulOfTheForest t; skill_buffs.push_back(t);
	}
	if (has_talent("savage_roar"))
	{
		TalentSavageRoar t; skill_buffs.push_back(t);
	}
	if (has_talent("incarnation"))
	{
		TalentIncarnation t; skill_buffs.push_back(t);
	}
	if (has_talent("brutal_slash"))
	{
		TalentBrutalSlash t; skill_buffs.push_back(t);
	}
	if (has_talent("moment_of_clarity"))
	{
		TalentMomentOfClarity t; skill_buffs.push_back(t);
	}
	if (has_talent("bloodtalons"))
	{
		TalentBloodtalons t; skill_buffs.push_back(t);
	}
	if (has_talent("feral_frenzy"))
	{
		TalentFeralFrenzy t; skill_buffs.push_back(t);
	}*/
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
		//TODO: Implement attack power and agility buffs
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
		//TODO: Apply rating buffs here
		//TODO: With Shadowlands apply penalty here 
		//Penalty: 0-25%:0%, 25-34%:10%, 34-42%:20%, 42-49%:30%, 49-106%:40%, >106%:100%
		//Crit rating conversion: 1 rating = 0,00093718%
		//TODO: Apply percentage buffs here, but also change crit_rel
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
		//TODO: Apply rating buffs here
		//TODO: With Shadowlands apply penalty here 
		//Penalty: 0-25%:0%, 25-34%:10%, 34-42%:20%, 42-49%:30%, 49-106%:40%, >106%:100%
		//haste rating conversion: 1 rating = 0,00093718%
		//TODO: Apply percentage buffs here, but also change haste_rel
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
		//TODO: Apply rating buffs here
		//TODO: With Shadowlands apply penalty here 
		//Penalty: 0-25%:0%, 25-34%:10%, 34-42%:20%, 42-49%:30%, 49-106%:40%, >106%:100%
		//versatility rating conversion: 1 rating = 0,00093718%
		//TODO: Apply percentage buffs here, but also change versatility_rel
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
		//TODO: Apply rating buffs here
		//TODO: With Shadowlands apply penalty here 
		//Penalty: 0-25%:0%, 25-34%:10%, 34-42%:20%, 42-49%:30%, 49-106%:40%, >106%:100%
		//mastery rating conversion: 1 rating = 0,00093718%
		//TODO: Apply percentage buffs here, but also change mastery_rel
		buffed_mastery = mastery_rating * 0.00187422f + base_mastery;
	}
	return buffed_mastery;
}