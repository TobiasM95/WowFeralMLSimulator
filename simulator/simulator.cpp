#include "simulator.h"
#include "../skills/skill.h"
#include "../utils/communication_handler.h"

//Works, don't use pointers since you have to do lifetime management, which sucks..
Simulator::Simulator(float simulation_duration, std::vector<Player> players, bool log_events, bool log_transitions) :
	Simulator(simulation_duration, players, log_events, log_transitions, nullptr)
{
}

Simulator::Simulator(
	float simulation_duration, 
	std::vector<Player> players, 
	bool log_events, 
	bool log_transitions, 
	std::shared_ptr<CommunicationHandler> comm_handler) :
	max_time(simulation_duration), players(players), log_events(log_events), log_transitions(log_transitions),
	comm_handler(comm_handler)
{
	for (size_t i = 0; i < this->players.size(); i++)
	{
		this->players.at(i).id = i;
	}
	init_player_targets();
	if (log_events || log_transitions)
	{
		logger.init_logger(this->players);
	}
}

void Simulator::reset(bool reset_logger)
{
	for(Player &p : players)
	{
		p.reset();
		p.target->reset();
	}
	current_time = 0;
	if (log_events && reset_logger)
	{
		logger.reset();
	}
}

void Simulator::init_player_targets()
{
	//Note: Here could be multitarget initialization in the future
	for (Player& p : players)
	{
		std::shared_ptr<Target> t(new Target { this });
		p.target = t;
	}
}

float Simulator::get_current_time()
{
	return current_time;
}

bool Simulator::tick()
{
	if (current_time > max_time) {
		return true;
	}
	for (unsigned int i = 0; i < players.size(); i++)
	{
		players.at(i).tick(time_delta);
	}
	if (log_events)
	{
		logger.log_timestep(current_time, players);
	}
	current_time += time_delta;
	return current_time > max_time;
}

void Simulator::update_transitions(int mode)
{
	switch (mode) {
	case 0: {
		for (Player p : players)
		{
			logger.update_transitions(p.id, p.target->received_dmg / p.target->fight_duration);
		}
	}break;
	case 1: {
		for (Player p : players)
		{
			//Hardcoded normalized dps value with approximate dps max
			logger.update_transitions(p.id, p.target->received_dmg / p.target->fight_duration / 2000.0f);
		}
	}break;
	}
		
}

int Simulator::piped_model_select_action(std::vector<float> state, std::vector<int> valid_action_mask)
{
	std::stringstream state_string;
	state_string << state.size() << ";" << valid_action_mask.size() << ";";
	for (float f : state)
	{
		state_string << f << ";";
	}
	for (int i : valid_action_mask)
	{
		state_string << i << ";";
	}
	//std::cout << "C: Send state string to pipe: " << state_string.str() << "\n";
	auto c_to_py_msg = Message(FERAL_SIM_STATE_MESSAGE_HEADER, state_string.str());
	comm_handler->sendMessage(&c_to_py_msg);
	auto py_to_c_msg = comm_handler->getMessage();
	if (py_to_c_msg.getHeader() == FERAL_SIM_AGENT_ACTION_IMMEDIATE_RESPONSE_MESSAGE_HEADER)
		return std::stoi(py_to_c_msg.getBody());
	else
		return -1;
}

void Simulator::push_transitions_to_python()
{
	int num_transitions = 0;
	for (auto const& x : logger.transition_log)
	{
		num_transitions += x.second.size();
	}
	std::stringstream tss;
	for (auto const& player_transitions : logger.transition_log)
	{
		for (auto transition : player_transitions.second)
		{
			for (float sf : transition.state)
			{
				tss << sf << ";";
			}
			tss << transition.action << ";";
			for (int ami : transition.valid_action_mask)
			{
				tss << ami << ";";
			}
			tss << transition.reward << ";";
			for (size_t i = 0; i < transition.new_state.size() - 1; i++)
			{
				tss << transition.new_state.at(i) << ";";
			}
			tss << transition.new_state.at(transition.new_state.size() - 1) <<"\n";
		}
	}
	auto c_to_py_msg = Message(FERAL_SIM_RESET_GAME_STATE_MESSAGE_HEADER, tss.str());
	comm_handler->sendMessage(&c_to_py_msg);
	auto py_to_c_msg = comm_handler->getMessage();
	if (py_to_c_msg.getHeader() == FERAL_SIM_RESET_GAME_STATE_MESSAGE_HEADER)
		return;
	//Note: Maybe some error handling
}

void Logger::init_logger(std::vector<Player> players)
{
	for (auto p : players)
	{
		dps_log.insert(std::pair<int, std::vector<std::vector<float>> >(p.id, std::vector<std::vector<float>>()));
		buff_log.insert(std::pair<int, std::vector<status_timestamp> >(p.id, std::vector<status_timestamp>()));
		dot_log.insert(std::pair<int, std::vector<status_timestamp> >(p.id, std::vector<status_timestamp>()));
		event_log.insert(std::pair<int, std::vector<status_timestamp> >(p.id, std::vector<status_timestamp>()));
		transition_log.insert(std::pair<int, std::vector<ml_datapoint> >(p.id, std::vector<ml_datapoint>()));
	}
}

void Logger::reset()
{
	for (size_t i = 0; i < dps_log.size(); i++)
	{
		dps_log.at(i).clear();
		buff_log.at(i).clear();
		dot_log.at(i).clear();
		event_log.at(i).clear();
		transition_log.at(i).clear();
	}
}

void Logger::log_timestep(float current_time, std::vector<Player> players)
{
	log_dps(current_time, players);
	log_buffs(current_time, players);
	log_dots(current_time, players);
	//Events get registered automatically
}

void Logger::log_dps(float current_time, std::vector<Player> players)
{
	for (auto p : players)
	{
		std::vector<float> curr_dps = { current_time, p.target->received_dmg };
		dps_log.at(p.id).push_back(curr_dps);
	}
}

void Logger::log_buffs(float current_time, std::vector<Player> players)
{
	for (auto p : players)
	{
		status_timestamp curr_buffs = { current_time, p.cat_form?"cat_form":"moonkin_form" };
		buff_log.at(p.id).push_back(curr_buffs);
		for (Buff s : p.stat_buffs)
		{
			status_timestamp curr_buffs = { current_time, s.name , s.duration};
			buff_log.at(p.id).push_back(curr_buffs);
		}
		for (Buff s : p.skill_buffs)
		{
			status_timestamp curr_buffs = { current_time, s.name , s.duration };
			buff_log.at(p.id).push_back(curr_buffs);
		}
	}
}

void Logger::log_dots(float current_time, std::vector<Player> players)
{
	for (auto p : players)
	{
		for (std::shared_ptr<Dot> d : p.target->active_dots)
		{
			status_timestamp curr_dots = { current_time, d->name , d->duration_left};
			dot_log.at(p.id).push_back(curr_dots);
		}
	}
}

void Logger::log_events(float current_time, Skill& s, float dmg, bool crit, bool is_dot)
{
	std::string event_name = s.name;
	if (is_dot)
		event_name += "_tick";
	if (crit)
		event_name += "_crit";
	status_timestamp curr_event =
	{
		current_time,
		event_name,
		dmg
	};
	event_log.at(s.player->id).push_back(curr_event);
}

void Logger::update_transitions(int id, float reward)
{
	for (ml_datapoint& dp : transition_log.at(id))
	{
		dp.reward = reward;
	}
}

void Logger::log_transitions(
	int id, std::vector<float> state, int action, std::vector<int> valid_action_mask,
	float reward, std::vector<float> new_state
)
{
	ml_datapoint dp = { state, action, valid_action_mask, reward, new_state };
	transition_log.at(id).push_back(dp);
}