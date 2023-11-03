/* Vectors in Shared Memory */

#include <vector>
#include <cstring>
#include <iostream>
#include <sys/mman.h>
using namespace std;

void *create_shared_memory(size_t size) {
	
	int protection = PROT_READ | PROT_WRITE;

	int visibility = MAP_SHARED | MAP_ANONYMOUS;

	return mmap(NULL, size, protection, visibility, -1, 0);
}


vector<void*> shmem;


int main(int argc, char* argv[]){
	
	// The 1st memcpy() Operation
	vector<int> v1={0, 1, 2, 3, 4};
	cout << "v1: ";
	for(auto i : v1) cout << i << " ";
	cout << endl;

	shmem.push_back(create_shared_memory(v1.size()*sizeof(int)));	// Adding Shared Memory of v Size to the shmem Vector
	memcpy(shmem.back(), v1.data(), v1.size()*sizeof(int));			// Getting v to the shmem Vector with memcpy()

	vector<int> c1(v1.size(), 0);									// New Vector must Have the Same Size
	memcpy(c1.data(), shmem.back(), v1.size()*sizeof(int));
	
	cout << "c1: ";
	for(auto i : c1) cout << i << " ";
	cout << endl << endl;;
	
	//////////////////////////////////////////
	
	// The 2nd memcpy() Operation
	vector<int> v2={5, 6, 7, 8, 9, 0};
	cout << "v2: ";
	for(auto i : v2) cout << i << " ";
	cout << endl;

	shmem.push_back(create_shared_memory(v2.size()*sizeof(int)));	// Adding Shared Memory of v Size to the shmem Vector
	memcpy(shmem.back(), v2.data(), v2.size()*sizeof(int));			// Getting v to the shmem Vector with memcpy()

	vector<int> c2(v2.size(), 0);									// New Vector must Have the Same Size
	memcpy(c2.data(), shmem.back(), v2.size()*sizeof(int));
	
	cout << "c2: ";
	for(auto i : c2) cout << i << " ";
	cout << endl;
	
	//////////////////////////////////////////
	
	for(int i=0; i<int(shmem.size()); i++)
		munmap(shmem[i], sizeof(shmem[i]));							// Removing the Mappings...
	
	c1 = vector<int>(v1.size(), 0);									// Clearing the c1 Vector (Optional)
	c2 = vector<int>(v2.size(), 0);									// Clearing the c2 Vector (Optional)
	
	/*
	memcpy(c1.data(), shmem.back(), v1.size()*sizeof(int));			// THIS CAUSES SEGMENTATION FAULT!
	*/																// Because the Mappings were Removed

return 0;
}
