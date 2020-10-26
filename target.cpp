#include "target.h"
#include "player.h"
#include "skill.h"

#ifdef _DEBUG
#include <iostream>
#endif

void Target::resolve(Skill& s)
{
	float damage_reduction = 1.0f - (armor / (armor + k_value)) * !s.ignore_armor;
	received_dmg += damage_reduction * debuff_multiplier * s.process_instant_effects();
	s.player->stealth = false;
}

void Target::resolve(std::shared_ptr<Dot> s)
{
	float damage_reduction = 1.0f - (armor / (armor + k_value)) * !s->ignore_armor;
	received_dmg += damage_reduction * debuff_multiplier * s->process_instant_effects();
	s->player->stealth = false;
	//TODO: Dot management system, right now this leads to dot multiplication
	//What needs to be updated: max runtime, snapshot multiplier, everything else stays the same
	active_dots.push_back(s);
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
			received_dmg += debuff_multiplier * (*i)->process_dot_tick();
		}
		if ((*i)->duration_left <= 0)
			i = active_dots.erase(i);
		else
			i++;
	}

	fight_duration += time_delta;
	dps = received_dmg / fight_duration;
#ifdef _DEBUG
	
#endif
#ifdef _DEBUG
	std::cout << "________TARGET SUMMARY________\n";
	std::cout << "Fight duration: " << fight_duration << " - DPS: " << dps << "      \n";
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

