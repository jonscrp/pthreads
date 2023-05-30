#include <iostream>
#include <string>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <vector>


using namespace std;

// Global variables
string all = ""; 
string txt[] = {"", "", "", ""};
vector<string> input;
const int SIZ = 10;
int f2;

// Parameter class  
class MyParam{

	public:
		MyParam(int l, int u, int i): lb(l), ub(u), lid(i) {};
		int getLower() { return lb;}
		int getUpper() { return ub;}
		int getLid()   { return lid;}
  	
  	private:
  		int lb, ub, lid;
};

// ptread function
void *myrunner(void *param){

	MyParam *p = (MyParam *) param;
	
	// Parameters: lower bound, upper bound, and thread logical id
	int lower = p -> getLower();
	int upper = p -> getUpper();
	int idx   = p -> getLid();  	
	
	
	// string instructions -> int intstructions 
	int numbers[] = {0,0,0}; 
	
	for( int i = lower-1; i < upper; i++){
		string number = "";
		int c = 0;
		int p = 0;
		//string res = "Thread " + to_string(idx) + " reads " + input[i]+ "\n";
		//cout << res;
		while(input[i][c] != '\0'){	
			if( !(int(input[i][c]) > 47 && int(input[i][c]) < 58)){
				numbers[p] = stoi(number);
				number = "";
				p++;
				c++;
			} else {
				number += input[i][c]; 
				c++;
			}
		}
		numbers[p] = stoi(number);
		
		// read from prose that txt.
		char buff[numbers[2]];
		//lseek(f2,numbers[1],numbers[0]);
		int u;
		if( 0 > (u =  pread(f2, buff, numbers[2], numbers[1]))){
			cout << "error" << endl;
		}
		string res0 = " ";
		for( char o: buff){
			txt[idx] +=  o;
			//res0 += o;
		} 
		string res = "pthread " + to_string(idx) + " execs instructions " + to_string(i)+  " : " + input[i] + "\n" ;
		cout << res;
		txt[idx] += " ";    
	}	
	
	string endt = "Finished work in thread " + to_string(idx) + "\n";
	cout << endt;
	pthread_exit(0);
}

int main(int argc, const char * argv[]){

	if (argc != 3){
		write(STDERR_FILENO, "Wrong number of command line arguments\n", 36);
		return 1;
	}

	// prose file
	if((f2 = open(argv[2],O_RDONLY, 0)) == -1){
		write(STDERR_FILENO, "Can't open input file\n", 28);
		return 2;
	}

	// Main Thread reads Instructions file
	cout << "Main thread instructions read from instructions.txt: " << endl;
	int f1;
	if((f1 = open(argv[1], O_RDONLY, 0)) == -1){
		write(STDOUT_FILENO, "Can't open input file\n", 28);
		return 0;
	}
	// Algorithm to read the instructions file
	int n;
	char buf[SIZ];
	int r = 0;
	string line = "";
	while((n = read(f1, buf, SIZ)) > 0){
		
		r = 0;
		while(r < sizeof(buf)){
			if(buf[r] == '\n' )
			{
				cout << line << endl;
				input.push_back(line);
				line = "";
				r++;
			}
			else
			{
				if(buf[r] == 'o')
				{
				 	input.push_back(line);
				 	break;
				}
				else
				{
					
					line += buf[r];
					buf[r] = 'o'; 
					r++;
				}
			}
		}
	}
	cout << line << endl;
	input.push_back(line);

	// Create the parameters for each of the 4 pthreads
	MyParam *p[4];
	p[0] = new MyParam(1,  5,  0);
	p[1] = new MyParam(6,  10, 1);
	p[2] = new MyParam(11, 15, 2);
	p[3] = new MyParam(16, 20, 3);


	// Create pthreads
	pthread_t tid[4];
	pthread_attr_t attr[4];
	
	cout << "Creating Threads" << endl;
	for( int i = 0; i < 4; i++){
		string res3 = "Thread " + to_string(i) + " created\n";
		cout << res3;
		pthread_attr_init(&(attr[i]));
		pthread_create(&(tid[i]), &(attr[i]), myrunner, p[i]);
	}

	// Wait for pthreads to finish
	for( int i = 0; i < 4; i++){
		pthread_join(tid[i], NULL);
		string res2 = "Joined thread " + to_string(i) + "\n";
		cout << res2;
	}
	
	
 	// Get and print final txt
 	for ( int i = 0; i < 4; ++i)
 		all +=  txt[i];
 	
 	cout << "Joined String: " << all << endl;
 	
 	
 	// Cleanup
 	for( int i = 0; i < 4; ++i) delete p[i];
	// Close the file descriptor 						
	close(f1);
	close(f2);
	return 0;
}