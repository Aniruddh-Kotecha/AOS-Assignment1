#include <iostream>
#include <string.h>
#include <algorithm>
#include <ctype.h>
#include <cmath>
#include <fcntl.h> //open
#include <unistd.h> //read
#include <sys/stat.h> //mkdir
#include <errno.h> //errno perro

using namespace std;
#define BUFFER_SIZE int(pow(2,20))

void printMessage(string message){
    write(1, &message[0], message.length());
}

void getReverse(char* data, int data_size){
    for(int i = 0; i < data_size / 2; i++)
        swap(data[i], data[data_size-1-i]);
}

bool checkReverse(int old_fd, int new_fd, long long old_size, long long new_size){

    if(old_size != new_size)
        return false;
    
    long long offset = new_size;
    int  no_of_bytes;

    long long curr_position;

    char new_data[BUFFER_SIZE];
    char old_data[BUFFER_SIZE];
    int new_chunk_size, old_chunk_size;

    do{
        no_of_bytes = offset - BUFFER_SIZE > 0 ? BUFFER_SIZE : offset;
        curr_position = offset - BUFFER_SIZE > 0 ? offset - BUFFER_SIZE : 0;

        if(lseek(new_fd, curr_position, SEEK_SET) == -1) {
                perror("lseek");
                close(new_fd);
                close(old_fd);
                return -1;
        }

        new_chunk_size = read(new_fd, new_data, no_of_bytes);
        if(new_chunk_size == 0)
            break;
        
        old_chunk_size = read(old_fd, old_data, no_of_bytes);
        if(old_chunk_size == 0)
            break;
        
        for(int i = 0; i < no_of_bytes; i++){
            if(new_data[i] != old_data[no_of_bytes - 1 - i])
                return false;
        }

        offset -= no_of_bytes;

    }while(offset >= 0);

    return true;
}

void printPermissions(struct stat st, string pathtype){

    if(st.st_mode & S_IRUSR)
        printMessage("User has read permissions on " + pathtype + ": Yes\n");
    else
        printMessage("User has read permissions on " + pathtype + ": No\n");

    if(st.st_mode & S_IWUSR)
        printMessage("User has write permissions on " + pathtype + ": Yes\n");
    else
        printMessage("User has write permissions on " + pathtype + ": No\n");

    if(st.st_mode & S_IXUSR)
        printMessage("User has execute permissions on " + pathtype + ": Yes\n");
    else
        printMessage("User has execute permissions on " + pathtype + ": No\n");

    if(st.st_mode & S_IRGRP)
        printMessage("Group has read permissions on " + pathtype + ": Yes\n");
    else
        printMessage("Group has read permissions on " + pathtype + ": No\n");

    if(st.st_mode & S_IWGRP)
        printMessage("Group has write permissions on " + pathtype + ": Yes\n");
    else
        printMessage("Group has write permissions on " + pathtype + ": No\n");

    if(st.st_mode & S_IXGRP)
        printMessage("Group has execute permissions on " + pathtype + ": Yes\n");
    else
        printMessage("Group has execute permissions on " + pathtype + ": No\n");

    if(st.st_mode & S_IROTH)
        printMessage("Other has read permissions on " + pathtype + ": Yes\n");
    else
        printMessage("Other has read permissions on " + pathtype + ": No\n");

    if(st.st_mode & S_IWOTH)
        printMessage("Other has write permissions on " + pathtype + ": Yes\n");
    else
        printMessage("Other has write permissions on " + pathtype + ": No\n");

    if(st.st_mode & S_IXOTH)
        printMessage("Other has execute permissions on " + pathtype + ": Yes\n");
    else
        printMessage("Other has execute permissions on " + pathtype + ": No\n");    

}

int main(int argc, char* argv[]){

    // if(argc != 4){
    //     printMessage("Error: Incorrect input format - please enter new file, old file and directory\n");
    //     return 1;
    // }

    char* file_out = argv[1];
    char* file_in = argv[2];
    char* dir = argv[3]; 
    struct stat dirStatus;
    struct stat newStatus;
    struct stat oldStatus;

    long long new_size;
    long long old_size;

    int new_fd = open(file_out, O_RDONLY);
    if(new_fd == -1){
        perror("Error: Could not open new file");
        return 1;
    }

    int old_fd = open(file_in, O_RDONLY);
    if(old_fd == -1){
        perror("Error: Could not open new file");
        close(new_fd);
        return 1;
    }

    if (fstat(new_fd, &newStatus) < 0) {
        perror("Error: Incorrect new file");
        close(new_fd);
        close(old_fd);
        return 1;
    }

    if(!S_ISREG(newStatus.st_mode)){
        printMessage("Path for new file is not a regular file\n");
        close(new_fd);
        close(old_fd);
        return 1;
    }
    
    new_size = newStatus.st_size;

    if (fstat(old_fd, &oldStatus) < 0) {
        close(new_fd);
        close(old_fd);
        perror("Error: Incorrect old file");
        return 1;
    }

    if(!S_ISREG(oldStatus.st_mode)){
        printMessage("Path for old file is not a regular file\n");
        close(new_fd);
        close(old_fd);
        return 1;
    }

    if(stat(dir, &dirStatus) == 0){
        if(S_ISDIR(dirStatus.st_mode))
            printMessage("Directory is created: Yes\n");
        else{
            printMessage("Path is not a directory\n");
            return 1;
        }
            
    }  
    else{
        printMessage("Directory is created: No\n");
    }

    old_size = oldStatus.st_size;

    bool is_Reverse = checkReverse(old_fd, new_fd, old_size, new_size);

    if(is_Reverse)
        printMessage("Whether file contents are reversed in newfile: Yes\n");
    else
        printMessage("Whether file contents are reversed in newfile: No\n");

    if(old_size == new_size)
        printMessage("Both Files Sizes are Same : Yes\n\n");
    else
        printMessage("Both Files Sizes are Same : No\n\n");

    printPermissions(dirStatus, "directory");
    printMessage("\n");
    printPermissions(newStatus, "newfile");
    printMessage("\n");
    printPermissions(oldStatus, "oldfile");

    close(new_fd);
    close(old_fd);

    return 0;
}