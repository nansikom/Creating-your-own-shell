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
Declaration of the global variables.
*/
int childExitStatus = -5;
char *sourcepath =NULL;
char *targetpath = NULL;
char *args[512];
// creation of a dynamic array to keep all the backgroundprocesses.
pid_t backgroundprocessidarray[MAX_BACKGROUND_PROCESSES];
// see the number you have that you will store in the array
int backgroundprocessesnumber =0;
int foregroundonly = 1;

/*
function from canvas from slides in lecture void handle_SIGINT,void handle_SIGTSTP.
This  function is used when ctr c is pressed.writes the message and pauses for 10 seconds.
*/


void handle_SIGINT(int signo){
	char* message = "Caught SIGINT, sleeping for 10 seconds\n";
	// We are using write rather than printf
	write(STDOUT_FILENO, message, 39);
	sleep(10);
}
/*
function  from canvas slides.https://canvas.oregonstate.edu/courses/1942597. 
This function works when activated by ctlr z and starts activating if we should be running commands in the foregroundonly or not in the
foregroundonly mode.If in foregroundonly mode the & signs are ignored.
Statements are used to show if its enterring in the foreground only mode or if exiting the foregroundonly mode.
*/

void handle_SIGTSTP(int signo){
if(foregroundonly){
    char* message = "Now entering foregroundonly\n";  
    write(STDOUT_FILENO, message, strlen(message));
    foregroundonly = 0;
    }else{
    char* message = " Now exiting foreground only\n";
	// We are using write rather than printf
	write(STDOUT_FILENO, message, strlen(message));
	foregroundonly=1;
    }
}

/*
Major function that does alot of stuff
Declaring of the variables to use in the function
Tokenising of the command and sending it to fork and later to the exec function
split the command into tokens
look for the index and check for the last argument in the command.
If there is and & in the larst argument set the background to 1.In c this means true were actually in the background mode.
remove it before continuing. fork and dup code are from canvas sslides https://canvas.oregonstate.edu/courses/1942597
*/


void execute_command(char *command){
int childExitMethod = -5;
pid_t spawnPid = -5;
int i =0;
// setting last command and background to 0 and the last index to 0.
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


/*
checking if the last index is an & if not in foregroundonly seting is background to 0 so that we wait for the process.
If is its background calling is background by setting it to 1.
*/

 if(strcmp(args[last_arg_index - 1], "&") ==0){
    if(!foregroundonly){
        // remove ampersand before u start doing the exec thing. 
        args[last_arg_index - 1] = NULL;
        is_background = 0;
    }else {
        is_background = 1;
  
//        // remove ampersand before u start doing the exec thing. 
       args[last_arg_index - 1] = NULL;

    }
   
 }


// The rest of your 
/*
Start deining the structs for the signals since they contain some information we need.
https://canvas.oregonstate.edu/courses/1942597

*/

   

// checking the last argument to see if its a an & and also the mode ur actually in.

struct sigaction SIGINT_action = {0}, SIGTSTP_action = {0};

	// Fill out the SIGINT_action struct
	// Register handle_SIGINT as the signal handler
    // set it to ignore and continue so its not interrupted to run.
	SIGINT_action.sa_handler = SIG_IGN;
	// Block all catchable signals while handle_SIGINT is running
	sigfillset(&SIGINT_action.sa_mask);
	// No flags set
	SIGINT_action.sa_flags = 0;

	// Install our signal handler that will process SIGINT for us and save the old previous action if you wish to.
	sigaction(SIGINT, &SIGINT_action, NULL);


    

    // Fill out the SIGTSTP_action struct
	// Register handle_SIGTSTP as the signal handler

	SIGTSTP_action.sa_handler = handle_SIGTSTP;
	// Block all catchable signals while handle_SIGUSR2 is running
	sigfillset(&SIGTSTP_action.sa_mask);
	// No flags set
	SIGTSTP_action.sa_flags = 0;
    // Handle sigaction for SIGTSTP when this signal is recieved.and do what action is defined in the struct.
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

 // fork a new process. 
spawnPid = fork();
switch (spawnPid) {
// Create different cases for the fork  to handle different situation when the fork is released
// Case -1 is when the fork actually fails.
case -1: 
    perror("Hull Breach!\n"); 
    exit(EXIT_FAILURE); 
    break; 
    
case 0: {
    // if its in the backgroud mode  set it to ignore the parent signal.
    if(is_background){
    SIGINT_action.sa_handler = SIG_IGN;
    }else{
        //if it ain't in the background mode set it to default meaning the default for sig default is kill the child process.
       SIGINT_action.sa_handler = SIG_DFL; 
    }
    // set up action in the the sigaction struct to handle what is given unto u.
    sigaction(SIGINT, &SIGINT_action, NULL);
    // foreground only mode
    

	
	
	// Fill out the SIGUSR2_action struct
	// Register handle_SIGUSR2 as the signal handler.Meaning to force it to continue and not to interrupt the ctlr z so that its recieved.
	SIGTSTP_action.sa_handler = SIG_IGN;
    // typically forcing it to stop when ctlr z is recieved.
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);
	// Block all catchable signals while handle_SIGUSR2 is running
    

    // definng of the paths
    int sourceFD= -1, targetFD = -1;
    
    // checking the args array and seeing if two positions back after the current position is the > is for duplicate chap
   for(int j=0; args[j]; j++){
       if(strcmp(args[j], "<")==0){
        // check if there is a file name after the < if its not there print an error message.
        
        if(args[j] == NULL){
            fprintf(stderr,"Missing a file name dear\n");
            break;
        }
        // open the file for read only and it it fails print an error message to screen
        sourceFD = open(args[j+1], O_RDONLY);
        if (sourceFD == -1){
           perror("source open");
           exit(EXIT_FAILURE);
        }
        // loop through the argumennts array and shift the elements two positions ahead
        // This will force to over write the < and filename so we dnt send them to execvp as the commands.
        //This is wat we call command line overwriting.
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
        // check if there is a file name after the > if its not ther say error.
        // this is termed as error handling.
        if(args[j] == NULL){
            fprintf(stderr, "Your missing a file name dear<\n");
            break;
        }
        // open the file with these permissions and if it fails print an error
        targetFD = open(args[j+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (targetFD == -1){
           perror("target open");
           exit(EXIT_FAILURE);
        }
        // shifting the arguments down so they are handled we want exec to handle only arguments like the ls and not  the < or >
        // this helps it handle the exec command only we want exec to only deal with commands
        //
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
   //If all this arguments passed through,dupicate the file.like perform file redirection by duplicating the file desciptor the
   //Your actuallr reading from sTDIN_FILENO and writing to sourceFD.
   // close the file descriptor u dnt need since u already have a duplicate of it may not be so important.
   if(sourceFD != -1){
        dup2(sourceFD, STDIN_FILENO);
        close(sourceFD); 
       
   }
   //Doing the same thing but in the opposite direction.
   if(targetFD != -1){
    dup2(targetFD, STDOUT_FILENO);
    close(targetFD);
   }

   //Call exec on the forked child process because we want to execute the command if the command actually exists      
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


//loop through the processes
    if(backgroundprocessesnumber < MAX_BACKGROUND_PROCESSES){
   
   //store the process id in the backgroundprocessidarray at the index of the backgroundprocessnummber so ur putting them in the right
   //positions they should be
    backgroundprocessidarray[backgroundprocessesnumber] = spawnPid;
    // Print the background process ID onto the terminal.
    printf("background process id is %d\n", spawnPid);
    //increment it to keep the number of process ID's in he array.
    backgroundprocessesnumber++;
    
    }else{
        printf("Error: Dude you have too many background processes\n");
   }
}else{
// else id its not a background process wait for it to finish since its in the foreground mode.,
    waitpid(spawnPid, &childExitStatus, 0);
   
//       if(WIFEXITED(childExitStatus))
//   {
//           printf("Child process exited normally with exit status  %d\n", WEXITSTATUS(childExitStatus));
//       }
     if (WIFSIGNALED(childExitStatus)){
              printf("PID %d The process was terminated by a signal %d\n",spawnPid, WTERMSIG(childExitStatus));
                    
    

}
  }
break;

}
}
}

/*
after all this happens u need to not wait and just prompt to the next prompt
*/
int main (){
    
while(1){
       //this will only be activated if its a background process that will be added to the backgroundid array with t=a process number.
       // loop through all the background processes in the backgroundIdarray till u reach that ka exact process kononyaaa akoo kenyini.
        for (int i = 0; i < backgroundprocessesnumber; i++) {
        int status = -5;
    
        // index array and add in the process ID numbers  to the array.
        //for each of all of the background processes call it with a no hang option.
        // set upp a variable and dont wait for the process to finish returns command line access immediately.
        pid_t childpid= waitpid(backgroundprocessidarray[i], &status, WNOHANG);
            //print exit status or signal termination
            // declare status and add it the process.
            //if waitpid returns a value greater than 0 we know the process has finished or even terminated.
            //we check the exit status and its signal if it was terminated by a signal.
               if(childpid > 0){
                if (WIFEXITED(status) != 0)
                    printf("PID  %d Exit value  %d\n",childpid, WEXITSTATUS(status));
                if (WIFSIGNALED(status) != 0)
                    printf(" PID %d The process was terminated by a signal %d\n",childpid, WTERMSIG(status));
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
            char *position;
        
        // replacing each occurence of $$ with a process ID
        //this is used to make the space of the process ID by moving anything after $$ to the right just to make sure the process ID is actually written
            while((position = strstr(command, "$$")) != NULL){
                memmove(position + strlen(pid), position + 2, strlen(position + 2) + 1);
                //copy the process ID by the space u created using memoveeeeee.
                memcpy(position, pid, strlen(pid));
            }
           strcpy(command_copy,command);
        
        //tokenise the command if u find a command and increment it solong as there is a command in the terminal
            char *tokenptr = strtok(command_copy, "\n");
            while(tokenptr !=NULL){
            tokenptr = strtok(NULL, " \n");
            tokencount++;
            }
       
            
            
            
        //check for the blank line and the comments.
         
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
    
        //printf("Command name is %s\n", command_name);
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
                printf("pwd is %s\n", args[1]); fflush(stdout);
                chdir(args[1]);

            
            }else{
               printf("pwd is %s\n", args[1]);
               chdir(getenv("HOME")); 
              printf("pwd is %s\n", args[1]);
               //printf("Command name is %s\n", command_name);
            }
            continue;
        }
        
        // check for the status command and see how it exited , by signal or by exit.
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
         

     // free all the paths when done this is useless since I dont have those paths anymore.
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

    //run through background processes
    // call waitpid WNOHANG on all of them, any that return their PID have died, so remove them frmo the aarrray
    
}

}

 


