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

Tigers_Fury_Buff::Tigers_Fury_Buff(bool predator)
	:
	Buff{ predator ? Buff{"tigers_fury", BuffCategory::SkillBuff, 1, 1, 22.0, false}
				   : Buff{"tigers_fury", BuffCategory::SkillBuff, 1, 1, 17.0, false} }
{}

Berserk_Buff::Berserk_Buff(bool incarnation)
	:
	Buff{ incarnation ? Buff{"berserk", BuffCategory::SkillBuff, 1, 1, 30.0, false}
					  : Buff{"berserk", BuffCategory::SkillBuff, 1, 1, 20.0, false} }
{}

Incarnation_Buff::Incarnation_Buff()
	:
	Buff("incarnation", BuffCategory::SkillBuff, 1, 1, 30.0, false)
{}

Heart_Of_The_Wild_Buff::Heart_Of_The_Wild_Buff()
	:
	Buff("heart_of_the_wild", BuffCategory::SkillBuff, 1, 1, 45.0, false)
{}

Savage_Roar_Buff::Savage_Roar_Buff(float init_duration)
	:
	//Note: Savage roar pandemic is bugged and only accounts for 6s*CP*1.3 and not 6s*(CP+1)*1.3
	Buff("savage_roar", BuffCategory::SkillBuff, 1, 1, init_duration, 36, true, 0.25f)
{
}

Bloodtalons_Buff::Bloodtalons_Buff()
	:
	Buff("bloodtalons", BuffCategory::SkillBuff, 2, 2, 30.0, false)
{}