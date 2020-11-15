#pragma once

#include <map>
#include <vector>
#include <array>
#include "../entity/target.h"
#include "../entity/player.h"

class Logger
{
public:
	struct status_timestamp
	{
		float time;
		std::string status_name;
		float status_value = 0.0f;
	};
	struct ml_datapoint
	{
		std::vector<float> state;
		int action;
		std::vector<int> valid_action_mask;
		float reward;
		std::vector<float> new_state;
	};
	std::map<int, std::vector<std::vector<float>>> dps_log;
	std::map<int, std::vector<status_timestamp>> buff_log;
	std::map<int, std::vector<status_timestamp>> dot_log;
	std::map<int, std::vector<status_timestamp>> event_log;
	std::map<int, std::vector<ml_datapoint>> transition_log;

	void init_logger(std::vector<Player> players);
	void reset();
	void log_timestep(float current_time, std::vector<Player> players);
	void log_dps(float current_time, std::vector<Player> players);
	void log_buffs(float current_time, std::vector<Player> players);
	void log_dots(float current_time, std::vector<Player> players);
	void log_events(float current_time, Skill& s, float dmg, bool crit, bool is_dot);
	void log_transitions(
		int id, std::vector<float> state, int action, std::vector<int> valid_action_mask,
		float reward, std::vector<float> new_state
	);
	void update_transitions(int id, float reward);
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
	void reset(bool reset_logger);
	void init_player_targets();
	float get_current_time();
	bool tick();

	void update_transitions(int mode);
};