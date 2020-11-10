#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ppl.h>
#include <chrono>

#include "player.h"
#include "simulator.h"
#include "target.h"
#include "random_gen.h"

void run_simulations_parallel(size_t, float);
void run_simulations_serial(size_t, float);
void write_logs_to_disk(std::string, Logger);

int main()
{
    std::cout << "Hello World! "<< rng_namespace::getRandom(0,20) << "  " << 
        rng_namespace::getChance() <<"\n";

    int num_runs = 10000;
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
    run_simulations_parallel(num_runs, simulation_duration);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << num_runs << " runs complete!\nTime elapse: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
#endif // _DEBUG

    //finally: perform single run and optionally log
    std::vector<Player> players;
    Player p1(12.8f, 424, 156, 78, 32, 142, { 1,0,0,2,0,1,0 });
    players.push_back(p1);
    Player p2(12.8f, 424, 156, 78, 32, 142, { 1,0,0,2,0,1,0 });
    players.push_back(p2);

    //initialize target
    //Target t1;
    //std::vector<Target*> targets;
    //targets.push_back(&t1);

    //initialize simulator
    //Simulator simulator(players, targets);
    Simulator simulator(simulation_duration, players, log_single_run);
    bool simulation_complete = false;
    do
    {
        simulation_complete = simulator.tick();
    } while (!simulation_complete);
    //Write logging information to disk:
    write_logs_to_disk(log_path, simulator.logger);
}

void run_simulations_parallel(size_t num_runs, float simulation_duration)
{
    //for (int i = 0; i < num_runs; i++)
    concurrency::parallel_for(size_t(0), num_runs, [&](size_t i)
        {
            //initialize player
            Player p1(12.8f, 424, 156, 78, 32, 142, { 1,0,0,2,0,1,0 });
            std::vector<Player> players;
            players.push_back(p1);

            //initialize target
            //Target t1;
            //std::vector<Target*> targets;
            //targets.push_back(&t1);

            //initialize simulator
            //Simulator simulator(players, targets);
            Simulator simulator(simulation_duration, players, false);
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
    for (size_t i = 0; i < num_runs; i++)
    {
        //initialize player
        Player p1(12.8f, 424, 156, 78, 32, 142, { 1,0,0,2,0,1,0 });
        std::vector<Player> players;
        players.push_back(p1);

        //initialize target
        //Target t1;
        //std::vector<Target*> targets;
        //targets.push_back(&t1);

        //initialize simulator
        //Simulator simulator(players, targets);
        Simulator simulator(simulation_duration, players, false);
        bool simulation_complete = false;
        do
        {
            simulation_complete = simulator.tick();
        } while (!simulation_complete);
        //if (i % (num_runs / 100) == 0)
        //    std::cout << "Run " << i << " of " << num_runs << " completed...\n";
    }
}

void write_logs_to_disk(std::string path, Logger logger)
{
    //write dps logs
    int num_players = logger.dps_log.size();
    for (int i = 0; i < num_players; i++)
    {
        std::ofstream myfile;
        myfile.open(path + "fight_damage_player_" + std::to_string(i) + ".txt");
        myfile << "timestep;player_" << i << "_damage";
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
        myfile << "timestep;player_" << i << "_buffs";
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
        myfile << "timestep;player_" << i << "_target_dots";
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
        myfile << "timestep;player_" << i << "_events";
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
}