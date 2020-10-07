#pragma once

#include "skill.h"

class AutoAttack : public Skill
{
public:
	AutoAttack
	(
		Player& player
	);

	float calc_instant_dmg() override;
};

