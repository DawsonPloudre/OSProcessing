#include "process.h"
#include "ioModule.h"
#include "processMgmt.h"


#include <chrono> // for sleep
#include <thread> // for sleep


int main(int argc, char* argv[])
{
	// single thread processor
	// it's either processing something or it's not
//	bool processorAvailable = true;


	// vector of processes, processes will appear here when they are created by
	// the ProcessMgmt object (in other words, automatically at the appropriate time)
	list<Process> processList;
	list<unsigned int> readylist;
	// this will orchestrate process creation in our system, it will add processes to
	// processList when they are created and ready to be run/managed
	ProcessManagement processMgmt(processList);


	// this is where interrupts will appear when the ioModule detects that an IO operation is complete
	list<IOInterrupt> interrupts;  
	vector<IOEvent> waiting;


	// this manages io operations and will raise interrupts to signal io completion
	IOModule ioModule(interrupts);  


	// Do not touch
	long time = 1;
	long sleepDuration = 50;
	string file;
	stringstream ss;
	enum stepActionEnum {noAct, admitNewProc, handleInterrupt, beginRun, continueRun, ioRequest, complete} stepAction;


	// Do not touch
	switch(argc)
	{
    	case 1:
        	file = "./procList.txt";  // default input file
        	break;
    	case 2:
        	file = argv[1];     	// file given from command line
        	break;
    	case 3:
        	file = argv[1];     	// file given
        	ss.str(argv[2]);    	// sleep duration given
        	ss >> sleepDuration;
        	break;
    	default:
        	cerr << "incorrect number of command line arguments" << endl;
        	cout << "usage: " << argv[0] << " [file] [sleepDuration]" << endl;
        	return 1;
        	break;
	}


	processMgmt.readProcessFile(file);




	time = 0;
//	processorAvailable = true;






	//keep running the loop until all processes have been added and have run to completion
	while(processMgmt.moreProcessesComing() || processRunning(processList)/* TODO add something to keep going as long as there are processes that arent done! */ )
	{
    	//Update our current time step
    	++time;


    	//let new processes in if there are any
    	processMgmt.activateProcesses(time);


    	//update the status for any active IO requests
    	ioModule.ioProcessing(time);


    	//If the processor is tied up running a process, then continue running it until it is done or blocks
    	//   note: be sure to check for things that should happen as the process continues to run (io, completion...)
    	//If the processor is free then you can choose the appropriate action to take, the choices (in order of precedence) are:
    	// - admit a new process if one is ready (i.e., take a 'newArrival' process and put them in the 'ready' state)
    	// - address an interrupt if there are any pending (i.e., update the state of a blocked process whose IO operation is complete)
    	// - start processing a ready process if there are any ready




    	//init the stepAction, update below
    	stepAction = noAct;
  	 
    	//TODO add in the code to take an appropriate action for this time step!
    	//you should set the action variable based on what you do this time step. you can just copy and paste the lines below and uncomment them, if you want.
    	//stepAction = continueRun;  //runnning process is still running
    	//stepAction = ioRequest;  //running process issued an io request
    	//stepAction = complete;   //running process is finished
    	//stepAction = admitNewProc;   //admit a new process into 'ready'
    	//stepAction = handleInterrupt;   //handle an interrupt
    	//stepAction = beginRun;   //start running a process
    	bool taken = false;
    	for(list<Process>::iterator it = processList.begin(); it != processList.end(); ++it){
      	if(it->state == processing){ // Process is currently running
			++it->processorTime;
			taken = true;
				if(!it->ioEvents.empty() && it->ioEvents.front().time == it->processorTime){ // Process issues IoRequest
					stepAction = ioRequest;
					it->state = blocked;
					ioModule.submitIORequest(time, it->ioEvents.front(), *it);
					it->ioEvents.pop_front();
					break;
				}
				else if(it->processorTime >= it->reqProcessorTime){ //Process is completed
					it->state = done;
					stepAction = complete;
					break;
				}
				else{ // Current process is not interrupted or complete
					stepAction = continueRun;
					break;
				}
			}
    	}
	if(!taken){
		for(list<Process>::iterator it = processList.begin(); it != processList.end(); ++it){ //Check for admitting new process
			Process& currprocess = *it;
			if(currprocess.state == newArrival && !taken){
				taken = true;
				currprocess.state = ready;
				readylist.push_back(it->id);
				stepAction = admitNewProc;
				break;
			}
		}
	}
	if(!taken && !interrupts.empty()){ //Handles interrupt
		stepAction = handleInterrupt;
		for(list<Process>::iterator it = processList.begin(); it != processList.end(); it++){
			if(it->id == interrupts.front().procID){
				it->state = ready;
				readylist.push_back(it->id);
			}
		}
		interrupts.pop_front();
		taken = true;
	}


	if(!taken && !readylist.empty()){ //Start running a ready process
		for(list<Process>::iterator it = processList.begin(); it != processList.end(); ++it){ //Check for ready processes
			Process& currprocess = *it;
			if(currprocess.id == readylist.front()){
				taken = true;
				currprocess.state = processing;
				readylist.pop_front();
				stepAction = beginRun;
				break;
			}
		}
	}



    	// Leave the below alone (at least for final submission, we are counting on the output being in expected format)
    	cout << setw(5) << time << "\t";
  	 
    	switch(stepAction)
    	{
        	case admitNewProc:
          	cout << "[  admit]\t";
          	break;
        	case handleInterrupt:
          	cout << "[ inrtpt]\t";
          	break;
        	case beginRun:
          	cout << "[  begin]\t";
          	break;
        	case continueRun:
          	cout << "[contRun]\t";
          	break;
        	case ioRequest:
          	cout << "[  ioReq]\t";
          	break;
        	case complete:
          	cout << "[ finish]\t";
          	break;
        	case noAct:
          	cout << "[*noAct*]\t";
          	break;
    	}


    	// You may wish to use a second vector of processes (you don't need to, but you can)
    	printProcessStates(processList); // change processList to another vector of processes if desired


    	this_thread::sleep_for(chrono::milliseconds(sleepDuration));
	}


	return 0;
}

