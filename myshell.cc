#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>
using namespace std;


class Shell
{
	private:
	string currentDir;
	vector<string> commandHistory;
	vector<pid_t> pid_history;
	
	public:
	Shell(string dir)
	{
		currentDir = dir;
		commandHistory.clear();
		pid_history.clear();
	};
	
	string getShellDir() {return currentDir;}
	void setShellDir(string dir) {currentDir = dir;} 
	void appendShellDir(string toAppend) {currentDir.append(toAppend);}
	void recordCommand (string toRecord) {commandHistory.push_back(toRecord);}
	void recordPID (pid_t toRecord) {pid_history.push_back(toRecord);}
	void printCommandHistory () 
		{
			
			int size = commandHistory.size();
			
			if(size == 0)
				return;
				
				
			int i = 0;
			
			cout << "Command History\n";
			for(; i < size - 1; i++)
			{
			 cout << commandHistory.at(i) << "\n";
				}
		
		
			}
	
	void eraseHistory() {commandHistory.clear();}
	void erasePIDHistory() {pid_history.clear();}
	int getPIDSize() {return pid_history.size();}
	pid_t getPIDAt(int i) {return pid_history.at(i);}
};

//A function to convert from string to ints, because stoi is unrecognized for some reason and
//I dont want to run the risk of the same thing happening on whatever PC the TA is grading this on
int stringToInt(string inputString)
{
	int size = inputString.size() - 1;
	double exp = 0;
	int val = 0;
	
	for (; size >= 0; size--)
	{
		val +=  ( (int)inputString[size] - 48 ) * pow(10, exp);
		exp++;
		}
		
		return val;
	
	}



//A function to take a string of unknown but hopefully reasonable length, tokenize it with a list,
//and finally put it in an array of char*s
 char ** stringToVectorToCharArray(string inputString)
{
	
	stringstream ss(inputString);
	vector<string> tokens;
	tokens.clear();
	
	while(ss >> inputString)
	{
		tokens.push_back(inputString);
		}
		
	
	int size = tokens.size();
	int newSize = 0;	
	char **args = new char*[size + 1];
	
	args[0] = new char[tokens.at(0).size()];
	strcpy(args[0], tokens.at(0).c_str());
	
	int i = 1;
	int j = 0;
	for(; i < size; i++)
	{
			j++;
			args[i] = new char[tokens.at(i).size()];
			strcpy(args[i], tokens.at(i).c_str() );
		}
		
		
	args[j + 1] = (char *) 0;
		
	return args;
}

Shell parseString(Shell shell, string inputString)
{
	stringstream ss(inputString);
	string command;
	string argument = "";
	string parameters;
	bool relative = false;
	
	//Save the full command text to shell's history
	shell.recordCommand(inputString);
	
	//Parse command
	ss >> command;

	//Change Directory
	if(command.compare("cd") == 0)
	{
		
		const char * testChar;
		//Grab the target directory
		ss >> argument;
		
		//Is it a relative path or full path?
		if(argument[0] == '/')
		{
			//Full Path
			testChar = argument.c_str();
			
			}
			
		else
		{
			//Relative Path
			string testString = shell.getShellDir() + "/" + argument;
			testChar = testString.c_str();
			relative = true;
		
			}
		//Make sure the file exists
		int result = access(testChar, F_OK);
		if( result == 0)
		{
			//Format the working directory based on relative path
			if(relative == true)
			{
				shell.appendShellDir("/");
				shell.appendShellDir(argument);
				
		
				}
			
			//Replace the working directory based on absolute path	
			else
				shell.setShellDir(argument);
				
				//Print new directory
				cout << shell.getShellDir();
				return shell;
		}
		
		else
			cout << "Error " << testChar << " is not a valid path.";
			return shell;
		
		}
	
	
	//Print Working Directory
	else if(command.compare("pwd") == 0)
	{
		cout << shell.getShellDir();
		return shell;

		}
	
	//Print Command History
	else if(command.compare("history") == 0)
	{
		
		ss >> argument;
		
		if(argument.compare("-c") == 0)
			shell.eraseHistory();
			
		else if(argument == "")
			shell.printCommandHistory();
			
			
			
		else
			cout << "Invalid argument(s) for history command\n";
			
		return shell;
			
		}
		
		
	else if(command.compare("run") == 0)
	{
		
		ss >> argument;
		string executeString = "";
		int childEscapeStatus;
		
		if(argument[0] == '/')
		{
			//Absolute
			executeString = argument;
			}
			
		else
		{
			//Relative
			 executeString = shell.getShellDir() + '/' + argument;
			}
		
		
		
		const char * executeChar = executeString.c_str();
		if(access(executeChar, X_OK) != 0)
		{
			cout << "File does not exist\n";
			return shell;
			}
			

		pid_t PID = fork();
			
			//If its a child process, use execv to execute the program at the path specified by argument
				//With any parameters given by user passed as one array of args, with arg[0] being the path
				if(PID == 0)
				{
					
					 while(ss >> parameters)
					 {
						 argument.append(" ");
						 argument.append(parameters);
						 }

					char **args = stringToVectorToCharArray(argument);
					execv(executeChar, args);
					
					}

			
			//If its an error, return
			else if(PID < 0)
			{
				cout << "Failed to fork, returning \n";
				return shell;
				}
			
			//If its a parent process, wait for the child process to finish	
			else
				waitpid(PID, &childEscapeStatus, WUNTRACED);

				
				return shell;
		
		}	
	
	
	
	else if(command.compare("background") == 0)
	{
		ss >> argument;
		string executeString = "";
		int childEscapeStatus;
		
		if(argument[0] == '/')
		{
			//Absolute
			executeString = argument;
			}
			
		else
		{
			//Relative
			 executeString = shell.getShellDir() + '/' + argument;
			}
		
		
		
		const char * executeChar = executeString.c_str();
		if(access(executeChar, X_OK) != 0)
		{
			cout << "File does not exist\n";
			return shell;
			}
			

		pid_t PID = fork();
			
			//If its a child process, use execv to execute the program at the path specified by argument
				//With any parameters given by user passed as one array of args, with arg[0] being the path
				if(PID == 0)
				{
					
					 while(ss >> parameters)
					 {
						 argument.append(" ");
						 argument.append(parameters);
						 }

					char **args = stringToVectorToCharArray(argument);
					execv(executeChar, args);
					
					}

			
			//If its an error, return
			else if(PID < 0)
			{
				cout << "Failed to fork, returning \n";
				return shell;
				}
			
			//If its a parent process, wait for the child process to finish	
			else
			{
				cout<<"PID for this process is " << PID ;
				shell.recordPID(PID);
				waitpid(PID, &childEscapeStatus, WNOHANG);
				}
		
			return shell;
		}
	
	else if(command.compare("murder") == 0)
	{
		ss >> argument;
		pid_t arg = (pid_t)stringToInt(argument);
		int killSuccess = kill( (arg), SIGKILL);
		
		
		if(killSuccess == -1)
		{
			cout << "Failed to kill process\n";
			return shell;
			}
		
		return shell;
		}
	
	else if (command.compare("murderall") == 0)
	{
		int size = shell.getPIDSize();
		int i = 0;
		pid_t currentPID;
		cout << "Murdering " << size << " processes: ";
		for (; i < size; i++)
		{
			currentPID = shell.getPIDAt(i);
			kill(currentPID, SIGKILL);
			cout << currentPID << " ";
			}
			
			shell.erasePIDHistory();
			
			return shell;
			
		}
		
		
	else
	{
		cout << "Error! Unrecognized command. Please try again.";
		return shell;
		}
	
	
}

main()
{
std::string inputString;
char currentDir[500];
string workingDir = getcwd(currentDir, sizeof(currentDir));
Shell shell(workingDir);
cout << "Welcome to Jordan Santacruz's Shell program.\n Please enter input:\n# ";
getline(cin, inputString);
while(inputString.compare("quit") != 0)
	{
		shell = parseString(shell, inputString);
		
		
		cout << "\n# ";
		getline(cin, inputString);
	}

}
