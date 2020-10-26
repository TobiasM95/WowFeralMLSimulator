#pragma once

#include <vector>
#include "target.h"
#include "player.h"

class Simulator
{
private:
	float current_time = 0;
	float max_time = 300.0f;
	float time_delta = 0.1f;
	std::vector<Player> players;
public:
	//Simulator(std::vector<Player*> players, std::vector<Target*> targets);
	Simulator(float simulation_duration, std::vector<Player> players);
	void init_player_targets();
	float get_current_time();
	bool tick();

};