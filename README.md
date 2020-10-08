# WoW Machine Learning Feral Simulator

## About

This project is a terminal based World of Warcraft (WoW) simulator that simulates a (for now) single target fight from the point of view of a feral druid.
The simulation not only simulates a fight as close as possible to the base game (Shadowlands expansion) but it also includes a machine learning interface, i.e. for every simulation step a state vector and a valid action mask can be requested and an action can be input via a simple action vector.

This project was started because the pre-existing WoW simulator [SimulationCraft](https://www.simulationcraft.org/) was too extensive and complex for my basic C++ knowledge to modify and implement a machine learning interface. I decided to write up a feral druid (my main for a long time) simulator from scratch which does not need any functionality beyond that. It also should offer a simple machine learning interface for me to extract fight information and action masks to train an agent via different reinforcement learning algorithms.

## Disclaimer

My C++ skills are basic at best so be wary of bugs, bad design or other problems that could arise

## Features

_Features listed here are currently work in progress!_

* Terminal based feral druid simulation with adjustable input parameters such as fight duration
* Feature vector extraction at every timestep
* Valid action mask extraction at every timestep
* Log output to text (or json or other data) files to check results or generate training data
