#include "target.h"
#include "skill.h"

#include <iostream>

void Target::resolve(Skill& s)
{
	//TODO: temporary, do way more stuff here, see notepad++
	received_dmg += s.calc_instant_dmg();
}

void Target::tick(float time_delta)
{
	//TODO: implement dot updates and their effects, see notepad ++
	fight_duration += time_delta;
	dps = received_dmg / fight_duration;
	//std::cout << "Fight duration: " << fight_duration << " - DPS: " << dps << "      \r";
}

