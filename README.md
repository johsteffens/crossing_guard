# Crossing Guard - Simple Daily Scheduler

## What it is
Crossing-Guard is a a simple console application, assisting in planning a daily task-assignment for a group of persons.

Crossing-Guard is based on project [beth](https://github.com/johsteffens/beth).

## How it works

### The Problem
Say there is a specific task that is to be performed daily or weekly on certain weekdays by one person. For example: Patrolling the school crossing on all schooldays during rush hour. You've got a number of volunteers with individual perferences/availabilities. Those people are to be assigned fairly observing indifidual preferences.

### The Solution
Create two data files:
   * One contains information about the task. E.g. which days of the week, holidays, vacation-periods etc.
   * One contains a list of volunteers with individual preferences.
   
Crossing-Guard will take the the two files and calculate the assignment for all persons for a specified period. The result can be shown in different modes such as resolved by person, by date or nicely formatted as overview table to be printed and handed out.

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

### Example
Folder `example` contains an example configuration with some phantasy-names.
   * config.txt contains information about schooldays, vacation and holidays.
   * assignment.txt contains information about volunteers and theyr preferences.
Both text files are writton in beth-markup-format, which is a editable markup syntax used for data serialization. It has some resemblence to XML and is hopefully easy enough to understand and edit. The files contain explanatory inline comments.
   
   * Run `crossing_guard`
      * This gives you a brief overview of avilable options.
      
   * Run `crossing_guard config.txt assignment.txt 01.01.2018 31.03.2018 -format persons`
      * This will output the assignment in text form.
      
   * Run `crossing_guard config.txt assignment.txt 01.01.2018 31.03.2018 -format html_table > table.html`
      * This will produce the html-table: table.html. (Currently that format is available only in German language).

## License
The source code in this repository is licensed under the [Apache 2.0 License](https://github.com/johsteffens/crossing_guard/blob/master/LICENSE).

## Motivation

I wrote this tool to help my wife with crossing-guard assignment for our local primary school. However, it is not limited to just that type of assignment task.
