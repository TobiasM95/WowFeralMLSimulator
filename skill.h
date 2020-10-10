#pragma once

#include "player.h"
class Buff;

class Skill
{
protected:
	Player player;
	const std::vector<Buff> affected_by_buff;
	const std::vector<Buff> consumes_buff;
	const std::vector<Buff> can_proc_buff;

public:
	Skill
	(
		Player player,
		std::vector<Buff> affected_by_buff,
		std::vector<Buff> consumes_buff,
		std::vector<Buff> can_proc_buff
	);

	virtual float calc_instant_dmg() = 0;
};

class Dot : public Skill
{
protected:
	float max_duration;
	float duration_left;
	float tick_dmg;
	float tick_every;
	std::vector<Buff> tick_affected_by_buff;
	std::vector<Buff> tick_consumes_buff;
	std::vector<Buff> tick_can_proc_buff;
public:
	Dot
	(
		Player player, 
		std::vector<Buff> affected_by_buff,
		std::vector<Buff> consumes_buff, 
		std::vector<Buff> can_proc_buff,
		float max_duration, 
		float tick_dmg, 
		float tick_every,
		std::vector<Buff> tick_affected_by_buff,
		std::vector<Buff> tick_consumes_buff, 
		std::vector<Buff> tick_can_proc_buff
	);

	virtual float calc_tick_dmg() = 0;
	virtual float tick(float time_delta) = 0;
};