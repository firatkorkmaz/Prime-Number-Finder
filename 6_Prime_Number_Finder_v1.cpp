/* Finding Prime Numbers with Vectors in Multiple Processes and Threads (Ver.1) */

// Compile: "g++ 6_Prime_Numbers_v1.cpp"
// Run: "./a.out first_num last_num process_num thread_num" (process_num = 2 or thread_num = 2 if not given).
// Example: "./a.out -4 7 3 2"		// Finding Prime Numbers Between -4 and 7 with 2 Threads per 3 Processes.
// Example: "./a.out 1 10 3"		// Finding Prime Numbers Between 1 and 10 with 2 Threads per 3 Processes.
// Example: "./a.out 0 20"			// Finding Prime Numbers Between 0 and 20 with 2 Threads per 2 Processes.

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


int idp = 0;
vector<int*> value;
vector<void*> shmem;
int share;


void forker(int inpid, int pr, int in, int th){
	
	if(inpid == 0){
		//cout << "I am Child: " << idp << endl;	// Child Works
		int *work;
		if(share>0){
			work= new int[in/pr+1];
			memcpy(work, shmem[idp-1], (in/pr+1)*sizeof(int));
			for(int j=0; j<in/pr+1; j++){
				
				thread t{[&](){
					for(int w=2; w<=work[j]/2; w++){
						if(work[j]%w==0){
							work[j]=0;
							break;
						}
					}
				}};
				t.join();
				
			} memcpy(shmem[idp-1], work, (in/pr+1)*sizeof(int));
		}
		else{
			work= new int[in/pr];
			memcpy(work, shmem[idp-1], (in/pr)*sizeof(int));
			for(int j=0; j<in/pr; j++){
				
				thread t{[&](){
					for(int w=2; w<=work[j]/2; w++){
						if(work[j]%w==0){
							work[j]=0;
							break;
						}
					}
				}};
				t.join();
				
			} memcpy(shmem[idp-1], work, (in/pr)*sizeof(int));
		}
		delete[] work;
		share--;
	}
	
	if(inpid > 0){
		idp++;
		if(idp > pr){
			wait(NULL);
			//cout << "And I am Parent" << endl;	// Parent Works
			int *result;
			share=in%pr;
			printf("\nAfter Detection of Prime Numbers:\n");
			
			
			for(int z=0; z<pr; z++){
				if(share>0){
					result = new int[in/pr+1];
					memcpy(result, shmem[z], (in/pr+1)*sizeof(int));
					for(int u=0; u<in/pr+1; u++)
						printf("%d ", result[u]);
					munmap(shmem[z], (in/pr+1)*sizeof(int));
				}
				else{
					result = new int[in/pr];
					memcpy(result, shmem[z], (in/pr)*sizeof(int));
					for(int u=0; u<in/pr; u++)
						printf("%d ", result[u]);
					munmap(shmem[z], (in/pr)*sizeof(int));
				}
				delete[] result;
				share--;
				if(z!=pr-1)
					cout << "- ";
			}
			
			vector<void*>().swap(shmem);
			printf("\n\nShared Memory is Released! Now Quitting the Program...\n");
			return;
		}
		
		inpid = fork();
		wait(NULL);
		forker(inpid, pr, in, th);
	}

return;
}


int main(int argc, char* argv[]){
	
	int init, end, pr, th;

	if(argv[1] && argv[2] && argv[3] && argv[4]){	// If All the Requested Inputs are Given Properly
		istringstream first(argv[1]), last(argv[2]), processes(argv[3]), threads(argv[4]);
		first >> init; last >> end; processes >> pr; threads >> th;		// Assign Them to the Variables
	}
	
	else if(argv[1] && argv[2] && argv[3]){			// If the Thread Number is Not Given from the Input
		istringstream first(argv[1]), last(argv[2]), processes(argv[3]);
		first >> init; last >> end; processes >> pr;
		th = 2;										// Set the Thread Number to 2 by Default
	}
	
	else if(argv[1] && argv[2]){					// If both the Process and Thread Numbers are Not Given
		stringstream first(argv[1]), last(argv[2]);
		first >> init; last >> end; 
		pr = 2;				 						// Set both the Process and Thread Numbers to 2 by Default
		th = 2;
	}
	
	else{						// If Nothing is Given from the Input, Assign Some Values to All the Variables
		init = 1;				// This Section Could Be A Warning Printer and An Exit Command from the Program
		end = 40;
		pr = 6;
		th = 3;
	}
	
	int in = (end-init)+1;
	int val=init;

	//int share;

	if(in<=0 || pr<=0)
		return 0;

	if(in<pr)
		pr=in;

	printf("Before Detection of Prime Numbers:\n");

	share=in%pr;
	for(int i=0; i<pr; i++){
		
		if(share>0){
			value.push_back(new int[(in/pr)+1]);
			for(int j=0; j<(in/pr)+1; j++){
				value[i][j]=val++;
			}
			shmem.push_back(create_shared_memory(((in/pr)+1)*sizeof(int)));
			memcpy(shmem[i], value[i], ((in/pr)+1)*sizeof(int));
		}
		
		else{
			value.push_back(new int[in/pr]);
			for(int j=0; j<in/pr; j++){
				value[i][j]=val++;
			}
			shmem.push_back(create_shared_memory((in/pr)*sizeof(int)));
			memcpy(shmem[i], value[i], (in/pr)*sizeof(int));
		}
		
		share--;
	}

	////////////////////////////////////////

	int *input;
	share=in%pr;
	for(int i=0; i<pr; i++){
		if(share>0){
			input=new int[in/pr+1];
			memcpy(input, shmem[i], (in/pr+1)*sizeof(int));
			for(int j=0; j<(in/pr)+1; j++){
				cout << input[j] << " ";
			}
		}
		else{
			input=new int[in/pr];
			memcpy(input, shmem[i], (in/pr)*sizeof(int));
			for(int j=0; j<(in/pr); j++){
				cout << input[j] << " ";
			}
		}
		delete[] input;

		if(i!=pr-1)
			cout << "- ";
		share--;
	}

	cout << endl;

	share=in%pr;
	int pid = getpid();
	forker(pid, pr, in, th);

	value = vector<int*>();
	shmem = vector<void*>();

return 0;
}
