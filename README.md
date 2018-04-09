# Crossing Guard - Simple Daily Scheduler

## What it is
Crossing-Guard is a a simple console application, assisting in planning a daily task-assignment for a group of persons.

Crossing-Guard is based on project [beth](https://github.com/johsteffens/beth).

## How it works

### The Problem
Say there is a specific task that is to be performed daily or weekly on certain weekdays. For example: Patrolling the school crossing on all schooldays during rush hour. You've got a number of volunteers with individual available or preferred days. 

You job is to find a fair assignment observing all individual preferences.

### The Solution
You create two data files:
   * One contains information about the task. E.g. which days of the week, holidays, vacation-periods etc.
   * One contains a list of volunteers with individual preferences.
   
Crossing-Guard will take the the two files and calculate a fair assignment for all persons for a specified period. The result can be shown in different modes such as resolved by person, by date or nicely formatted as overview table to be printed and handed out.

## How to use it
**Linux (or operating systems supporting POSIX):** Just follow suggestions below.

**Windows:** [Set up a POSIX-environment first](https://github.com/johsteffens/beth/wiki/Requirements#how-to-setup-a-posix-environment-for-beth-on-windows). 

### Requirements/Dependencies
   * Project [beth](https://github.com/johsteffens/beth).
   * gcc (or similar compiler suite) supporting the C11 standard.
   * Library `pthread` of the POSIX.1c standard.

### Build
   * Clone [beth](https://github.com/johsteffens/beth) and [crossing_guard](https://github.com/johsteffens/crossing_guard):
      * `git clone https://github.com/johsteffens/beth.git`
      * `git clone https://github.com/johsteffens/crossing_guard.git`
      * *(Ensure both reprositories have the same root folder)*
   * In a terminal ...
      * enter folder crossing_guard/build: `cd crossing_guard/build`.
      * run: `make`. 
      * This creates the executale binary file `crossing_guard`.

## License
The source code in this repository is licensed under the [Apache 2.0 License](https://github.com/johsteffens/crossing_guard/blob/master/LICENSE).

## Motivation

I wrote this tool to help my wife with crossing-guard assignment for our local primary school.
