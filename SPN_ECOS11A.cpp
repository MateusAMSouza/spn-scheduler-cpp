//Operating Systems Lab - Final Project - 2022.2
//Shortest Process Next Algorithm
//Mateus Alexandre M. de Souza - 2021004023
#include<bits/stdc++.h>
#include<fstream>

using namespace std;

int totalProcesses, quantum;

//to sort the priority_queues in descending order, the first elements of the pairs are inserted as negatives
priority_queue<tuple<int, int, int>> processes;
priority_queue<tuple<int, int, int>> scheduler;

ifstream inFile("processes.txt");
ofstream outFile("diagram.txt");

//this function prints diagram's lines according to the current quantum
//current process in execution is represented by "##" and waiting processes by "--"
void PrintLine(int process, int* life, int* wait)
{
    outFile << setw(2) << setfill('0') << quantum << "-";
    outFile << setw(2) << setfill('0') << quantum+1 << " ";

    for(int i = 0; i < totalProcesses-1; i++)
    {
        if(!life[i])
        {
            if(i == process-1)
            {
                outFile << " ##";
            }
            else
            {
                outFile << " --";
            }
        }
        else
        {
            outFile << "   ";
        }
    }
    outFile << endl;
}

//the processes will enter the priority_queue scheduler according to their arrival time and the current quantum
//then, the scheduler decreases the absoulte value of the shortest process and updates its waiting time and, if it finishes, its lifetime
void Scheduler(int* life, int* wait)
{
    bool switched;
    int aux, previousProcess, processNumber = -1;
    int runtime, arrival, number;
    
    while(!processes.empty() || !scheduler.empty())
    {
        switched = false;

        if(!processes.empty())
        {
            arrival = -get<0>(processes.top());

            if(arrival == quantum)
            {
                runtime = get<1>(processes.top());
                arrival = -get<0>(processes.top());
                number = get<2>(processes.top());

                scheduler.push({-runtime, arrival, number});
                processes.pop();
                
                life[number-1] = 0;

                continue;
            }
        }

        if(!scheduler.empty())
        {
            if(processNumber > 0)
            {
                previousProcess = processNumber;
            }

            runtime = get<0>(scheduler.top()) + 1;
            arrival = get<1>(scheduler.top());
            processNumber = get<2>(scheduler.top());

            scheduler.pop();

            if(processNumber != previousProcess) switched = true;

            if(wait[processNumber-1] == -1)
            {
                wait[processNumber-1] = quantum - arrival;
            }
            else if(switched)
            {
                aux = wait[processNumber-1] + arrival+1;
                wait[processNumber-1] += quantum - aux;
            }

            PrintLine(processNumber, life, wait);

            if(runtime)
            {
                scheduler.push({runtime, arrival, processNumber});
            }
            else
            {
                life[processNumber-1] = (quantum+1) - arrival;
            }
        }
        quantum++;
    }
}

int main()
{
    int runtime, arrival, ignore;
    int number = 1;

    float averageLife, averageWait;

    //reads the process' values, makes a tuple and inserts it on the processes queue
    //the third value is the priority of the process, but it's not used on the SPN algorithm
    while(inFile >> arrival >> runtime >> ignore)
    {
        processes.push({-arrival, runtime, number});
        number++;
    }
    inFile.close();
    
    totalProcesses = number;

    int life[totalProcesses-1];
    int wait[totalProcesses-1];

    quantum = 0;

    memset(life, -1, sizeof(life));
    memset(wait, -1, sizeof(wait));

    //prints the diagram's header
    outFile << "time  ";
    for(int i = 1; i < totalProcesses; i++)
    {
        outFile << " P" << i;
    }
    outFile << endl;

    Scheduler(life, wait);

    averageLife = (float)accumulate(life, life+totalProcesses-1, 0)/(totalProcesses-1);
    averageWait = (float)accumulate(wait, wait+totalProcesses-1, 0)/(totalProcesses-1);

    outFile << endl << "Average process lifetime: " << averageLife << endl;
    outFile << "Average process waiting time: " << averageWait << endl;
    outFile << "Number of context switches: " << quantum-1 << endl;

    return 0;
}
