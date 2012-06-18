#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>


#define FLAGS_NUMBER 5
#define MAX_FILENAME_LENGTH 100
#define MAX_FOUND_LENGTH 100
#define MAX_LINE 1024
#define SL_LIST_SIZE 1


char* pathname;
char* wanted;
char* allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.*?";


// I really should just declare them to be static... may do it later
int* entry_points;
int size;
int cur_ind;


int inode_present(int); 
void init();
	    
	    
	    
int parse_command(int,char**,int*);
void traverse(char*,char*, int*);
int match_file(char*, char);
int regex_match(FILE*,char*,FILE*); 
int clear_string(char*); 
int verify_string(char*);
int compare(char*, char*);
int look_up(char*,int); 



int main(int argc, char** argv) 
{ 
    int* flags = malloc(sizeof(int)*FLAGS_NUMBER);
    init();
    parse_command(argc, argv,flags); 
    if (!verify_string(wanted))
    {
	printf("Your regular expressions is of wrong format \n");
	return 0;
    }
    traverse(pathname, wanted, flags);
    return 0;
}



void init() 
{ 
    cur_ind = 0;
    pathname = NULL;
    wanted = NULL;
    entry_points = NULL;
    
}


verify_string(char* str) 
{
    if (str[0] == '\0')
	return 1;
    int len = strlen(allowed); 
    int i; 
    for (i = 0; i < len; i++)
    {
	if (str[0] == allowed[i]) 
	    return verify_string(str+1);	
    }
    return 0;
} 



int parse_command(int argc, char** argv, int* flags) 
{ 
    int fflag = 0;
    int cflag = 0; 
    int hflag = 0; 
    int Sflag = 0;  
    int lflag = 0; 
    int res;
    while ((res = getopt (argc, argv, "p:f:ls:")) != -1)
    {
     
	switch (res) 
	{ 
	case 'p': 
	    pathname = optarg;
	    break;
	case 'f':
	    fflag = 1;
	    char* temp = optarg;
	    if (temp[0] == 'c')
		cflag = 1; 
	    else if (temp[0] == 'h') 
		hflag = 1; 
	    else if (temp[0] == 'S') 
		Sflag = 1;
	    else 
	    {
		fprintf (stderr, "Unrecognized argument of f flag: %s \n", temp);
		return -1;
	    }
	    break;
	case 'l': 
	    lflag = 1; 
	    break; 
	case 's': 
	    wanted = optarg;  
	    break; 
	case '?': 
	    if (optopt == 'p' || optopt == 's')
		fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	    else if (isprint (optopt))
		fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	    else
		fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
	    return 1;
	default: 
	    printf("Aborting\n");
	    abort(); 
	} 
    }
    int index;
    *(flags)++ = fflag; 
    *(flags)++ = cflag; 
    *(flags)++ = hflag; 
    *(flags)++ = Sflag;
    *(flags)++ = lflag;
    
    printf ("pathname = %s, fflag = %d, cflag = %d, hflag = %d, Sflag = %d,  lflag = %d, wanted = %s \n", pathname, 
	    fflag, cflag, hflag, Sflag, lflag, wanted);
    for (index = optind; index < argc; index++)
	printf ("Non-option argument %s\n", argv[index]);
    
}     




void traverse(char *dir, char* wanted, int* flags)
{
    int k; 
    
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"Cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    
    while((entry = readdir(dp)) != NULL) 
    {
	//if (flags[4] == 0)
	
	lstat(entry->d_name,&statbuf); // use lstat to ignore symbolic links. To test I have a symbolic link sl in cs511/hw4/sl
	// 	else 
	//     stat(entry->d_name,&statbuf);
	
       	if (flags[4] == 1) 
       	{
	    if (entry_points == NULL) 
	    {
		
		int cur_inode = statbuf.st_ino;
		printf("initializing entry_points with %d \n", cur_inode);
		
		entry_points = malloc (sizeof(int)*SL_LIST_SIZE);
		entry_points[0] = cur_inode;
		size = SL_LIST_SIZE;
		cur_ind = 0;
	    }   
	    
	}
	
    	
        if(S_ISDIR(statbuf.st_mode)) 
        {
	    if (flags[4] == 1) 
	    { 
		int size = sizeof(entry_points)/sizeof(int);
		int i; 
		int cur_inode = statbuf.st_ino;
		char* name = entry->d_name;
		if (name[0] == '.' || (name[0] == '.' && name[1] == '.'))
		    continue;    
		if (inode_present(cur_inode)) 
		{
		    printf("skipping %s \n", entry->d_name);
		    continue;
		}
	    }
	    char* name = entry->d_name;
            if (name[0] == '.' || (name[0] == '.' && name[1] == '.'))
            	continue;                  
            traverse(entry->d_name,wanted,flags);
        }
	
        else if (S_ISLNK(statbuf.st_mode) && flags[4]==1) 
	{ 
	    char original[PATH_MAX + 1];
	    char* res = realpath(dir,original);
	    int t = strlen(original); 
	    int p; 
	    for(p = t; p>=0;p--)
	    { 
		if (original[p] == '/')
		{ 
		    original[p] = '\0';
		    break;
		}
	    }
	    int cur_inode = statbuf.st_ino;
	
	    if (inode_present(cur_inode)==0 && look_up(entry->d_name,cur_inode))
	    {
		
		chdir(original);
		char link[PATH_MAX+1];
		char* temp = realpath(entry->d_name,link);
		stat(entry->d_name,&statbuf);
		
		traverse(entry->d_name,wanted,flags);
		chdir(original);
	    }
	    else 
	    { 
		continue;
	    }
	    
        }
	
	else 
        {  
	    FILE *fp = fopen(entry->d_name,"r");
	    if (fp == NULL) 
	    {
		printf("Cannot open the file %s \n", entry->d_name);
		continue;
	    }
	    FILE *cp = fopen(entry->d_name,"r");
		
            int count = 0;
            if ( (flags[0] && ( (flags[1] && match_file(entry->d_name,'c')) || (flags[2] && match_file(entry->d_name, 'h')) 
				|| (flags[3] && match_file(entry->d_name, 'S')))) || flags[0] == 0 )
            {
            	char* found = malloc(sizeof(char)*MAX_FOUND_LENGTH);
            	if (regex_match(fp,found,cp))
		    count++;
            	if (count > 0) 
            	{
		    char full_path[PATH_MAX + 1];
		    char* res = realpath(entry->d_name,full_path);
		    printf("%s is found in file %s \n", found, full_path);
		    printf("\n");
            	}
            	free(found);
	    }
	    fclose(fp); 
	    fclose(cp);
            
        }
    }
    chdir("..");
    closedir(dp);
}



int match_file(char* filename, char c) 
{ 
    int len = strlen(filename); 
    int i; 
    for (i = 0 ; (i < len) && (filename[i] != '.'); i++) 
	;
    if (filename[i+1] == c && filename[i+2] == '\0')
	return 1; 
    else return 0;
}




int regex_match(FILE* fp,char* found_string,FILE* cp) 
{ 
    
    char buf[MAX_LINE];  
    clear_string(buf);
    fgets(buf,MAX_LINE,cp);
    char* alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int alphabet_length = strlen(alphabet);
    // dont forget about the case when file is empty
    int len = strlen(wanted);
    int wanted_index = 0; 
    int found_index = 0;
    int c;
    do 
    {
    	c = fgetc (fp);
    	if (c == '\n') 
    	{
	    clear_string(buf);
	    //printf("new line %s \n",buf);
	    
	    fgets(buf,MAX_LINE,cp);
    	}
    	if (wanted[wanted_index] == '.')
    	{
	    int i; 
	    for (i = 0; i < alphabet_length; i++) 
	    {
		if (c == alphabet[i]) 
		{ 
		    found_string[found_index] = c;
		    if (++wanted_index == len)
		    {
			found_index++;
			if (buf[strlen(buf)-1] != '\n')
			    printf("line in the file:  %s \n", buf);
			else 
			    printf("line in the file: %s", buf);
			return 1;
		    }
		    else 
		    {
			found_index++;
			break;
		    }
		}
	    }
	    if (i == alphabet_length) 
	    {
		clear_string(found_string);
		wanted_index = 0;
		found_index = 0;
	    }
    	}
    	else if ( (wanted_index + 1 < len) && ( wanted[wanted_index+1] == '?') )  // This shiz better be right
    	{ 
	    if (c == wanted[wanted_index+2] && (c != wanted[wanted_index])) // zero occurrences of previous character and make sure 2 characters arent same
	    {
		found_string[found_index++] = c;
		if(wanted_index+2 == len-1)
		{
		    if (buf[strlen(buf)-1] != '\n')
			printf("line in the file:  %s \n", buf);
		    else 
			printf("line in the file: %s", buf);    		
		    return 1;
		}
		wanted_index+=3;
	    }
	    else if (c == wanted[wanted_index+2] && (c == wanted[wanted_index])) 
	    { 	
		found_string[found_index++] = c;
		int next = fgetc (fp);
		if (next == '\n') 
		{
		    clear_string(buf);
		    fgets(buf,MAX_LINE,cp);
		}
		if (next == EOF) 
		{
		    if (buf[strlen(buf)-1] != '\n')
			printf("line in the file:  %s \n", buf);
		    else 
			printf("line in the file: %s", buf);    				
		    return 1;
		}	
		
		if (next == c) 
		{ 
		    found_string[found_index++] = next;
		    if(wanted_index+2 == len-1)
		    {
			if (buf[strlen(buf)-1] != '\n')
			    printf("line in the file:  %s \n", buf);
			else 
			    printf("line in the file: %s", buf);
			return 1;
		    }
		    wanted_index+=3;
		}
		else
		{ 
		    if (next == wanted[wanted_index+3])
		    { 
			found_string[found_index++] = next;
			if (wanted_index + 3 == len-1) 
			{
			    if (buf[strlen(buf)-1] != '\n')
				printf("line in the file:  %s \n", buf);
			    else 
				printf("line in the file: %s", buf);
			    return 1;
			}
			if (wanted_index+4 < len-1 && (wanted[wanted_index+4] == '?' || wanted[wanted_index+4] == '*' )) //to deal with Hel?lo?o 
			    wanted_index +=3;
			else
			    wanted_index += 4;
		    } 
		    else 
		    { 
			clear_string(found_string);
			wanted_index = 0;
			found_index = 0;
		    }
		}
		
	    } 
	    else 
	    { 
		if (c == wanted[wanted_index]) 
		{ 
		    found_string[found_index++] = c;
		    wanted_index+=2;
		}
		else
		{
		    clear_string(found_string);
		    wanted_index = 0;
		    found_index = 0;
		}
	    }
    	}
    	else if ( (wanted_index + 1 < len) && ( wanted[wanted_index+1] == '*') ) 
    	{ 
	    if (c == wanted[wanted_index+2] && (c != wanted[wanted_index])) // zero occurrences of previous character and make sure 2 characters arent same
	    {
		found_string[found_index++] = c;
		if(wanted_index+2 == len-1)
		{
		    if (buf[strlen(buf)-1] != '\n')
			printf("line in the file:  %s \n", buf);
		    else 
			printf("line in the file: %s", buf);    	
		    return 1;
		}
		wanted_index+=3;
	    } 
	    else if (c == wanted[wanted_index+2] && (c == wanted[wanted_index])) 
	    { 	
		found_string[found_index++] = c;
		
		int next;
		do 
		{ 
		    next = fgetc(fp); 
		    if (next == '\n') 
		    {
			clear_string(buf);
			fgets(buf,MAX_LINE,cp);
		    }
		    if (next == EOF)
		    {
			//	printf("end of file\n"); 
			if (buf[strlen(buf)-1] != '\n')
			    printf("line in the file:  %s \n", buf);
			else 
			    printf("line in the file: %s", buf);    		
			return 1;
		    } 
		    
		    if (next != wanted[wanted_index])
			break;
		    
		    found_string[found_index++] = c;
		    //printf("found_string = %s \n", found_string);
		} while (next == wanted[wanted_index]);
		if (next == wanted[wanted_index+3])
		{ 
		    found_string[found_index++] = next;
		    if (wanted_index + 3 == len-1) 
		    {
			if (buf[strlen(buf)-1] != '\n')
			    printf("line in the file:  %s \n", buf);
			else 
			    printf("line in the file: %s", buf);					
			return 1;
		    }
		    if (wanted_index+4 < len-1 && (wanted[wanted_index+4] == '?' || wanted[wanted_index+4] == '*' )) //to deal with Hel?lo?o 
			wanted_index +=3;
		    else
			wanted_index += 4;
		} 
		else 
		{ 
		    clear_string(found_string);
		    wanted_index = 0;
		    found_index = 0;
		}
	    }
	    else 
	    { 
		
		//found_string[found_index++] = c;
		if (c == wanted[wanted_index])
		{
		    //printf("found %s \n", found_string);
		    
		    found_string[found_index++] = c;
		    
		    int next;
		    do 
		    { 
			//	printf("found %s \n", found_string);
			next = fgetc(fp); 
			if (next == '\n') 
			{
			    clear_string(buf);
			    fgets(buf,MAX_LINE,cp);
			}
			if (next == EOF)
			{
			    if (buf[strlen(buf)-1] != '\n')
				printf("line in the file:  %s \n", buf);
			    else 
				printf("line in the file: %s", buf);    	
			    return 1; 
			}
			if (next != wanted[wanted_index])
			    break;
    			
			found_string[found_index++] = c;
		    } while (next == wanted[wanted_index]);
		    if (next == wanted[wanted_index+2]) 
		    { 
			found_string[found_index++] = next;
			if (wanted_index + 3 == len-1) 
			{
			    if (buf[strlen(buf)-1] != '\n')
				printf("line in the file:  %s \n", buf);
			    else 
				printf("line in the file: %s", buf);					
			    return 1;
			}
			if (wanted_index+4 < len-1 && (wanted[wanted_index+4] == '?' || wanted[wanted_index+4] == '*' )) //to deal with Hel?lo?o 
			    wanted_index +=3;
			else
			    wanted_index += 4;
		    }
		    else 
		    { 
			clear_string(found_string);
			wanted_index = 0;
			found_index = 0;
		    }
		}
		else 
		{ 
		    clear_string(found_string);
		    wanted_index = 0;
		    found_index = 0;
		}
	    }
	    
    	} 
    	
    	else if ((wanted[wanted_index] != '.') && (wanted[wanted_index] != '*') && (wanted[wanted_index] != '?')) 
    	{
	    if(c == wanted[wanted_index])
	    {
		found_string[found_index++] = c;
		
		if (++wanted_index == len)
		{
		    found_index++;
		    if (buf[strlen(buf)-1] != '\n')
			printf("line in the file:  %s \n", buf);
		    else 
			printf("line in the file: %s", buf);    
		    return 1;    		 
		}
	    }
	    else 
	    {
		clear_string(found_string);
		found_index = 0;
		wanted_index = 0;
	    }
    	}
    } while (c != EOF);
    return 0;
}




int clear_string(char* str) 
{ 
    int len = strlen(str); 
    int i; 
    for (i = 0; i < len; i++)
	str[i] = '\0';
    return 1;
}


int compare(char* str1, char* str2) 
{ 
    int len1 = strlen(str1); 
    int len2 = strlen(str2);
    if (len1 != len2) 
	return 0;
    int i; 
    for (i = 0; i < len1; i++) 
    { 
	if (str1[i] != str2[i])
	    return 0;
    }
    return 1;
}



int look_up(char* original,int cur_inode)
{
    struct stat statbuf;
    
    chdir(original);
    int iter = 0 ;
    
    char buffer[20];
    int i; 
    for (i = 0; i < 20; i++)
	buffer[i] = '\0';
    char* cur_dir =  getwd(buffer);
    stat(buffer, &statbuf);
    int ino = statbuf.st_ino;
    //&& iter!=0
    if(inode_present(ino)&& iter!=0) 
    {
	return 0;
    }
    iter++;
    if ( (buffer[0] == '/' && buffer[1] == '\0') )
    {	
	if (cur_ind < size)
	{
	    //    printf("size %d \n", size);
	     
	     entry_points =  (int*) realloc(entry_points, size*(sizeof(int)) + sizeof(int)*SL_LIST_SIZE) ;
	     
	     size +=SL_LIST_SIZE;
	     entry_points[++cur_ind] = cur_inode; 
	}
	else 
	{
	    entry_points[++cur_ind] = cur_inode;
	    size++;
	}
	return 1;
    }	
    return look_up("..",cur_inode);
}




int inode_present(int cur_inode) 
{ 
    int size = sizeof(entry_points)/ sizeof(int); 
    int i;
    for (i=0; i < size;i++) 
    {  
	if (entry_points[i] == cur_inode)
        {
	    return 1;
	}
    }
    return 0;
}



