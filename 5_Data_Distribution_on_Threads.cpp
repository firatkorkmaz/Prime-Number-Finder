/* Sharing a Number of Data with Multiple Processes and Threads */

#include <iostream>
using namespace std;


int main(){

int in = 19;	// Number of Data to Share with Processes & Threads
int pr = 5;		// Number of Processes
int th = 3;		// Number of Threads Per Each Process
int x, y;

if(in < pr)
	pr = in;

cout << "Number of Data Parts: " << in << endl;
cout << "Number of Processes : " << pr << endl;
cout << "Number of Threads   : " << th << endl;
cout << endl;

for(int i=0; i<pr; i++){
	
	cout << "pr-" << i+1 << ": ";
	
	if(i < in%pr)
		x = in/pr+1;
	else
		x = in/pr;
	
	if(x < th)
		th = x;
	
	cout << x << " Data -> " << th << " Threads: ";
	
	for(int j=0; j<th; j++){
		if(j < x%th)
			y = x/th+1;
		else
			y = x/th;
		cout << y << " ";
	}
	cout << endl;
}

return 0;
}
