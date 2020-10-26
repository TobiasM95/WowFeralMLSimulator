#include "feral_buffs.h"

//The specific effects are applied when a skill is resolved

OmenOfClarity::OmenOfClarity()
	:
	Buff("omen_of_clarity", BuffCategory::SkillBuff, 1, 1, 15.0, false)
{}

MomentOfClarity::MomentOfClarity()
	:
	Buff("moment_of_clarity", BuffCategory::SkillBuff, 1, 2, 15.0, false)
{}
