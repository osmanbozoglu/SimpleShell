

// 220201048 OSMAN BOZOGLU - 230201035 MERVE BOZOGLU

// findloc function does not work.
// ls -l | wc -l etc. does not work.
// any other things works successfully.



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>




#define BUFFER_SIZE 100
#define READ_END 0
#define WRITE_END 1


//node for history
struct history_queue
{
	char command[100];
	struct history_queue *next;
};



int cd_directory(char *path);
int dir();
void add_queue(char str[100]);
void print_history();
bool is_myshell_command(char *str);

struct history_queue *head = NULL;

int main(int argc, char const *argv[])
{
	head = (struct history_queue*)malloc(sizeof(struct history_queue));
	while(1){
		printf("myshell>");
		char str[100];
		fgets(str,100,stdin);
		add_queue(str);
		char *token = strtok(str," \n");
		
				while(token != NULL){
				if (is_myshell_command(token))
				{
				if(!strcmp(token,"cd")){
					token = strtok(NULL," \n");
					cd_directory(token);
				}
				if (!strcmp(token,"dir"))
					dir();

				if (!strcmp(token,"history"))
					print_history();
				
				if (!strcmp(str,"bye"))
						exit(0);
			}
				else{
					char* argv[BUFFER_SIZE];
					int i=0;
					while( token != NULL ) { // assigning the commands in argv array
    					argv[i] = token;
    					i++;
    					token = strtok(NULL, " \n");
    					} 
					argv[i] = NULL; 
					int fd[2]; 
					pipe(fd); // creating pipe
					pid_t pid = fork(); //creating child process
					if (pid > 0)
					{
						//parent process sends the command to the child.
						close(fd[READ_END]);

						write(fd[WRITE_END], argv, BUFFER_SIZE);
						close(fd[WRITE_END]);
						if(strcmp(argv[i-1], "&") == 0) // checks the process is backgorund or foreground. If background, parent does not wait the child process.
							continue;
						else
							wait(NULL); // the process is foreground. Parent waits.
					}

					else{
						//child process read commands from parent
						char *read_comm[BUFFER_SIZE];
						close(fd[WRITE_END]);
						read(fd[READ_END], read_comm, BUFFER_SIZE); 
						close(fd[READ_END]);

						if(execvp(read_comm[0],read_comm) == -1) // child process runs the command
							printf("Error in calling exec!!\n");
						exit(0); // terminating child process

					}
				}
				token = strtok(NULL," \n");
			}
		}
		

	return 0;

}

bool is_myshell_command(char *str){ // this function for the given command is in myshell or not.
	const char *myshell_elements[] = {"cd","dir","history","findloc","bye"};
	int len = sizeof(myshell_elements)/sizeof(myshell_elements[0]);
	for (int i = 0; i < len; ++i)
	{
		if(strcmp(str,myshell_elements[i]) == 0){
			return true;
		}
	}
	return false;
}


int cd_directory(char *path){
	//this function changes the current directory.
    if( chdir( path ) == 0 ) {
        printf( "Directory changed to %s\n", path );
        return EXIT_SUCCESS;
    } else {
        perror( path );
        return EXIT_FAILURE;
    }
}


int dir(){ // for getting the current directory
	char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
    } 
    else {
       perror("getcwd() error");
       return 1;
   }
}


//This functions checks that the size of history queue is 10 or not.
bool is_history_full(){
	int counter = 0;
	struct history_queue *temp = head->next;
	while(temp!=NULL){
		counter++;
		temp = temp->next; 
	}
	if(counter == 10)
		return true;
	return false;
}

void add_queue(char str[100]) 
{
 	if (is_history_full()) // if the size of queue is equals 10, then this removes the queue of first element
    {
    	struct history_queue *tmp = head->next;
    	head->next = head->next->next;
    	free(tmp); 
    } 
    
    struct history_queue* new_node = (struct history_queue*) malloc(sizeof(struct history_queue)); // adding new node to history 
  
    strcpy(new_node->command,str); // copying the command to the node
  
    new_node->next = NULL; 
  
    if (head->next == NULL) // adding first node
    { 
       head->next = new_node; 
       return; 
    }
   
	struct history_queue *last = head->next;

    while (last->next != NULL) 
        last = last->next; 
  
    last->next = new_node; 
    return; 
} 



// this functions prints the history.
void print_history(){
	struct history_queue *temp = head->next;
	int counter = 1;
	while(temp!=NULL)
    {
    printf("[%d]%s",counter,temp->command);
    temp=temp->next;
    counter++;
    }
    free(temp);
}


