#pragma once
#include "buff.h"

//Talent passive buffs
//NOTE: Exclude non damage affecting talents (e.g. tiger dash etc.)
//NOTE: Scent of blood and primal wrath, since they're strictly aoe are excluded for now
//NOTE: Don't know if this is actually needed
/*
class TalentPredator : public Buff
{
public:
	TalentPredator() : Buff("talent_predator", BuffCategory::SkillBuff, 1, FLT_MAX, false) {};
};

class TalentSabertooth : public Buff
{
public:
	TalentSabertooth() : Buff("talent_sabertooth", BuffCategory::SkillBuff, 1, FLT_MAX, false) {};
};

class TalentLunarInspiration : public Buff
{
public:
	TalentLunarInspiration() : Buff("talent_lunar_inspiration", BuffCategory::SkillBuff, 1, FLT_MAX, false) {};
};

class TalentHeartOfTheWild : public Buff
{
public:
	TalentHeartOfTheWild() : Buff("talent_heart_of_the_wild", BuffCategory::SkillBuff, 1, FLT_MAX, false) {};
};

class TalentSoulOfTheForest : public Buff
{
public:
	TalentSoulOfTheForest() : Buff("talent_soul_of_the_forest", BuffCategory::SkillBuff, 1, FLT_MAX, false) {};
};

class TalentSavageRoar : public Buff
{
public:
	TalentSavageRoar() : Buff("talent_savage_roar", BuffCategory::SkillBuff, 1, FLT_MAX, false) {};
};

class TalentIncarnation : public Buff
{
public:
	TalentIncarnation() : Buff("talent_incarnation", BuffCategory::SkillBuff, 1, FLT_MAX, false) {};
};

class TalentBrutalSlash: public Buff
{
public:
	TalentBrutalSlash() : Buff("talent_brutal_slash", BuffCategory::SkillBuff, 1, FLT_MAX, false) {};
};

class TalentMomentOfClarity : public Buff
{
public:
	TalentMomentOfClarity() : Buff("talent_moment_of_clarity", BuffCategory::SkillBuff, 1, FLT_MAX, false) {};
};

class TalentBloodtalons : public Buff
{
public:
	TalentBloodtalons() : Buff("talent_bloodtalons", BuffCategory::SkillBuff, 1, FLT_MAX, false) {};
};

class TalentFeralFrenzy : public Buff
{
public:
	TalentFeralFrenzy() : Buff("talent_feral_frenzy", BuffCategory::SkillBuff, 1, FLT_MAX, false) {};
};*/

//active buffs
class OmenOfClarity : public Buff
{
public:
	OmenOfClarity();
};

class MomentOfClarity : public Buff
{
public:
	MomentOfClarity();
};

class Tigers_Fury_Buff : public Buff
{
public:
	Tigers_Fury_Buff(bool predator);
};

class Berserk_Buff : public Buff
{
public:
	Berserk_Buff(bool incarnation);
};

class Incarnation_Buff : public Buff
{
public:
	Incarnation_Buff();
};

class Heart_Of_The_Wild_Buff : public Buff
{
public:
	Heart_Of_The_Wild_Buff();
};

class Savage_Roar_Buff : public Buff
{
public:
	Savage_Roar_Buff(float init_duration);
};

class Bloodtalons_Buff : public Buff
{
public:
	Bloodtalons_Buff();
};