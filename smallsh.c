#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#define MAX_BACKGROUND_PROCESSES 100
/*
function from canvas from slides in lecture
*/
int childExitStatus = -5;
char *sourcepath =NULL;
char *targetpath = NULL;
char *args[512];
// creation of a dynamic array to keep all the backgroundprocesses.
pid_t backgroundprocessidarray[MAX_BACKGROUND_PROCESSES];
// see the number you have that you will store in the array
int backgroundprocessesnumber =0;
int foregroundonly = 0;



void handle_SIGINT(int signo){
	char* message = "Caught SIGINT, sleeping for 10 seconds\n";
	// We are using write rather than printf
	write(STDOUT_FILENO, message, 39);
	sleep(10);
}

void handle_SIGTSTP(int signo){
if(foregroundonly){
    char* message = "Entering foreground only mode(This should be ignored&)\n";  
    write(STDOUT_FILENO, message, 50);
    foregroundonly = 0;
    }else{
    char* message = "Exiting forreground only mode\n";
	// We are using write rather than printf
	write(STDOUT_FILENO, message, 39);
	foregroundonly=1;
    }
}




void execute_command(char *command){
int childExitMethod = -5;
pid_t spawnPid = -5;
int i =0;
// setting last command and background to 0.
int is_background =0;
int last_arg_index =0;

char *token = strtok(command, " \t\n");
// while there was a command split the token that way you can actually send it to the fork and execvp.
while(token !=NULL){
    args[i++] = token;
    token = strtok(NULL, " \t\n");
}
args[i] = NULL;
 //sleep thing
//i--; 
// force checking till you find a null.
//find the index of the last argument in a command
while(args[last_arg_index] !=NULL){
    last_arg_index++;
}



 if(strcmp(args[last_arg_index - 1], "&")==0){
     is_background = 1;
     // remove ampersand before u start doing the exec thing. 
     args[last_arg_index - 1] = NULL;

 } 


   

// checking the last argument to see if its a an & and also the mode ur actually in.

struct sigaction SIGINT_action = {0}, SIGTSTP_action = {0};

	// Fill out the SIGINT_action struct
	// Register handle_SIGINT as the signal handler
	SIGINT_action.sa_handler = SIG_IGN;
	// Block all catchable signals while handle_SIGINT is running
	sigfillset(&SIGINT_action.sa_mask);
	// No flags set
	SIGINT_action.sa_flags = 0;

	// Install our signal handler
	sigaction(SIGINT, &SIGINT_action, NULL);


    

    // Fill out the SIGUSR2_action struct
	// Register handle_SIGUSR2 as the signal handler
	SIGTSTP_action.sa_handler = handle_SIGTSTP;
	// Block all catchable signals while handle_SIGUSR2 is running
	sigfillset(&SIGTSTP_action.sa_mask);
	// No flags set
	SIGTSTP_action.sa_flags = 0;
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

  
spawnPid = fork();
switch (spawnPid) {
case -1: 
    perror("Hull Breach!\n"); 
    exit(EXIT_FAILURE); 
    break; 
    
case 0: {
    // if its in the backgroud mode  set it to ignore the parent signal
    if(is_background){
    SIGINT_action.sa_handler = SIG_IGN;
    //sigaction(SIGINT, &SIGINT_action, NULL);
    }else{
        //if it ain't in the background mode set it to default meaning the default for sig default is kill the child process.
       SIGINT_action.sa_handler = SIG_DFL; 
    }
    sigaction(SIGINT, &SIGINT_action, NULL);
    // foreground only mode
    

	
	
	// Fill out the SIGUSR2_action struct
	// Register handle_SIGUSR2 as the signal handler
	SIGTSTP_action.sa_handler = SIG_IGN;
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);
	// Block all catchable signals while handle_SIGUSR2 is running
	

    // definng of the paths
    int sourceFD= -1, targetFD = -1;
    
    // checking the args array and seeing if two positions back after the current position is the > is for duplicate chap
   for(int j=0; args[j]; j++){
       if(strcmp(args[j], "<")==0){
        if(args[j] == NULL){
            fprintf(stderr, "Your missing a file name dear<\n");
            break;
        }
        sourceFD = open(args[j+1], O_RDONLY);
        if (sourceFD == -1){
           perror("source open");
           exit(EXIT_FAILURE);
        }
        for(int k=j; args[k] && args[k+2];k++){
            args[k] = args[k+2];
        }
        
        args[i-2] = NULL;
        i -=2;
        
    } 
   }
    
    
     
    // two points behind and you meet the >.
    for(int j=0; args[j]; j++){
    if(strcmp(args[j], ">" )==0){
        if(args[j] == NULL){
            fprintf(stderr, "Your missing a file name dear<\n");
            break;
        }
        targetFD = open(args[j+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (targetFD == -1){
           perror("target open");
           exit(EXIT_FAILURE);
        }
       
        /*
        start the loop,continue until u reach the second to last argument,continue iteration
        move the elements in the args array by to positions thus removing the > and filename since there being over written.we dont waht them to come back
        Trying to perform this to move the elements in the args array by two positions thus removing the > and filename since there being over written. 
        // move elements and over write them till they > and filename are dead
        */
         for(int k=j; args[k] && args[k+2];k++){
            args[k] = args[k+2];
        }
        args[i-2] = NULL;
        i -=2;
       
    }
    }

   if(sourceFD != -1){
        dup2(sourceFD, STDIN_FILENO);
        close(sourceFD); 
       
   }
   if(targetFD != -1){
    dup2(targetFD, STDOUT_FILENO);
    close(targetFD);
   }

         
    if(execvp(args[0], args)==-1){
    perror("Command not found\n");
    exit(EXIT_FAILURE); 
    } 
    
}
 default:{

/*
Hey add the process ID if its a background process to the array of background processes
*/
// when flag was upset.
if (is_background == 1){
   // waitpid(spawnPid, &childExitStatus, 0);
    if(backgroundprocessesnumber < MAX_BACKGROUND_PROCESSES){
   
    backgroundprocessidarray[backgroundprocessesnumber] = spawnPid;
    // increment it to keep the number of process ID's in he array.
    printf("background process id is %d\n", spawnPid);
    //waitpid(spawnPid, &childExitStatus, WNOHANG);
    //waitpid(spawnPid, &childExitStatus, 0);
    backgroundprocessesnumber++;
    
    }else{
        printf("Error: Dude you have too many background processes\n");
   }
}else{

    waitpid(spawnPid, &childExitStatus, 0);
     if (WIFSIGNALED(childExitStatus)){
              printf("PID %d The process was terminated by a signal %d\n",spawnPid, WTERMSIG(childExitStatus));
                    
    

}
//printf("PARENT(%d): Child(%d) terminated, Exiting!\n", getpid(), actualPid);
  }//else{

break;

}
}
}

/*
after all this happens u need to not wait and just prompt to the next prompt
*/
int main (){
    
while(1){

        for (int i = 0; i < backgroundprocessesnumber; i++) {
        int status = -5;
    
        // index array and add in the process ID numbers  to the array.
        pid_t childpid= waitpid(backgroundprocessidarray[i], &status, WNOHANG);
            //print exit status or signal termination
            // declare status and add it the process.
               if(childpid > 0){
                if (WIFEXITED(status) != 0)
                    printf("PID  %d Exit value  %d\n",childpid, WEXITSTATUS(status));
                if (WIFSIGNALED(status) != 0)
                    printf("PID %d The process was terminated by a signal %d\n",childpid, WTERMSIG(status));
               }      
            // do I have to actually do this myself to make sure they have died
           
        }
        
    
        //getting the command and trying to display the command
        char command[2048];
        char command_copy[2048];
        printf(": ");
        fflush(stdout);
        memset(command, '\0', sizeof(command));
        fgets(command, sizeof(command), stdin);
        // I wanna string coppy because strtok modifies the string  and I dont want it to be  modified by strtok.
        //strcpy(command_copy,command);
        
        int tokencount = 0;
        int isforeground =1;
        char pid[16];
            snprintf(pid, sizeof(pid), "%d", getpid());
            //snprintf(command, sizeof(command), "%s%s", command,pid);
            char *position;
            while((position = strstr(command, "$$")) != NULL) {
                memmove(position + strlen(pid), position + 2, strlen(position+2) + 1);
                memcpy(position, pid, strlen(pid));
        }
            
           strcpy(command_copy,command);
        

            char *tokenptr = strtok(command_copy, "\n");
            while(tokenptr !=NULL){
            tokenptr = strtok(NULL, " \n");
            tokencount++;
            }
       
            
            
            

         
        if (command[0] == '#'){
            continue;
        }
        int isBlank = 1;
        for (int i=0;i < strlen(command); i++){
            if(command[i] != ' ' && command[i] != '\n'){
                isBlank = 0;
                break;
            }
        }
        if (isBlank){
            continue;
        }
  
        char *command_name= strtok(command, "\t\n");
        if (command_name == NULL) {
            continue;
        }
        if(strcmp(command_name, "exit")==0){
           
        int status;
          for(int i=0; i < backgroundprocessesnumber; i++){
             waitpid(backgroundprocessidarray[i], &status, 0);
             if (WIFEXITED(status)){
                    printf("PID  %d Exit status  %d\n",backgroundprocessidarray[i], WEXITSTATUS(status));
             }else if (WIFSIGNALED(status)){
                    printf("PID %d The process was terminated by a signal %d\n",backgroundprocessidarray[i], WTERMSIG(status));
                
             }
          }
            break;
           // exit(0);
        
          }
        
        

        else if(strcmp(command_name, "cd")==0){
            char path;
            if(args[1] != NULL){
                chdir(args[1]);
            
            }else{
               chdir(getenv("HOME")); 
            }
            continue;
        }
      else  if (strcmp(command_name, "status") == 0) {
            if (childExitStatus == -5) {
                printf("Process exited with status of 0");
            } else {
                if (WIFEXITED(childExitStatus) != 0)
                    printf("Exit value  %d\n", WEXITSTATUS(childExitStatus));
                if (WIFSIGNALED(childExitStatus) != 0)
                    printf("The process was terminated by a signal %d\n", WTERMSIG(childExitStatus));
                    
            }
            isforeground = 1;
            continue;
        }
         


        else{
            isforeground = 1;
            execute_command(command);
        }
        if (sourcepath != NULL){
            free(sourcepath);
            sourcepath = NULL;

        }
        if (targetpath != NULL){
            free(targetpath);
            targetpath = NULL; 

        }

  
}
}

 



















         
      

















         
      


 


