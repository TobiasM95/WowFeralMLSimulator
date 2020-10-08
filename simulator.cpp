#include "simulator.h"
#include "target.h"
#include "player.h"

//works
/*Simulator::Simulator(std::vector<Player*> players, std::vector<Target*> targets) :
	players(players), targets(targets)
{
}*/

//Works TODO: Implement deletion function since target leaks memory
Simulator::Simulator(std::vector<Player*> players) :
	players(players)
{
	//foreach player init one target
	for (unsigned int i = 0; i < players.size(); i++) {
		Target* a = new Target();
		targets.push_back(a);
	}
}

//Does not work
/*Simulator::Simulator(std::vector<Player*> players) :
	players(players)
{
	//foreach player init one target
	for (unsigned int i = 0; i < players.size(); i++) {
		Target a;
		targets.push_back(&a);
	}
}*/

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
		players.at(i)->tick(time_delta, targets.at(i));
	}
	for (unsigned int i = 0; i < targets.size(); i++)
	{
		targets.at(i)->tick(time_delta);
	}
	current_time += time_delta;
	return current_time > max_time;
}