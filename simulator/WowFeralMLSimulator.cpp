#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ppl.h>
#include <chrono>

#include "../entity/player.h"
#include "simulator.h"
#include "../entity/target.h"
#include "../utils/random_gen.h"
#include "../utils/communication_handler.h"
#include "../utils/message.h"

int standard_test_runs();
void run_simulations_parallel(size_t, float);
void run_simulations_serial(size_t, float);
void write_logs_to_disk(std::string, Logger);

int main(int argc, char* argv[])
{
    //Set sim params
    int num_players = 1;
    int train_after_n_runs = 3;
    float simulation_duration = 420.0f;

    for (int i = 0; i < argc; i++)
    {
        auto arg = argv[i];
        std::cout << arg << "\n";
    }

    //Connect to python pipe server
    std::string pipeName = argv[argc - 1];
    std::shared_ptr<CommunicationHandler> comm_handler(new CommunicationHandler{});
    std::cout << "CONNECTING TO LOCAL PIPE: " + pipeName + "\n";
    const char* pipeID = pipeName.c_str();
    comm_handler->connect((LPCSTR)pipeID, 30);

    while (comm_handler->isConnected())
    {
        for (int i = 0; i < train_after_n_runs; i++)
        {
            std::vector<Player> players;
            for (int i = 0; i < num_players; i++)
            {
                Player p1(3.6f, 12.8f, 424, 156, 78, 32, 142, { 1,0,0,2,0,1,0 });
                players.push_back(p1);
            }
            Simulator simulator(simulation_duration, players, false, true, comm_handler);
            bool simulation_complete = false;
            do
            {
                //std::cout << "Do simulation tick" << "\n";
                simulation_complete = simulator.tick();
                //Sleep(1000);
            } while (!simulation_complete);
            simulator.update_transitions(1);
            //Push transitions to python
            simulator.push_transitions_to_python();
            std::cout << i + 1 << "/" << train_after_n_runs << " runs done!\n";
        }
        //Train python model and wait for epoch finish
        auto c_to_py_msg = Message("FRSST", "");
        comm_handler->sendMessage(&c_to_py_msg);
        auto py_to_c_msg = comm_handler->getMessage();
        if (py_to_c_msg.getHeader() == "FRSST")
            continue;
        else
            break;
    }
}

int standard_test_runs()
{
    std::cout << "Hello World! "<< rng_namespace::getRandom(0,20) << "  " << 
        rng_namespace::getChance() <<"\n";

    int num_runs = 1000;
    float simulation_duration = 420.0f;
    bool log_single_run = true;
    std::string log_path = "C:/Users/Tobi/Documents/Programming/MachineLearning/WowFeralML/Fight_Logs/";

#ifdef _DEBUG
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    run_simulations_serial(num_runs, simulation_duration);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << num_runs << " runs complete!\nTime elapse: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
#else
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    //run_simulations_parallel(num_runs, simulation_duration);
    run_simulations_serial(num_runs, simulation_duration);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << num_runs << " runs complete!\nTime elapse: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
#endif // _DEBUG

    //finally: perform single run and optionally log
    std::vector<Player> players;
    Player p1(3.6f, 12.8f, 424, 156, 78, 32, 142, { 1,0,0,2,0,1,0 });
    players.push_back(p1);
    Player p2(3.6f, 12.8f, 424, 156, 78, 32, 142, { 1,0,0,2,0,1,0 });
    players.push_back(p2);

    //initialize simulator
    Simulator simulator(simulation_duration, players, log_single_run, false);
    bool simulation_complete = false;
    do
    {
        simulation_complete = simulator.tick();
    } while (!simulation_complete);
    simulator.update_transitions(0);
    //Write logging information to disk:
    write_logs_to_disk(log_path, simulator.logger);
}

void run_simulations_parallel(size_t num_runs, float simulation_duration)
{
    //for (int i = 0; i < num_runs; i++)
    concurrency::parallel_for(size_t(0), num_runs, [&](size_t i)
        {
            //initialize player
            Player p1(3.6f, 12.8f, 424, 156, 78, 32, 142, { 1,0,0,2,0,1,0 });
            std::vector<Player> players;
            players.push_back(p1);

            //initialize simulator
            //Simulator simulator(players, targets);
            Simulator simulator(simulation_duration, players, false, false);
            bool simulation_complete = false;
            do
            {
                simulation_complete = simulator.tick();
            } while (!simulation_complete);
            //if (i % (num_runs / 100) == 0)
            //    std::cout << "Run " << i << " of " << num_runs << " completed...\n";
        });
}

void run_simulations_serial(size_t num_runs, float simulation_duration)
{
    //Both variants take the same amount of time
    for (size_t i = 0; i < num_runs; i++)
    {
        //initialize player
        Player p1(3.6f, 12.8f, 424, 156, 78, 32, 142, { 1,0,0,2,0,1,0 });
        std::vector<Player> players;
        players.push_back(p1);

        //initialize simulator
        Simulator simulator(simulation_duration, players, false, false);
        bool simulation_complete = false;
        do
        {
            simulation_complete = simulator.tick();
        } while (!simulation_complete);
    }
    
    /*
    //initialize player
    Player p1(3.6f, 12.8f, 424, 156, 78, 32, 142, { 1,0,0,2,0,1,0 });
    std::vector<Player> players;
    players.push_back(p1);

    //initialize simulator
    Simulator simulator(simulation_duration, players, false);
    bool simulation_complete = false;

    for (size_t i = 0; i < num_runs; i++)
    {
        simulation_complete = false;
        do
        {
            simulation_complete = simulator.tick();
        } while (!simulation_complete);
        simulator.reset(true);
    }
    */
}

void write_logs_to_disk(std::string path, Logger logger)
{
    //write dps logs
    int num_players = logger.dps_log.size();
    for (int i = 0; i < num_players; i++)
    {
        std::ofstream myfile;
        myfile.open(path + "fight_damage_player_" + std::to_string(i) + ".txt");
        myfile << "#timestep;player_" << i << "_damage";
        myfile << "\n";
        for (auto timestep : logger.dps_log.at(i))
        {
            std::ostringstream line;
            line.precision(2);
            line << std::fixed << timestep.at(0);
            for (size_t p = 1; p < timestep.size(); p++)
            {
                line << ";" << std::fixed << timestep.at(p);
            }
            line << "\n";
            myfile << line.str();
        }
        myfile.close();
    }
    //write buff log
    for (int i = 0; i < num_players; i++)
    {
        std::ofstream myfile;
        myfile.open(path + "buffs_player_" + std::to_string(i) + ".txt");
        myfile << "#timestep;player_" << i << "_buffs";
        myfile << "\n";
        for (auto timestep : logger.buff_log.at(i))
        {
            std::ostringstream line;
            line.precision(2);
            line << std::fixed << timestep.time << ";" << timestep.status_name << ";" << std::fixed << timestep.status_value << "\n";
            myfile << line.str();
        }
        myfile.close();
    }
    //write dot log
    for (int i = 0; i < num_players; i++)
    {
        std::ofstream myfile;
        myfile.open(path + "dots_player_" + std::to_string(i) + ".txt");
        myfile << "#timestep;player_" << i << "_target_dots";
        myfile << "\n";
        for (auto timestep : logger.dot_log.at(i))
        {
            std::ostringstream line;
            line.precision(2);
            line << std::fixed << timestep.time << ";" << timestep.status_name << ";" << std::fixed << timestep.status_value << "\n";
            myfile << line.str();
        }
        myfile.close();
    }
    //write event log
    for (int i = 0; i < num_players; i++)
    {
        std::ofstream myfile;
        myfile.open(path + "events_player_" + std::to_string(i) + ".txt");
        myfile << "#timestep;player_" << i << "_events";
        myfile << "\n";
        for (auto timestep : logger.event_log.at(i))
        {
            std::ostringstream line;
            line.precision(2);
            line << std::fixed << timestep.time << ";" << timestep.status_name << ";" << std::fixed << timestep.status_value << "\n";
            myfile << line.str();
        }
        myfile.close();
    }
    //write transition log
    for (int i = 0; i < num_players; i++)
    {
        std::ofstream myfile;
        myfile.open(path + "transitions_player_" + std::to_string(i) + ".txt");
        myfile << "#state;action;action_mask;reward;new_state";
        myfile << "\n";
        for (auto transition : logger.transition_log.at(i))
        {
            std::ostringstream line;
            line.precision(2);
            for (float sf : transition.state) 
            {
                line << std::fixed << sf << ";";
            }
            line << transition.action << ";";
            for (int ami : transition.valid_action_mask)
            {
                line << ami << ";";
            }
            line << std::fixed << transition.reward << ";";
            for (float nsf : transition.new_state)
            {
                line << std::fixed << nsf << ";";
            }
            myfile << line.str() << "\n";
        }
        myfile.close();
    }
}