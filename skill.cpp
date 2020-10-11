#include "skill.h"

#include <vector>
#include <list>
#include "target.h"
#include "buff.h"


Skill::Skill
(
	Player& player,
	std::vector<Buff> affected_by_buff,
	std::vector<Buff> consumes_buff,
	std::vector<Buff> can_proc_buff
): 
	player(player),
	affected_by_buff(affected_by_buff),
	consumes_buff(consumes_buff),
	can_proc_buff(can_proc_buff)
{
}

Dot::Dot
(
	Player& player,
	std::vector<Buff> affected_by_buff,
	std::vector<Buff> consumes_buff, 
	std::vector<Buff> can_proc_buff,
	float max_duration, 
	float tick_dmg, 
	float tick_every,
	std::vector<Buff> tick_affected_by_buff,
	std::vector<Buff> tick_consumes_buff, 
	std::vector<Buff> tick_can_proc_buff
): 
	Skill(player, affected_by_buff, consumes_buff, can_proc_buff), 
	max_duration(max_duration),
	tick_dmg(tick_dmg),
	tick_every(tick_every),
	tick_affected_by_buff(tick_affected_by_buff), 
	tick_consumes_buff(tick_consumes_buff), 
	tick_can_proc_buff(tick_can_proc_buff)
{
	duration_left = max_duration;
}

