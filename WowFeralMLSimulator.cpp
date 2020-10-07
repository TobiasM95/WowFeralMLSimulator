// WowFeralMLSimulator.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>
#include <vector>

#include "player.h"
#include "simulator.h"

int main()
{
    std::cout << "Hello World!\n";
    //initialize player
    Player p1(15, 329, 0.2209f, 0.1223f, 0.301f, 0.0836f);
    std::vector<Player> players;
    players.push_back(p1);
    //initialize simulator
    for (int i = 0; i < 1000; i++)
    {
        Simulator simulator(players);
        bool simulation_complete = false;
        do
        {
            simulation_complete = simulator.tick();
        } while (!simulation_complete);
    }
    std::cout << "1000 runs complete!\n";
}
