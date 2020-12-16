#include <iostream>
#include <vector>
#include <ctime>
#include <unistd.h> 
#include <sys/types.h>
#include <cerrno>
#include <sys/wait.h>
#include <signal.h>

int main(){

	srand(time(NULL));
	
	int n;
	std::cout << "Write size of array with random numbers." << std::endl;
	std::cin >> n;
	
	std::vector<int> v(n);
	for(int i = 0 ; i < n; ++i){
		v[i] = rand()%2001 - 1000; // from -1000 to 1000
	}
	
	int m;
	std::cout << "Write number of parallel child to calculate the sum." << std::endl;
	std::cin >> m;

		
	int step = n/m; // count of numbers witch every child process have to read
	
	int ij[2];

	ij[0] = 0; // i
	if(n%m != 0){ // the first child have to count more than n/m numbers  
		ij[1] = n%m + step; // j
	}
	else{
		ij[1] = step; // j
	}

	int result = 0; // final result

	for(int x = 0; x < m; ++x){ //  create m child process with one parent 		

		int pipefd_1[2]; // from parent to child. Parent sends to child i and j 

		int pipefd_2[2]; // from child to parent. Child sends to parent sub-total sum from i to j 	

		int pipe_result_1 = pipe(pipefd_1); // create first pipe
		if(pipe_result_1 != 0){
			exit(errno);
		}

		int pipe_result_2 = pipe(pipefd_2); // create  second pipe
		if(pipe_result_2 != 0){
				exit(errno);
		}	


		int pid = fork(); // create new child process
		if(pid == -1){
			exit(errno);
		}

 		if(pid == 0) { // child process
			int ij[2]; 

			close(pipefd_1[1]); // child can only read i, j

			int readBytes = read(pipefd_1[0], &ij, sizeof(int)*2);
			if(readBytes == -1){
				exit(errno);
			}
				
			int subtotal_sum = 0;

			for(int y = ij[0]; y < ij[1]; ++y){ // calculate subtotal sum
				subtotal_sum += v[y];		
			}
				
			close(pipefd_2[0]); // child can only write the sum

			std::cout << "Child number " << x + 1 << " calculated result: " << subtotal_sum << std::endl;
				
			int writeBytes =  write(pipefd_2[1], &subtotal_sum, sizeof(int)); // send subtotal_sum to parent
			if(writeBytes == -1){ 
				exit(errno);
			}	
			
			kill(getpid(), SIGKILL); // kill all child after their work
				//wait(NULL);
				
		}
			
		else { // parent process
				close(pipefd_1[0]); // parent can only write i, j

				int writeBytes =  write(pipefd_1[1], &ij, sizeof(int)*2);
				if(writeBytes == -1){ 
					exit(errno);
				}	
				// wait(NULL);

				close(pipefd_2[1]); // parent can ony read the sum

				int cur = 0;
				int readBytes = read(pipefd_2[0], &cur, sizeof(int));
				if(readBytes == -1){
					exit(errno);
				}
							
				result += cur;
				
				ij[0] = ij[1];
				ij[1] += step;

				wait(NULL);			
		} 
		
	} // end of for
	std::cout << "Result: " << result << std::endl;
}
