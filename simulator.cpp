#include "simulator.h"

Simulator::Simulator(std::vector<Player>& players) :
	players(players)
{
	//foreach player init one target
	for (unsigned int i = 0; i < players.size(); i++) {
		targets.push_back(Target());
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
		players.at(i).tick(time_delta, targets.at(i));
	}
	for (unsigned int i = 0; i < targets.size(); i++)
	{
		targets.at(i).tick(time_delta);
	}
	current_time += time_delta;
	return current_time > max_time;
}