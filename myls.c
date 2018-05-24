/*On my honor, I have neither given nor received any academic 
aid or information that would violate the Honor Code of Mars 
Hill University.
*/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

void processArgs(int argc, char **argv, int *aFlag, int *lFlag, int *FFlag, int *RFlag, int *iFlag, char *name);
void printProperFormat(char *name, char *fullName, struct stat *statStruct, int lFlag, int FFlag, int iFlag);
void processDir(char *name,  int aFlag, int lFlag, int FFlag, int RFlag, int iFlag);

/****************************************************************************************************
main()
 ****************************************************************************************************/
int main(int argc, char *argv[]){
  //flags to set when options are selected at command line
  int aFlag=0;
  int lFlag=0;
  int FFlag=0;
  int RFlag=0;
  int iFlag=0;
  char name[1024]=".";
  
  processArgs(argc, argv, &aFlag, &lFlag, &FFlag, &RFlag, &iFlag, name);
  processDir(name, aFlag, lFlag, FFlag, RFlag, iFlag);
}

/****************************************************************************************************
If the file is a regular file, processDir simply calls printProperFormat.
Otherwise, it traversed the directory and calls printProper format on each item.
If the item is a directory and the -R option is set, it adds the name to a list for later traversal.
****************************************************************************************************/
void processDir(char *name, int aFlag, int lFlag, int FFlag, int RFlag, int iFlag){
  DIR *dp;
  struct  dirent *de;
  struct stat statStruct;
  int exists;
  char slName[256];
  struct tm *tmp;
  char outstr[200];
  char fullName[1024];

  errno=0;
  exists=lstat(name, &statStruct);
  
  if(exists<0){//if lstat failed
    perror("myls");
    exit(1);
  }
  else{
    if(S_ISDIR(statStruct.st_mode)){	  
      errno=0;
      dp=opendir(name);  //attempt to open the directory
      
      if(!dp)
	perror("myls");
      
      else{ //directory can be opened
	while((de=readdir(dp))){//traverse the directory
	  if(de->d_name[0]!='.' || aFlag){//do we need to try to print the file info?
	    //build a full *relative* path for this file name so stat won't fail
	    strcpy(fullName, name);//copy the directory name to fullpath as a starting point
	    
	    if(fullName[strlen(fullName)-1]!='/')//only add the slash between directories/files if it isn't there already
	      strcat(fullName, "/");//if user calls program with "/" or "/home/" as argument, this will apply
	    strcat(fullName, de->d_name);//finally, add the name of the file/subdirectory to the end of the path
	    //end building the path
	    
	    exists=lstat(fullName, &statStruct);
	    
	    if(exists<0)//this will be rare, since a file would have to be removed after we started traversing the directory for stat to fail
	      printf("%s does not exist\n", de->d_name);
	    
	    else
	      printProperFormat(de->d_name, fullName, &statStruct, lFlag, FFlag, iFlag);
	  }
	}
	printf("\n");
	closedir(dp);
      }
    }
    else
      printProperFormat(name, name, &statStruct, lFlag, FFlag, iFlag);
    
    //after we traverse the current directory 
    if(!lFlag)
      printf("\n");//only print a newline if we printed a regular listing, not long listing (long listing will print newline at end anyway)
  }
}
/****************************************************************************************************
scans the command line arguments and sets flags as needed for these options
****************************************************************************************************/
void processArgs(int argc, char **argv, int *aFlag, int *lFlag, int *FFlag, int *RFlag, int *iFlag, char *name){
  int i;
  int j;
  
  for(i=1;i<argc;i++){
    if(argv[i][0]!='-')
      strcpy(name, argv[i]);
    else{
      j=1;
      while(argv[i][j]){
	if(argv[i][j]=='a')
	  *aFlag=1;
	else if(argv[i][j]=='l')
	  *lFlag=1;
	else if(argv[i][j]=='F')
	  *FFlag=1;
	else if(argv[i][j]=='R')
	  *RFlag=1;
	else if(argv[i][j]=='i')
	  *iFlag=1;
	else{
	  printf("myls: Error: incorrect option: %c\n", argv[i][j]);
	  exit(1);
	}
	j++;
      }
    }
  }
}

/****************************************************************************************************
Decides, based on flags, how the output should look
****************************************************************************************************/
void printProperFormat(char *name, char *fullName, struct stat *statStruct, int lFlag, int FFlag, int iFlag){

  struct stat info;
  char realname[1024];
  struct tm *tmp;
  char outstr[200];
  struct passwd *ps;
  char *group;
  struct group *gs;


   /*
    using the flags, determine the proper format and display the item (file, link, or directory)
    use the macros like S_IRUSR, and S_IFDIR to display the drw-r--r-- and /, *, or @ info. 
    use the example to see how to convert the time information to a nicely formatted string
  */ 
	if(iFlag == 1){
         printf("%lu ", statStruct->st_ino);
     }
 
     if(lFlag == 1){
         ps = getpwuid(statStruct->st_uid);
         gs = getgrgid(statStruct->st_gid);
		 if(S_ISDIR(statStruct->st_mode)){
             printf("d");
         }else if(S_ISLNK(statStruct->st_mode)){
             printf("l");
         }else if(S_ISFIFO(statStruct->st_mode)){
             printf("p");
         }else{
             printf("-");
         }
	//Found this on Stack overflow
     //https://stackoverflow.com/questions/10323060/printing-file-permissio    ns-like-ls-l-using-stat2-in-c
     //I know i could have used a bunch of if else statments
         printf( (statStruct->st_mode & S_IRUSR) ? "r" : "-");
         printf( (statStruct->st_mode & S_IWUSR) ? "w" : "-");
         printf( (statStruct->st_mode & S_IXUSR) ? "x" : "-");
         printf( (statStruct->st_mode & S_IRGRP) ? "r" : "-");
         printf( (statStruct->st_mode & S_IWGRP) ? "w" : "-");
         printf( (statStruct->st_mode & S_IXGRP) ? "x" : "-");
         printf( (statStruct->st_mode & S_IROTH) ? "r" : "-");
         printf( (statStruct->st_mode & S_IWOTH) ? "w" : "-");
         printf( (statStruct->st_mode & S_IXOTH) ? "x" : "-");
		
		 printf(" %d ", statStruct->st_nlink);
		 printf(" %s ", ps->pw_name);
		 printf(" %s ", gs->gr_name);
		 printf(" %d ", statStruct->st_size);
  		
         tmp = localtime(&statStruct->st_mtime);
 
         if(!tmp){
             perror("localtime");
             exit(EXIT_FAILURE);
         }
         char tmstr[128];
 
         if(!strftime(tmstr, sizeof(tmstr), "%a", tmp)){
             perror("strftime");
             exit(EXIT_FAILURE);
         }

		 if(tmp->tm_mon == 0){
			printf("JAN ");
		 }else if(tmp->tm_mon == 1){
			printf("FEB ");
		 }else if(tmp->tm_mon == 2){
			printf("MAR ");
		 }else if(tmp->tm_mon == 3){
			printf("APR ");
		 }else if(tmp->tm_mon == 4){
			printf("MAY ");
		 }else if(tmp->tm_mon == 5){
			printf("JUN ");
		 }else if(tmp->tm_mon == 6){
			printf("JUL ");
		 }else if(tmp->tm_mon == 7){
			printf("AUG ");
		 }else if(tmp->tm_mon == 8){
			printf("SEP ");
		 }else if(tmp->tm_mon == 9){
			printf("OCT ");
		 }else if(tmp->tm_mon == 10){
			printf("NOV ");
		 }else{
			printf("DEC ");
		 }
		 printf("%d ", tmp->tm_mday);
		 if(tmp->tm_min < 10){
			printf("0");
		 }
		 printf("%d ", tmp->tm_min);
	 	 

     }

     printf("%s", name);
	 //prints the file path to the file
	 if(S_ISLNK(statStruct->st_mode) && lFlag == 1){
		printf(" %s", realpath(name, fullName));
	 }
     if(FFlag == 1){
		 //prints the / after a dirictory
         if(S_ISDIR(statStruct->st_mode)){
             printf("/");
		 //pirnts the @ after a link
         }else if(S_ISLNK(statStruct->st_mode)){
             printf("@");
		 //prints a | after a pipe
         }else if(S_ISFIFO(statStruct->st_mode)){
             printf("|");
		 // prints a * if the file is exacutable
		 }else if((S_IXUSR & statStruct->st_mode) || (S_IXGRP & statStruct->st_mode) || (S_IXOTH & statStruct->st_mode)){
             printf("*");
         }
     }
	 //trying to fix spacing issues
	 if(lFlag == 1){
		printf("\n");
	 }else{
     printf("  ");//trying to fix some spacing issues
	 }
}

