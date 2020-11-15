#include "skill.h"

#include <vector>
#include <list>
#include "../entity/target.h"
#include "../buffs/buff.h"
#include "../entity/player.h"


Skill::Skill
(
	std::string name,
	Player& player,
	bool ignore_armor
) :
	name(name),
	player(&player),
	ignore_armor(ignore_armor)
{
}

Dot::Dot
(
	std::string name,
	Player& player,
	bool ignore_armor,
	float max_duration,
	float tick_every,
	bool can_pandemic,
	float pandemic_window
) : Dot(
	name,
	player,
	ignore_armor,
	max_duration,
	max_duration,
	tick_every,
	can_pandemic,
	pandemic_window
)
{
}

Dot::Dot
(
	std::string name,
	Player& player,
	bool ignore_armor,
	float duration_left,
	float max_duration,
	float tick_every,
	bool can_pandemic,
	float pandemic_window
) :
	Skill(name, player, ignore_armor),
	duration_left(duration_left),
	max_duration(max_duration),
	tick_every(tick_every),
	can_pandemic(can_pandemic),
	pandemic_window(pandemic_window)
{
}

bool Dot::tick(float time_delta)
{
	duration_left -= time_delta;
	tick_timer += (1.0f + player->get_buffed_haste()) * time_delta;
	return (duration_left <= 0.0f) || (tick_timer >= tick_every);
}

void Dot::add_duration(float duration)
{
	duration_left = std::min(
		duration_left + duration,
		max_duration + can_pandemic * pandemic_window * max_duration
		);
}