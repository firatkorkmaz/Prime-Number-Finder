/* memset() and memcpy() Executions on Vectors */

#include <iostream>
#include <cstring>
#include <vector>
using namespace std;


void printArray(int iarr[], int n){
	for (int i=0; i<n; i++)
		cout << iarr[i] << " ";
}


int main(){
	
    vector<int> vec(5, 1);							// 5-Size Int Vector with "1" Values
	cout << "Vector: ";
	printArray(vec.data(), vec.size());
	cout << "(Printing Initial Vector)" << endl;
	
	memset(vec.data(), 0, vec.size()*sizeof(int));	// Set All Values to 0 with memset
	cout << "Vector: ";
	printArray(vec.data(), vec.size());
	cout << "(After memset: Elements->0)" << endl;
	
	// Copy to Int Vector from Int Vector
    memcpy(vec.data(), vector<int>{1, 2, 3, 4, 5}.data(), vec.size()*sizeof(int));
    cout << "Vector: ";
	printArray(vec.data(), vec.size());
    cout << "(After memcpy: from Vector)" << endl;
    
    // Copy to Int Vector from Int Array
    memcpy(vec.data(), new int[]{5, 4, 3, 2, 1}, vec.size()*sizeof(int));
    cout << "Vector: ";
	printArray(vec.data(), vec.size());
    cout << "(After memcpy: from iArray)" << endl;
    
    int arr[5];
    // Copy to Int Array from Int Vector
    memcpy(arr, vec.data(), vec.size()*sizeof(int));
    cout << "iArray: ";
	printArray(arr, sizeof(arr)/sizeof(int));
    cout << "(After memcpy: from Vector)" << endl;
	
return 0;
}

