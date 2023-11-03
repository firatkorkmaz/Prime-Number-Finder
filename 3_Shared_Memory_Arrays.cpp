/* Arrays in Shared Memory */

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


vector<void*> shmem;	// Vector of the Shared Memory


int main(int argc, char* argv[]){
	
	int a1[] = {6, 7, 8};
	int a2[] = {1, 3, 5, 7, 9};
	
	shmem.push_back(create_shared_memory(sizeof(a1)));		// Adding Shared Memory of a1 Size to the shmem Vector
	memcpy(shmem.back(), a1, sizeof(a1));					// Getting a1 to the shmem Vector with memcpy()

	shmem.push_back(create_shared_memory(sizeof(a2)));		// Adding Shared Memory of a2 Size to the shmem Vector
	memcpy(shmem.back(), a2, sizeof(a2));					// Getting a2 to the shmem Vector with memcpy()

	////////////////////////////////////////////

	cout << "a1: ";
	for(int i : a1) cout << i << " ";
	cout << endl;
	
	int b1[sizeof(a1)/sizeof(int)];
	memcpy(b1, shmem[0], sizeof(b1));			// a1 is Copied to b1 by memcpy()
	cout << "b1: ";
	for(int i : b1) cout << i << " ";
	cout << endl;

	cout << endl;
	
	cout << "a2: ";
	for(int i : a2) cout << i << " ";
	cout << endl;

	int b2[sizeof(a2)/sizeof(int)];
	memcpy(b2, shmem[1], sizeof(b2));			// a2 is Copied to b2 by memcpy()
	cout << "b2: ";
	for(int i : b2) cout << i << " ";
	cout << endl;

	
	for(int i=0; i<int(shmem.size()); i++)
		munmap(shmem[i], sizeof(shmem[i]));		// Removing the Mappings...
	
	/*
	memcpy(b2, shmem[1], sizeof(b2));			// THIS CAUSES SEGMENTATION FAULT!
	*/											// Because the Mappings were Removed
	
return 0;
}
