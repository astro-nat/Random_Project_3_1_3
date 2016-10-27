/* --------------------------------------------------------------------------- */
/* Developer: Andrew Kirfman                                                   */
/* Project: CSCE-313 Machine Problem #1                                        */
/*                                                                             */
/* File: ./MP6/main.cpp                                                        */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/* Standard Library Includes                                                   */
/* --------------------------------------------------------------------------- */

#include<iostream>
#include<sched.h>
#include<vector>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<time.h>
#include<math.h>

/* --------------------------------------------------------------------------- */
/* User Defined Includes                                                       */
/* --------------------------------------------------------------------------- */

#include "utility.h"

/* --------------------------------------------------------------------------- */
/* Global Variables                                                            */
/* --------------------------------------------------------------------------- */

std::vector<pid_t> cpu_bound;
std::vector<pid_t> io_bound;

/* 
 * This machine problem will introduce you to the function of the operating system's scheduler and how different
 * processes are affected by various scheduling policies.  Additionally, it will also serve as a practical visualization
 * of the scheduling policies covered in lecture and those policies that are actually implemented in real UNIX/UNIX Like systems.  
 *
 * Two fundamentally different process types will be used to demonstrate the relative strengths and weaknesses inherrent in
 * the variety of different scheduling algorithms persented here: cpu bound and i/o bound processes.  CPU bound processes perform
 * tasks that require, almost exclusively, the use of the CPU/main memory and no other components of the system (hard disk, network,
 * etc..).  As such, whenever a CPU bound process is running, the CPU itself is, in general, completing an instruction every clock
 * cycle.  (not counting unavoidable stalls for pipeline hazards, stalls from cache misses, or other data hazards)  This means that 
 * the CPU never wastes time and is always busy performing useful work.  
 *
 * Examples of both CPU and I/O bound processes are provided below.  For this assignment, you must implement 5 CPU bound functions and
 * 5 I/O bound functions all of your choice.  The input parameters for these programs must be chosen beforehand such that each process
 * will run for a reasonable, finite amount of time.  Also, you should stagger the expected runtimes of each function.  
 *
 * CPU Bound Processes:
 *  - Satisfiability problem
 *  - Cryptarithm Solver
 *  - Breadth-First Search/Depth-First Search for something
 *  - Seive of (whatever his name is)
 *  - Matrix multiplication
 *  - A for loop that increments a number (tight loop)
 */

void cleanup()
{
    /* Clean up CPU bound processes */
    for(unsigned short int i=0; i<cpu_bound.size(); i++)
    {
        kill(cpu_bound[i], SIGKILL);
    }

    /* Clean up I/O bound processes */
    for(unsigned short int i=0; i<io_bound.size(); i++)
    {
        kill(io_bound[i], SIGKILL);
    }

    /* Clean up temporary directories */
    system("rm -rf ./cpu_bound");
    system("rm -rf ./io_bound");

    exit(255);
}

void handle_signals(int signum)
{
    cleanup();
}


int main()
{
    /* Define signal handlers to clean up from exceptions */
    /* 
     * Note: It would disastrous for any of the cpu intensive processes to 
     * continue running in the event that something goes wrong.  Make absolutely
     * sure that they will be cleaned up by the machine.  
     */
    struct sigaction signal_struct;
    signal_struct.sa_handler = handle_signals;
    sigaction(SIGINT, &signal_struct, NULL);
    sigaction(SIGQUIT, &signal_struct, NULL);
    sigaction(SIGILL, &signal_struct, NULL);
    sigaction(SIGABRT, &signal_struct, NULL);
    sigaction(SIGFPE, &signal_struct, NULL);
    sigaction(SIGSEGV, &signal_struct, NULL);
    sigaction(SIGTERM, &signal_struct, NULL);

    std::cout << std::endl;
    std::cout << BOLDGREEN << "/* -------------------------------------------------------------------------- */" << RESET << std::endl;
    std::cout << BOLDGREEN << "/* Setup                                                                      */" << RESET << std::endl;
    std::cout << BOLDGREEN << "/* -------------------------------------------------------------------------- */" << RESET << std::endl;
    std::cout << std::endl;

    /* Reused Variables */
    pid_t pid = 0;
    int return_val = 0;
    struct sched_param scheduling_struct;
    std::string system_string = "";

    /* Clear old occurrences of the temp directories */
    return_val = system("rm -rf ./cpu_bound 2>&1 > /dev/null");
    if(return_val == -1)
    {
        std::cout << "[ERROR]: Cound not remove old cpu_bound directory." << std::endl;
    }

    return_val = system("rm -rf ./cpu_bound 2>&1 > /dev/null");
    if(return_val == -1)
    {
        std::cout << "[ERROR]: Cound not remove old cpu_bound directory." << std::endl;
    }

    /* Make a directory to write executable files to */
    mkdir("./cpu_bound", S_IRWXU | S_IRWXG | S_IRWXO);
    mkdir("./io_bound", S_IRWXU | S_IRWXG | S_IRWXO);

    /* Start CPU bound processes */
    std::cout << "Starting CPU bound processes." << std::endl;
    
    for(unsigned short int i=1; i<=5; i++)
    {
        system_string = "g++ -std=c++11 ./cpu_bound_processes/cpu_bound_" + std::to_string(i) + ".cpp -o ./cpu_bound/cpu_" + std::to_string(i);
        return_val = system(system_string.c_str());
        if(return_val == -1)
        {
            std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Could not create CPU_" << std::to_string(i) << std::endl
                << "    " << strerror(errno) << std::endl;
            cleanup();
        }
        else
        {
            pid = fork();

            if(pid == 0)
            {
                system_string = "./cpu_bound/cpu_" + std::to_string(i);
                return_val = execlp(system_string.c_str(), system_string.c_str(), NULL);
            }
            else if(pid == -1)
            {
                std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Could not create CPU_" << std::to_string(i) << std::endl
                    << "    " << strerror(errno) << std::endl;
                cleanup();
            }
            else
            {
                cpu_bound.push_back(pid);
                std::cout << "  [" << BOLDBLUE << "SUCCESS" << RESET << "]: Created CPU_" << std::to_string(i) << std::endl;
            }
        }
    }
    std::cout << std::endl;

    /* Start I/O bound processes */
    std::cout << "Starting I/O bound processes." << std::endl;

    for(unsigned short int i=1; i<=5; i++)
    {
        system_string = "g++ --std=c++11 ./io_bound_processes/io_bound_" + std::to_string(i) + ".cpp -o ./io_bound/io_" + std::to_string(i);
        return_val = system(system_string.c_str());
        if(return_val == -1)
        {
            std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Couild not create IO_" << std::to_string(i) << std::endl
                << "    " << strerror(errno) << std::endl;
        }
        else
        {
            pid = fork();

            if(pid == 0)
            {
                system_string = "./io_bound/io_" + std::to_string(i);
                execlp(system_string.c_str(), system_string.c_str(), NULL);
            }
            else if(pid == -1)
            {
                std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Could not create IO_" << std::to_string(i) << std::endl
                    << "    " << strerror(errno) << std::endl;
                cleanup();
            }
            else
            {
                io_bound.push_back(pid);
                std::cout << "  [" << BOLDBLUE << "SUCCESS" << RESET << "]: CREATED IO_" << std::to_string(i) << std::endl;
            }
        }
    }
    std::cout << std::endl;

    /* ------------------------------------------------------------------------- */
    /* Round Robbin                                                              */
    /* ------------------------------------------------------------------------- */
    
    std::cout << std::endl;
    std::cout << BOLDGREEN << "/* -------------------------------------------------------------------------- */" << RESET << std::endl;
    std::cout << BOLDGREEN << "/* Round Robin                                                                */" << RESET << std::endl;
    std::cout << BOLDGREEN << "/* -------------------------------------------------------------------------- */" << RESET << std::endl;
    std::cout << std::endl;

    /* Set scheduling priority for cpu-bound processes */
    int max_priority = sched_get_priority_max(SCHED_RR);

    // Make sure that the parent process is at the highest priority!
    scheduling_struct.sched_priority = max_priority;
    return_val = sched_setscheduler(getpid(), SCHED_RR, &scheduling_struct);
    if(return_val == -1)
    {
        std::cout << "[" << BOLDRED << "ERROR" << RESET << "]: Could not properly set scheduling priority for parent process." << std::endl
            << "    " << strerror(errno) << std::endl;
        cleanup();
    }
    else
    {
        std::cout << "[" << BOLDBLUE << "SUCCESS" << RESET << "]: Set the parent's scheduling process to Round Robin." << std::endl;
    }
    std::cout << std::endl;

    // Now also set the children
    scheduling_struct.sched_priority = max_priority - 1;

    std::cout << "Setting Child scheduling policies." << std::endl;
    for(unsigned short int i=1; i<=5; i++)
    {
        return_val = sched_setscheduler(cpu_bound[i-1], SCHED_RR, &scheduling_struct);
        if(return_val == -1)
        {
            std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Could not set scheduling priority for cpu_" << std::to_string(i) << "." << std::endl
                << "      " << strerror(errno) << std::endl;
            cleanup();
        }
        else
        {
            std::cout << "  [" << BOLDBLUE << "SUCCESS" << RESET << "]: Set CPU_" << std::to_string(i) << " scheduling policy to round robin." << std::endl;     
        }
    }
    std::cout << std::endl;

    /* Set scheduling priority for I/O bound processes */
    for(unsigned short int i=1; i<=5; i++)
    {
        return_val = sched_setscheduler(io_bound[i-1], SCHED_RR, &scheduling_struct);
        if(return_val == -1)
        {
            std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Could not set scheduling priority for IO_" << std::to_string(i) << "." << std::endl
                << "      " << strerror(errno) << std::endl;
            cleanup();
        }
        else
        {
            std::cout << "  [" << BOLDBLUE << "SUCCESS" << RESET << "]: Set IO_" << std::to_string(i) << " scheduling policy to round robin." << std::endl;
        }
    }
    std::cout << std::endl;

    /* Keep time during program runs */
    struct timespec run_time;
    struct timespec snapshot_time;
    clock_gettime(CLOCK_MONOTONIC, &run_time);
    time_t seconds;
    long nanoseconds;

    /* Start the CPU bound processes */
    struct timespec rr_time;
    return_val = sched_rr_get_interval(cpu_bound[0], &rr_time);

    std::cout << "Starting cpu bound processes in order from 1 through 5." << std::endl;
    if(return_val != -1)
    {
        std::cout << "  Round robin time quantum: " << std::endl
            << "    - seconds: " << rr_time.tv_sec << std::endl
            << "    - nanoseconds: " << rr_time.tv_nsec << std::endl
            << std::endl;
    }
    else
    {
        std::cout << "[ERROR]: Could not determine round robin time quantum." << std::endl
            << "    " << strerror(errno) << std::endl;
    }
    for(unsigned short int i=0; i<cpu_bound.size(); i++)
    {
        return_val = kill(cpu_bound[i], SIGCONT);

        if(return_val == -1)
        {
            std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Could not wake up CPU_" << std::to_string(i + 1) << "." << std::endl
                << "    " << strerror(errno) << std::endl;
            cleanup();
        }

        // Ensure that the processes are scheduled in the right order 
        // The sleep is a slightly flawed way of making sure that all processes
        // receive their designated signals in the right order
        usleep(10000);
    }

    unsigned short int wait_count = 0;
    while(wait_count < cpu_bound.size())
    {
        for(unsigned short int i=0; i<cpu_bound.size(); i++)
        {
            return_val = waitpid(cpu_bound[i], NULL, WNOHANG);
            if(return_val != 0 && return_val != -1)
            {
                std::cout << "  [" << BOLDBLUE << "SUCCESS" << RESET << "]: Process " << std::to_string(i + 1) << " completed!  ";
                clock_gettime(CLOCK_MONOTONIC, &snapshot_time);
                seconds = snapshot_time.tv_sec - run_time.tv_sec;
                if(run_time.tv_nsec > snapshot_time.tv_nsec)
                {
                    nanoseconds = snapshot_time.tv_nsec + (1*pow(10, 9) - run_time.tv_nsec);
                    seconds -= 1;
                }
                else
                {
                    nanoseconds = snapshot_time.tv_nsec - run_time.tv_nsec;
                }
                std::cout << " Finish Time: " << seconds << " seconds, " << nanoseconds << " nanoseconds." << std::endl;
                wait_count += 1;
            }
            usleep(100000);
        }
    }
    std::cout << std::endl;

    /* Start I/O-bound processes */
    std::cout << "Starting io bound processes in order from 1 through 5." << std::endl;
    clock_gettime(CLOCK_MONOTONIC, &run_time);

    rr_time.tv_sec = 0;
    rr_time.tv_nsec = 0;
    return_val = sched_rr_get_interval(io_bound[0], &rr_time);
    
    if(return_val != -1)
    {
        std::cout << "  Round robin time quantum: " << std::endl
            << "    - seconds: " << rr_time.tv_sec << std::endl
            << "    - nanoseconds: " << rr_time.tv_nsec << std::endl
            << std::endl;
    }
    else
    {
        std::cout << "[" << BOLDRED << "ERROR" << RESET << "]: Could not determine round robin time quantum." << std::endl
            << std::endl;
    }
    for(unsigned short int i=0; i<io_bound.size(); i++)
    {
        return_val = kill(io_bound[i], SIGCONT);
        if(return_val == -1)
        {
            std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Could not wake up io_" << std::to_string(i + 1);
            cleanup();
        }

        usleep(10000);
    }

    wait_count = 0;
    while(wait_count < io_bound.size())
    {
        for(unsigned short int i=0; i<io_bound.size(); i++)
        {
            return_val = waitpid(io_bound[i], NULL, WNOHANG);
            if(return_val != 0 && return_val != -1)
            {
                std::cout << "  [" << BOLDBLUE << "SUCCESS" << RESET << "]: Process " << std::to_string(i + 1) << " completed!  ";
                clock_gettime(CLOCK_MONOTONIC, &snapshot_time);
                seconds = snapshot_time.tv_sec - run_time.tv_sec;
                if(run_time.tv_nsec > snapshot_time.tv_nsec)
                {
                    nanoseconds = snapshot_time.tv_nsec + (1*pow(10, 9) - run_time.tv_nsec);
                    seconds -= 1;
                }
                else
                {
                    nanoseconds = snapshot_time.tv_nsec - run_time.tv_nsec;
                }
                std::cout << " Finish Time: " << seconds << " seconds, " << nanoseconds << " nanoseconds." << std::endl;

                wait_count += 1;
            }
            usleep(100000);
        }
    }
 
    /* Clean up at the very end of round robin */
    cpu_bound.clear();
    io_bound.clear();

    /* ------------------------------------------------------------------------- */
    /* First In First Out                                                        */
    /* ------------------------------------------------------------------------- */
 
    /* Remember Reused Variables */
    // pid_t pid = 0;
    // int return_val = 0;
    // struct sched_param scheduling_struct;
    // std::string system_string = "";

    std::cout << std::endl;
    std::cout << BOLDGREEN << "/* -------------------------------------------------------------------------- */" << RESET << std::endl;
    std::cout << BOLDGREEN << "/* First In First Out                                                         */" << RESET << std::endl;
    std::cout << BOLDGREEN << "/* -------------------------------------------------------------------------- */" << RESET << std::endl;
    std::cout << std::endl;

    /* Create CPU Bound Processes */
    std::cout << "Starting CPU bound proccesses." << std::endl;
    for(unsigned short int i=1; i<=5; i++)
    {
        pid = fork();

        if(pid == 0)
        {
            system_string = "./cpu_bound/cpu_" + std::to_string(i);
            execlp(system_string.c_str(), system_string.c_str(), NULL);
        }
        else if(pid == -1)
        {
            std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Could not create CPU_" << std::to_string(i) << std::endl
                << "    " << strerror(errno) << std::endl;
            cleanup();
        }
        else
        {
            std::cout << "  [" << BOLDBLUE << "SUCCESS" << RESET << "]: Created CPU_" << std::to_string(i) << std::endl;
            cpu_bound.push_back(pid);
        }
    }
    std::cout << std::endl;

    /* Create I/O Bound Processes */
    std::cout << "Starting I/O bound processes." << std::endl;
    for(unsigned short int i=1; i<=5; i++)
    {
        pid = fork();

        if(pid == 0)
        {
            system_string = "./io_bound/io_" + std::to_string(i);
            execlp(system_string.c_str(), system_string.c_str(), NULL);
        }
        else if(pid == -1)
        {
            std::cout << "[" << BOLDRED << "ERROR" << RESET << "]: Could not create IO_" << std::to_string(i) << std::endl
                << "    " << strerror(errno) << std::endl;
        }
        else
        {
            std::cout << "  [" << BOLDBLUE << "SUCCESS" << RESET << "]: CREATED IO_" << std::to_string(i) << std::endl;
            io_bound.push_back(pid);
        }
    }
    std::cout << std::endl;

    /* Set scheduling priority to fifo for CPU bound processes */
    
    // Note: There is no need to worry about a priority or niceness value here.  
    // First in first out only cares about the order in which the processes arrive.  
    scheduling_struct.sched_priority = sched_get_priority_max(SCHED_FIFO);
    
    return_val = sched_setscheduler(getpid(), SCHED_FIFO, &scheduling_struct);
    if(return_val == -1)
    {
        std::cout << "[" << BOLDRED << "ERROR" << RESET << "]: Could not properly set scheduling priority for parent process." << std::endl
            << "    " << strerror(errno) << std::endl;
        cleanup();
    }
    else
    {
        std::cout << "[" << BOLDBLUE << "SUCCESS" << RESET << "]: Set the parent's scheduling policy to FIFO." << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Setting child scheduling policies." << std::endl;
    scheduling_struct.sched_priority = scheduling_struct.sched_priority - 1;
    for(unsigned short int i=0; i<cpu_bound.size(); i++)
    {
        return_val = sched_setscheduler(cpu_bound[i], SCHED_FIFO, &scheduling_struct);

        if(return_val == -1)
        {
            std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Could not set scheduling policy for CPU_" << std::to_string(i) << "." << std::endl
                << "    " << strerror(errno) << std::endl;
            cleanup();
        }
        else
        {
            std::cout << "  [" << BOLDBLUE << "SUCCESS" << RESET << "]: Set CPU_" << std::to_string(i) << " scheduling policy to fifo." << std::endl;
        }
    }   
    std::cout << std::endl;

    /* Set scheduling priority to fifo for I/O bound processes */
    for(unsigned short int i=0; i<io_bound.size(); i++)
    {
        return_val = sched_setscheduler(io_bound[i], SCHED_FIFO, &scheduling_struct);

        if(return_val == -1)
        {
            std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Could not set scheduling priority for IO_" << std::to_string(i) << "." << std::endl
                << "    " << strerror(errno) << std::endl;
            cleanup();
        }
        else
        {
            std::cout << "  [" << BOLDBLUE << "SUCCESS" << RESET << "]: Set IO_" << std::to_string(i) << " scheduling policy to fifo." << std::endl;
        }
    }
    std::cout << std::endl;

    /* Start CPU bound processes */
    std::cout << "Starting cpu bound processes in order from 1 through 5." << std::endl;
    clock_gettime(CLOCK_MONOTONIC, &run_time);

    for(unsigned short int i=0; i<cpu_bound.size(); i++)
    {
        return_val = kill(cpu_bound[i], SIGCONT);

        if(return_val == -1)
        {
            std::cout << "  [" << BOLDRED << "ERROR" << RESET << "]: Could not wake up CPU_" << std::to_string(i + 1) << "." << std::endl
                << "    " << strerror(errno) << std::endl;
            cleanup();
        }

        usleep(10000);
    }

    // Remember Definition: unsigned short int wait_count = 0
    wait_count = 0;
    while(wait_count < 5)
    {
        pid_t returned_pid = wait(NULL);

        for(unsigned short int i=0; i<cpu_bound.size(); i++)
        {
            if(returned_pid == cpu_bound[i])
            {
                std::cout << "  [" << BOLDBLUE << "SUCCESS" << RESET << "]: Process " << std::to_string(i + 1) << " completed!";
                clock_gettime(CLOCK_MONOTONIC, &snapshot_time);
                seconds = snapshot_time.tv_sec - run_time.tv_sec;
                if(run_time.tv_nsec > snapshot_time.tv_nsec)
                {
                    nanoseconds = snapshot_time.tv_nsec + (1*pow(10, 9) - run_time.tv_nsec);
                    seconds -= 1;
                }
                else
                {
                    nanoseconds = snapshot_time.tv_nsec - run_time.tv_nsec;
                }
                std::cout << "   Finish Time: " << seconds << " seconds, " << nanoseconds << " nanoseconds." << std::endl;
                
                wait_count += 1;
            }
        }
    }


    std::cout << std::endl;
    cleanup();
    return 0;
}