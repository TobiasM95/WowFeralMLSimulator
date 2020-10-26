#include <iostream>
#include <vector>
#include <ppl.h>
#include <chrono>

#include "player.h"
#include "simulator.h"
#include "target.h"
#include "random_gen.h"

void run_simulations_parallel(size_t, float);
void run_simulations_serial(size_t, float);

int main()
{
    std::cout << "Hello World! "<< rng_namespace::getRandom(0,20) << "  " << 
        rng_namespace::getChance() <<"\n";

    int num_runs = 1000;
    float simulation_duration = 300.0f;

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
            Simulator simulator(simulation_duration, players);
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
        Simulator simulator(simulation_duration, players);
        bool simulation_complete = false;
        do
        {
            simulation_complete = simulator.tick();
        } while (!simulation_complete);
        //if (i % (num_runs / 100) == 0)
        //    std::cout << "Run " << i << " of " << num_runs << " completed...\n";
    }
}