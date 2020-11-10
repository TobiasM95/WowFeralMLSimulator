#include "simulator.h"
#include "target.h"
#include "player.h"
#include "skill.h"

#ifdef _DEBUG
#include <iostream>
#endif

Target::Target(Simulator* simulator)
	:
	simulator(simulator)
{
}

void Target::resolve(Skill& s)
{
	float damage_reduction = 1.0f - (armor / (armor + k_value)) * !s.ignore_armor;
	std::pair<float, bool> damage_info = s.process_instant_effects();
	float res_damage_dealt = damage_reduction * debuff_multiplier * damage_info.first;
	received_dmg += res_damage_dealt;
	s.player->stealth = false;
	if (simulator->log_events)
		simulator->logger.log_events(simulator->get_current_time(), s, res_damage_dealt, damage_info.second, false);
}

void Target::resolve(std::shared_ptr<Dot> s)
{
	float damage_reduction = 1.0f - (armor / (armor + k_value)) * !s->ignore_armor;
	std::pair<float, bool> damage_info = s->process_instant_effects();
	float res_damage_dealt = damage_reduction * debuff_multiplier * damage_info.first;
	received_dmg += res_damage_dealt;
	s->player->stealth = false;
	apply_dot(s);
	if (simulator->log_events)
		simulator->logger.log_events(simulator->get_current_time(), *s, res_damage_dealt, damage_info.second, false);
}

void Target::tick(float time_delta)
{
	//TODO: implement dot updates and their effects, see notepad ++
	for (
			std::list<std::shared_ptr<Dot>>::iterator i = active_dots.begin(); 
			i != active_dots.end();
		) 
	{
		if ((*i)->tick(time_delta)) {
			std::pair<float, bool> damage_info = (*i)->process_dot_tick();
			float res_damage_dealt = debuff_multiplier * damage_info.first;
			received_dmg += res_damage_dealt;
			if (simulator->log_events)
				simulator->logger.log_events(simulator->get_current_time(), **i, res_damage_dealt, damage_info.second, true);
		}
		if ((*i)->duration_left <= 0)
			i = active_dots.erase(i);
		else
			i++;
	}

	fight_duration += time_delta;
#ifdef _DEBUG
	
#endif
#ifdef _DEBUG
	std::cout << "________TARGET SUMMARY________\n";
	std::cout << "Fight duration: " << fight_duration << " - DPS: " << received_dmg / fight_duration << "      \n";
	std::cout << "Number of dots: " << active_dots.size() << "\n";
	for (std::shared_ptr<Dot> d : active_dots) {
		std::cout << d->name << " " << d->duration_left << "/" << d->max_duration
			<< "  " << "\n";
	}
#endif
}

bool Target::has_dot(std::string name)
{
	bool has_dot = false;
	for (std::shared_ptr<Dot> d : active_dots)
	{
		if (d->name == name)
			has_dot = true;
	}
	return has_dot;
}

std::shared_ptr<Dot> Target::get_dot(std::string name)
{
	for (std::shared_ptr<Dot>& d : active_dots) {
		if (d->name == name)
			return d;
	}
	return nullptr;
}

void Target::apply_dot(std::shared_ptr<Dot> s)
{
	//What needs to be updated: max runtime, snapshot multiplier, everything else stays the same
	std::shared_ptr<Dot> d = get_dot(s->name);
	if(d == nullptr)
		active_dots.push_back(s);
	else
	{
		//Refresh duration
		d->duration_left = std::min(
			d->duration_left + s->duration_left,
			d->max_duration * (1.0f + d->pandemic_window * d->can_pandemic)
		);
		//Update snapshot multipliers
		d->stealth_snapshotted = s->stealth_snapshotted;
		d->tigers_fury_snapshotted = s->tigers_fury_snapshotted;
		d->bloodtalons_snapshotted = s->bloodtalons_snapshotted;
	}
}

