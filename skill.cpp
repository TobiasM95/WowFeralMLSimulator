#include "skill.h"

#include <vector>
#include <list>
#include "target.h"
#include "buff.h"
#include "player.h"


Skill::Skill
(
	Player& player,
	bool ignore_armor
): 
	player(&player),
	ignore_armor(ignore_armor)
{
}

Dot::Dot
(
	Player& player,
	bool ignore_armor,
	std::string name,
	float max_duration,
	float tick_every,
	bool can_pandemic,
	float pandemic_window
) :
	Skill(player, ignore_armor),
	name(name),
	max_duration(max_duration),
	tick_every(tick_every),
	can_pandemic(can_pandemic),
	pandemic_window(pandemic_window)
{
	duration_left = max_duration;
}

bool Dot::tick(float time_delta)
{
	duration_left -= time_delta;
	tick_timer += (1.0f + player->get_buffed_haste()) * time_delta;
	return (duration_left <= 0.0f) || (tick_timer >= tick_every);
}
