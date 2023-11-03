/* Forks with Shared Memory */

#include <vector>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <sys/mman.h>
using namespace std;


void *create_shared_memory(size_t size) {

  int protection = PROT_READ | PROT_WRITE;

  int flags = MAP_SHARED | MAP_ANONYMOUS;

  return mmap(NULL, size, protection, flags, -1, 0);
}


vector<void*> shmem;			// Vector of the Shared Memory

int i = 0;
int stop = 2;
int value = 0;
int value1 = 1;
int value2 = 1;


void forker(int inpid, int limit){

	if(inpid == 0){
		cout << "I am Child: " << i << endl;			// Child Works
		
		memcpy(&value1, shmem[1], sizeof(int));
		memcpy(&value2, shmem[2], sizeof(int));
		
		value1 *= 5;
		cout << "value1 *= 5 => " << value1 << endl;
		value2 *= 10;
		cout << "value2 *= 10 => " << value2 << endl;
		
		memcpy(shmem[1], &value1, sizeof(int));
		memcpy(shmem[2], &value2, sizeof(int));

	}
	
	if(inpid > 0){
		
		i++;
		if(i > limit){
			wait(NULL);
			cout << "And I am Parent" << endl;			// Parent Works
			
			memcpy(&value1, shmem[1], sizeof(int));
			memcpy(&value2, shmem[2], sizeof(int));
			
			value = value1 + value2;
			cout << "value = value1 + value2" << endl;
			cout << value << "   =   " << value1 << "   +  " << value2 << endl;
			
			memcpy(shmem[0], &value, sizeof(int));
			
			for(int i=0; i<int(shmem.size()); i++)
				munmap(shmem[i], sizeof(shmem[i]));		// Removing the Mappings...
			
			return;
		}
		
		inpid = fork();
		wait(NULL);
		forker(inpid, limit);
	}

return;
}


int main(){
	
	cout << "value = " << value << endl;
	cout << "value1 = " << value1 << endl;
	cout << "value2 = " << value2 << endl;
	cout << "Stop at: " << stop << endl << "----------" << endl << endl;
	
	shmem.push_back(create_shared_memory(sizeof(int)));
	memcpy(shmem[0], &value, sizeof(int));
	
	shmem.push_back(create_shared_memory(sizeof(int)));
	memcpy(shmem[1], &value1, sizeof(int));
	
	shmem.push_back(create_shared_memory(sizeof(int)));
	memcpy(shmem[2], &value2, sizeof(int));
	
	int pid = getpid();
	forker(pid, stop);
	
	cout << "main() Prints: Value = " << value << endl << endl;
	
return 0;
}
