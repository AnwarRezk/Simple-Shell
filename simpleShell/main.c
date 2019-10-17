#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#define MAX_SIZE 50
#define MAX_CHARS 100

///Command Handling Functions

//a funtion to check child type
//background or not (&)
int check_bgp(char line[]){
int bgp = 0;
if(line[strlen(line)-1] == '&'){
bgp = 1; //background process
line[strlen(line)-1] = '\0';
}
return bgp;
}

//a function to remove the Enter from
//string
void remove_nl(char line[]){
int i=0;
while(line[i] != '\n'){
i++;
}
line[i]='\0';
}

//a function to read the whole command
//from console
void read_line(char line[]){
printf("~$ ");
fgets(line,MAX_CHARS,stdin);
remove_nl(line);
if(strcmp(line,"exit") == 0){
exit(0);}
}

//a function to compute the arguments
//command + parameters
int parse_line(char* args[],char line[]){
int i=0;
args[i] = strtok(line," ");

if(args[i] == NULL){
printf("No Command Entered!\n");
return 1;
}


while(args[i] != NULL){
args[++i] = strtok(NULL," "); //check
}
return 1;
}

//a function to read the command and handle
//arguments
int read_cmd(char* args[],char line[]){
read_line(line);
int flag = check_bgp(line);
parse_line(args,line);
return flag;
}

//int cd_handle(char line[]){}

///Signal Handling Function

void signal_handler(){
FILE *f = fopen("log_file.txt","a");//open the file for stating child death
time_t time_x;
struct tm *time_info;
time(&time_x);
time_info = localtime(&time_x); //get current time
fputs("\nChild was dead at : ",f); //type in the file when child died
fputs(asctime(time_info),f);
fclose(f); //close the file stream
}

//===================================================//


void main()
{
    char line[MAX_CHARS]; //whole command as one string
    char *args[MAX_SIZE]; //array of pointers to char holding arguments of cmd

    int status; //status for waiting
    int f; //child type check


    //shell session loop
    while(1){

        f = read_cmd(args,line); //returns a flag of child type(bg or not)
        pid_t PID = fork(); //forking a child

        if(PID < 0){printf("An Error has occured,Try Again!\n");}

        else if(PID == 0){ //child
            if(f){setpgid(0,0);} //set process group id to 0 to use id of calling process
            //execvp(args[0],args);
            if(execvp(args[0],args) == -1){
                printf("Command Cannot Be Found!\n");
            }
        }
        else{ //parent
        signal(SIGCHLD,signal_handler);//handle child death and write to log file
        if(f){tcsetpgrp(0,getpgrp());} //called by member of background process to avoid blocking
        else{wait(&status);}
        }
     }
}
