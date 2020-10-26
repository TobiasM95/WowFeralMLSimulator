#include "simulator.h"

//Works, don't use pointers since you have to do lifetime management, which sucks..
Simulator::Simulator(float simulation_duration, std::vector<Player> players) :
	max_time(simulation_duration), players(players)
{
}

void Simulator::init_player_targets()
{
	//Note: Here could be multitarget initialization in the future
	for (Player& p : players)
	{
		Target t;
		p.target = t;
	}
}

float Simulator::get_current_time()
{
	return current_time;
}

bool Simulator::tick()
{
	//TODO: design a combat log system
	if (current_time > max_time) {
		return true;
	}
	for (unsigned int i = 0; i < players.size(); i++)
	{
		players.at(i).tick(time_delta);
	}
	current_time += time_delta;
	return current_time > max_time;
}