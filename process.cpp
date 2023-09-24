#include "process.h"

void printProcessStates(list<Process>& processVector)
{
    char stateChar;
    for(auto & Proc : processVector)
    {
        switch (Proc.state)
        {
            case ready:
                stateChar = 'r';
                break;
            case processing:
                stateChar = 'p';
                break;
            case blocked:
                stateChar = 'b';
                break;
            case newArrival:
                stateChar = 'n';
                break;
            case done:
                stateChar = 'd';
                break;
        }
        cout << stateChar << ' ';
    }
    cout << endl;
}

void printProcessSet(vector<Process>& processVector)
{
    cout << "AT | DT | RQPT | PT | S | IO" << endl;
    for(auto & Proc : processVector)
    {
        Proc.printProcess();
    }
}

bool processRunning(list<Process>& processes){
    list<Process>::iterator it;
    it = processes.begin();
    for(it = processes.begin(); it != processes.end(); ++it){
    if(it->state != done){ return false; }
    }
    return true;
}

long ioEventTime(Process process){
    return process.ioEvents.front().time;
}