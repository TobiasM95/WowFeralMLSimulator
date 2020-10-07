#pragma once

#include <vector>

#include "player.h"
#include "target.h"

class Simulator
{
private:
	float current_time = 0;
	float max_time = 300;
	float time_delta = 0.05f;
	std::vector<Player> players;
	std::vector<Target> targets;
public:
	Simulator(std::vector<Player>& players);
	float get_current_time();
	bool tick();

};