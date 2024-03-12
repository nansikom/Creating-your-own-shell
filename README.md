#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
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
// force checking till you find a null.
//find the index of the last argument in a command
while(args[last_arg_index] !=NULL){
    last_arg_index++;
}
// checking the last argument to see if its a an &.
if(strcmp(args[last_arg_index - 1], "&")==0){
    is_background = 1;
    // remove ampersand before u start doing the exec thing. 
    args[last_arg_index - 1] = NULL;

} 
spawnPid = fork();
switch (spawnPid) {
case -1: 
    perror("Hull Breach!\n"); 
    exit(EXIT_FAILURE); 
    break; 
    
case 0: {
    // definng of the paths
    int sourceFD= -1, targetFD = -1;
    // checking the args array and seeing if two positions back after the current position is the > is for duplicate chap
    for(int j=0; j < i; j++){
       if(strcmp(args[j], "<")==0){
        sourceFD = open(args[j+1], O_RDONLY);
        if (sourceFD == -1){
           perror("source open");
           exit(EXIT_FAILURE);
        }
        for(int k=j; k < i-1;k++){
            args[k] = args[k+2];
        }
        i -=2;
        
    } 
    
    
     
    // two points behind and you meet the >.
    if(strcmp(args[j], ">" )==0){
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
        */
         for(int k=j; k < i-1;k++){
            args[k] = args[k+2];
        }
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
// printf("PARENT(%d): Sleeping for 2 seconds\n", getpid());
//sleep(2);
 //printf("PARENT(%d): Wait()ing for child(%d) to terminate\n", getpid(), spawnPid);
/*
Hey add the process ID if its a background process to the array of background processes
*/
// when flag was upset.
if (is_background == 1){
    if(backgroundprocessesnumber < MAX_BACKGROUND_PROCESSES){
    // resize the array to add in the new process ID to the array
   // backgroundprocessid = realloc(backgroundprocessid, sizeof(pid_t) * (backgroundprocessesnum + 1));
    backgroundprocessidarray[backgroundprocessesnumber] = spawnPid;
    // increment it to keep the number of process ID's in he array.
    printf("background process id is %d\n", spawnPid);
    backgroundprocessesnumber++;
    
   }else{
        printf("Error: Dude you have too many background processes\n");
    }
} else {
    waitpid(spawnPid, &childExitStatus, 0);
}

//printf("PARENT(%d): Child(%d) terminated, Exiting!\n", getpid(), actualPid);

break;
}
}
}

/*
after all this happens u need to not wait and just prompt to the next prompt
*/
int main (){
while(1){
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
            isforeground = 0;
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

    //run through background processes
    // call waitpid WNOHANG on all of them, any that return their PID have died, so remove them frmo the list
    for (int i = 0; i < backgroundprocessesnumber; i++) {
        int status = -5;
    
        // index array and add in the process ID numbers  to the array.
        pid_t childpid= waitpid(backgroundprocessidarray[i], &status, WNOHANG);
            //print exit status or signal termination
            // declare status and add it the process.
               if(childpid > 0){
                if (WIFEXITED(status) != 0)
                    printf("Exit value  %d\n", WEXITSTATUS(status));
                if (WIFSIGNALED(status) != 0)
                    printf("The process was terminated by a signal %d\n", WTERMSIG(status));
               }      
            // do I have to actually do this myself to make sure they have died
            
        
    }
}
//return 0;
}






















         
      
