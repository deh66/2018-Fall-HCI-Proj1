// Daniel Hollo
// deh66@zips.uakron.edu
// Human Computer Interaction (HCI) Assignment 1
// 1/28/2018

#include <iostream>
#include <string>
#include <cmath>
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <queue>

// Remove before submitting //
using namespace std;
// //////////////////////// //



mutex mtxIn;
mutex mtxOut;

string input = "";
string outputStr = "";
queue<string> outputQ;

bool stopflag = false;
bool outputflag = false;
bool exitflag = false;
bool completeflag = false;



// Thread function to perform computation
void compute(int inputInt);
// Thread string to input from console
void inputGet();
// Thread function to output to console
void output();


int main() 
{
	bool done = false;
	string locStr = "";
	int inputInt = 0;
	
	thread inThread(inputGet);
	thread outThread(output);

	// Send initial output -- Thread
	mtxOut.lock();
	outputStr = "Please enter an integer or 'q' to quit: ";
	outputQ.push(outputStr);
	outputflag = true;
	mtxOut.unlock();

	// Wait for Threads to Terminate
	inThread.join();
	outThread.join();

	// Gracefully Terminate Threads (in case they werent already, probably not needed, i am very tired...)
	stopflag = true;

	return 0;
}


// Thread string to input from console
void inputGet()
{
	string inStr = "";
	int inputInt = 0;

	while (!stopflag)
	{
		// Read in user input
		cin >> inStr;

		mtxIn.lock();	// Lock global mutex
		input = inStr;	// Set value of global string variable
		mtxIn.unlock();	//unlock global mutex 


		if (inStr[0] == 's')		// if semaphore 's' entered set appropreate flag
		{
			exitflag = true;
		}
		else if (inStr[0] == 'q')	// If quit command is called, create exit case
		{
			stopflag = true;
			exitflag = true;
		}
		else if (isdigit(inStr[0])) // Check if integer input and set inputInt if so
		{
			inputInt = stoi(inStr);

			// Compute
			thread compThread(compute, inputInt);
			// Do Not wait for compute thread to complete
			compThread.detach();
		}

	}
	return;
}


// Thread function to output to console
void output()
{
	while (!stopflag)
	{
		// if outputflag is true (there is output in outputStr)
		if (outputflag)
		{
			// Lock outputStr, output it, then unlock it again later
			mtxOut.lock();

			// while there are items in the queue and an exit condition was not created: output to console the output queue
			while (!outputQ.empty() && !exitflag)
			{
				outputStr = outputQ.front();
				outputQ.pop();
				
				cout << outputStr << "\n";
			}
			outputStr = "";
			// compute operation exited, dump the output queue
			if (exitflag)
			{
				while (!outputQ.empty())
					outputQ.pop();
			}
			// if operation in compute function is complete print user prompt
			if (completeflag)
			{
				cout << "Please enter an integer or 'q' to quit: \n";
				completeflag = false;
			}

			// reset the outputflag
			outputflag = false;

			mtxOut.unlock();
		}
	}

	return;
}


// Thread function to perform computation
void compute(int inputInt)
{
	bool doneLoop = false;
	long double result = 3.1415926535897;	// initialize result to pi
	int i = 1;								// iterator

	while (!doneLoop) {
		if (inputInt == 0)
		{
			// End case, we have iterated as many times as asked
			doneLoop = true;
		}
		else
		{
			// output number of iterations this pass is (push to output queue)
			mtxOut.lock();
			outputStr = to_string(i);
			outputQ.push(outputStr);
			outputflag = true;
			mtxOut.unlock();
			
			// get the square root of the current working number
			result = sqrt(result);
			inputInt--;
		}
		// if exitflag is true (escape sequence entered), create exit case
		if (exitflag)
		{
			exitflag = false;
			doneLoop = true;
		}

		i++;
	}
	// format result string
	string resultStr = "Result = " + to_string(result);

	// output result to output thread function
	mtxOut.lock();
	outputStr = resultStr;
	outputQ.push(outputStr);
	outputflag = true;
	mtxOut.unlock();

	// flag to instruct output thread function
	completeflag = true;

	return;
}