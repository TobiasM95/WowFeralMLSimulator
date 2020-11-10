#pragma once

#include <map>
#include <vector>
#include <array>
#include "target.h"
#include "player.h"

class Logger
{
public:
	struct status_timestamp
	{
		float time;
		std::string status_name;
		float status_value = 0.0f;
	};
	std::map<int, std::vector<std::vector<float>>> dps_log;
	std::map<int, std::vector<status_timestamp>> buff_log;
	std::map<int, std::vector<status_timestamp>> dot_log;
	std::map<int, std::vector<status_timestamp>> event_log;

	void init_logger(std::vector<Player> players);
	void log_timestep(float current_time, std::vector<Player> players);
	void log_dps(float current_time, std::vector<Player> players);
	void log_buffs(float current_time, std::vector<Player> players);
	void log_dots(float current_time, std::vector<Player> players);
	void log_events(float current_time, Skill& s, float dmg, bool crit, bool is_dot);

};

class Simulator
{
private:
	float current_time = 0;
	float max_time = 300.0f;
	float time_delta = 0.1f;
	std::vector<Player> players;
public:
	bool log_events = false;
	Logger logger;

	Simulator(float simulation_duration, std::vector<Player> players, bool log_events);
	void init_player_targets();
	float get_current_time();
	bool tick();

};