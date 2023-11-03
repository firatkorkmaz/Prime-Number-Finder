/* Finding Prime Numbers with Vectors in Multiple Processes and Threads (Ver.2) */

// Compile: "g++ 7_Prime_Numbers_v2.cpp"
// Run: "./a.out first_num last_num process_num thread_num" (process_num = 2 or thread_num = 2 If Not Given)
// Example: "./a.out -4 7 3 2"		// Finding Prime Numbers Between -4 and 7 with 2 Threads per 3 Processes
// Example: "./a.out 1 10 3"		// Finding Prime Numbers Between 1 and 10 with 2 Threads per 3 Processes
// Example: "./a.out 0 20"			// Finding Prime Numbers Between 0 and 20 with 2 Threads per 2 Processes

#include <vector>
#include <thread>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/mman.h>
#include <sys/wait.h>
using namespace std;


void *create_shared_memory(size_t size) {
	
	int protection = PROT_READ | PROT_WRITE;

	int visibility = MAP_SHARED | MAP_ANONYMOUS;

	return mmap(NULL, size, protection, visibility, -1, 0);
}


int main(int argc, char* argv[]){

	// Firstly, A 3D Int Vector is Declared to Store the Sizes
	// ...of Sub-Intervals Assigned to the Processes and Threads
	vector<vector<vector<int>>> value;

	// "shmem" is A 2D Void Pointer Vector Used as A Vector of the Shared Memory
	vector<vector<void*>> shmem;

	// pid Values will Be Held by A pid_t Vector
	vector<pid_t> pids;

	// Variables: starting_number(init), ending_number(end), process_number(pr) and thread_number(th)
	int init, end, pr, th;

	if(argv[1] && argv[2] && argv[3] && argv[4]){	// If All the Requested Inputs are Given Properly 
		istringstream first(argv[1]), last(argv[2]), processes(argv[3]), threads(argv[4]);
		first >> init; last >> end; processes >> pr; threads >> th;	// Assign Them to the Variables
	}
	
	else if(argv[1] && argv[2] && argv[3]){			// If the Thread Number is Not Given from the Input
		istringstream first(argv[1]), last(argv[2]), processes(argv[3]);
		first >> init; last >> end; processes >> pr;
		th = 2;										// Set the Thread Number to 2 by Default
	}
	
	else if(argv[1] && argv[2]){				// If both the Process and Thread Numbers are Not Given
		stringstream first(argv[1]), last(argv[2]);
		first >> init; last >> end; 
		pr = 2;				 					// Set both the Process and Thread Numbers to 2 by Default
		th = 2;
	}
	
	else{					// If Nothing is Given from the Input, Assign Some Values to All the Variables
		init = 1;			// This Section could be A Warning Printer and An Exit Command from the Program
		end = 40;
		pr = 6;
		th = 3;
	}
	

	int in = (end-init)+1;		// Interval of the Entered "init" and "end" Numbers from the Input

	if(pr <= 0 || th <= 0){		// If the Number of Processes or Threads is Zero, Exit the Program
		cout << "Process or Thread Number cannot Be Negative or Zero!" << endl;
		cout << "Please Run the Program with A Correct Input such as:" << endl;
		cout << "./a.out min_number max_number process_number thread_number" << endl;
		return 0;
	}

	if(in < 0){					// If the User Firstly Entered the Max Value and Then the Min Value
		int t=end;
		end = init;
		init = t;				// Change Their Order to init=min & end=max Values
		in = (end-init)+1;
	}

	if(in < pr){				// If the Number of Total Data is Less than the Process Number
		pr = in;				// Make the Process Number Equal to the Total Data Number
	}


	cout << "Number of Data Parts: " << in << endl;
	cout << "Number of Processes : " << pr << endl;
	cout << "Number of Threads   : " << th << endl;
	cout << endl;


	int x, y;
	int val = init;	// Copy the "init" Value which will Be Used for Assigning All the Numbers to the "value" Vector
	
	for(int i = 0; i < pr; i++){				// For Each Process
		value.push_back(vector<vector<int>>());	// A New Data Sub-Vector for Each Process is Added to the "value"
		shmem.push_back(vector<void*>());		// A New Memory Sub-Vector for Each Process is Added to the "shmem"
		
		cout << "Process-" << i+1 << ": ";
		
		if(i < in%pr)				// If There are Extra Data Left After Sharing Them with Each Process
			x = in/pr+1;			// Add One of Them to the Current Process
		else
			x = in/pr;
		
		if(x < th){					// If the Data Number for This Process is Less than Its Thread Number
			th = x;					// Make the Thread Number Equal to This Data Number
		}
		
		cout << x << " Data Parts -> " << th << " Threads: ";
		
		for(int j = 0; j < th; j++){			// For Each Thread
			
			value[i].push_back(vector<int>());	// Add A New Data Sub-Vector to the "value"
			
			if(j < x%th)
				y = x/th+1;
			else
				y = x/th;
			
			cout << y << " ";
			
			for(int k = 0; k < y; k++){
				value[i][j].push_back(val++);	// Assign Values to the 3D "value" Vector
			}
		
			shmem[i].push_back(create_shared_memory(value[i][j].size()*sizeof(int)));	// Allocate Memory for Each Thread's Data Interval
			memcpy(shmem[i][j], value[i][j].data(), value[i][j].size()*sizeof(int));	// Copy the Data Sub-Vector for This Thread to "shmem"
		}
		cout << endl;
	}
	
	
	// From Now on, "value.size()" Instead of "pr" and value[i].size() Instead of "th" (for i'th pr) will Be Used
	
	cout << endl;
	cout << "Master: Started." << endl;

	vector<int> work;	// Int Vector to Get Raw Data from the Shared Memory and Execute Prime Number Detection on It
	
	for (int i = 0; i < int(value.size()); i++) {				// For Each Process
		pids.push_back(fork());									// Do the Fork Operation
		
		if (pids[i] == 0) {										// If the Process is A Child
			cout << "Slave " << i+1 << ": Started. Interval ";	// Give Information About the Child Process (Slave)
			if(value[i][0][0] < 0)								// If the Value is Negative
				cout << "(" << value[i][0][0] << ")";			// Show Them within Paranthesis
			else
				cout << value[i][0][0];
			cout << "-";
			
			if(value[i][int(value[i].size())-1][value[i][int(value[i].size())-1].size()-1] < 0)
				cout << "(" << value[i][int(value[i].size())-1][value[i][int(value[i].size())-1].size()-1] << ")";
			else
				cout << value[i][int(value[i].size())-1][value[i][int(value[i].size())-1].size()-1];
			cout << endl;
			
			for(int j = 0; j < int(value[i].size()); j++){
				work = vector<int>(value[i][j].size(), 0);
				memcpy(work.data(), shmem[i][j], value[i][j].size()*sizeof(int));	// Get Data from the Shared Memory into "work"
				
				cout << "Thread " << i+1 << "." << j+1 << ": Searching in ";	// Thread Information and Their Processed Intervals

				if(value[i][j][0] < 0)									// If the Value is Negative
					cout << "(" << value[i][j][0] << ")";				// Show It in Paranthesis Like: "(-4)-3" {Between -4 and 3}
				else
					cout << value[i][j][0];
				cout << "-";
				
				if(value[i][j][value[i][j].size()-1] < 0)
					cout << "(" << value[i][j][value[i][j].size()-1] << ")";
				else
					cout << value[i][j][value[i][j].size()-1];		// Last Edge of the Interval Being Held by A Thread
				cout << endl;

				for(int k = 0; k < int(value[i][j].size()); k++){	// For Each Element of the Thread Interval
					thread t{[&](){									// Run A Thread
						bool sw = false;							// Detection of the Prime Numbers
						if(work[k] < 0){							// If the Number is Negative
							work[k] = -work[k];						// Make It Positive Temporarily to Check If It is Prime
							sw = true;
						}
						for(int w = 2; w <= work[k]/2; w++){		// Prime Number Detection Algorithm
							if(work[k] % w == 0){
								work[k] = 0;
								break;
							}
						}
						if(sw == true)			// If the Number was Negative and Also was A Prime Number, Change It Back to Negative
							work[k] = -work[k];
					}};
					t.join();		// After That, All the Prime Numbers will Remain and All the Non-Prime Numbers will Be Zero (Eliminated)
				}
				memcpy(shmem[i][j], work.data(), value[i][j].size()*sizeof(int));		// Copy the Thread Intervals Back to the Shared Memory
			}
			cout << "Slave " << i+1 << ": Done." << endl;			// Give Information About the End of Slave Run
		exit(0);
		}
	}

	for (int i = 0; i < int(value.size()); i++){
		waitpid(-1, NULL, 0);										// Wait all the child processes to finish.
	}
	cout << "Master: Done." << endl;
	
	
	cout << endl << "Prime Numbers are: ";
	vector<int> result;
	
	for(int i = 0; i < int(value.size()); i++){						// For Each Process
		for(int j = 0; j < int(value[i].size()); j++){				// For Each Thread
			result = vector<int>(value[i][j].size(), 0);			// Prepare the Result Vector for memcpy(result.data(),..)
			memcpy(result.data(), shmem[i][j], value[i][j].size()*sizeof(int));	// Copy the Shared Memory to the Result Vector
			for(int k = 0; k < int(value[i][j].size()); k++){		// Each Element of the Thread Intervals
				if(result[k] != 0){									// If the Number is Not Replaced with Zero (If It is A Prime Number)
					cout << result[k] << ", ";						// Print It to the Screen
				}
			}
			munmap(shmem[i][j], value[i][j].size()*sizeof(int));	// Release the Shared Memory
		}
	}

	
	// Delete the Vectors and Free Their Memories
	value = vector<vector<vector<int>>>();	
	shmem = vector<vector<void*>>();
	pids = vector<pid_t>();

	printf("\nShared Memory is Released! Now Quitting the Program...\n\n");


return 0;
}
