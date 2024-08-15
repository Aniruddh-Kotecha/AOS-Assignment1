#include <iostream>
#include <string.h>
#include <algorithm>
#include <ctype.h>
#include <cmath>
#include <fcntl.h> //open
#include <unistd.h> //read
#include <sys/stat.h> //mkdir
#include <errno.h> //errno perror

using namespace std;

#define BUFFER_SIZE int(pow(2,20))
long long written_size;
long long file_size;

void printMessage(string message){
    write(1, &message[0], message.length());
}

char* getFileName(char* file_path){
    int len = strlen(file_path);
    int index = -1;
    for(int i = len - 1; i >= 0; i--){
        if(file_path[i] == '\\' || file_path[i] == '/'){
            index = i;
            break;
        }
    }

    if(index != -1){
        int filename_len = len - index - 1;

        char* file_name = new char[filename_len + 1];
    
        int j = 0;
        for(int i = index + 1; i < len; i++)
            file_name[j++] = file_path[i];

        file_name[j] = '\0';

        return file_name;
    }
        
    return file_path;
}

long getFileSize(int fd) {

    if (lseek(fd, 0, SEEK_END) == -1) {
        perror("Error when seeking to end of file");
        close(fd);
        return -1;
    }

    long size = lseek(fd, 0, SEEK_CUR);
    if (size == -1) {
        perror("Error when getting size of file!");
        close(fd);
        return -1;
    }

    return size;
}

void getReverse(char* data, int data_size){
    for(int i = 0; i < data_size / 2; i++)
        swap(data[i], data[data_size-1-i]);
}

int writeFile(int in_fd, int out_fd, long long start, long long end, bool isReverse){
    
    long long offset = end;

    char data[BUFFER_SIZE];
    int chunk_size, no_of_bytes, curr_progress;
    long long curr_position;

    if(!isReverse && lseek(in_fd, start, SEEK_SET) == -1) {
        perror("lseek");
        close(in_fd);
        return -1;
    }

    do{
        curr_progress = written_size*100/(file_size-1);
        string progress = "Progress: " + to_string(curr_progress) + "%\r";
        char* ptrProgress = &progress[0];
        write(1, ptrProgress, progress.length());

        no_of_bytes = offset - start - BUFFER_SIZE > 0 ? BUFFER_SIZE : offset - start;

        if(isReverse){
            curr_position = offset - BUFFER_SIZE > start ? offset - BUFFER_SIZE : start;
        
            if(lseek(in_fd, curr_position, SEEK_SET) == -1) {
                perror("lseek");
                close(in_fd);
                return -1;
            }
        }

        chunk_size = read(in_fd, data, no_of_bytes);
        if(chunk_size == 0)
            break;
        if(isReverse)
            getReverse(data, chunk_size);
        write(out_fd, data, chunk_size);
        
        offset -= no_of_bytes;
        written_size += no_of_bytes; 

    }while(offset >= start);

    return 0;
}

int main(int argc, char* argv[]){

    //create a dir if not already present.
    string dir = "Assignment1";
    char* dir_name = &dir[0];
    if(mkdir(dir_name, 0000700) == -1){
        switch(errno){
            case 17:break;
            default: perror("Error when creating directory");
        }
    }

    if(argc != 3 && argc != 5){
        printMessage("Error: Incorrect input format - \nPlease enter 3 arguments for flag 0.\nOr enter 5 arguments for flag 1\n");
        return 1;
    }

    //get file name and create output file
    char* file_path = argv[1];
    char out_file_name[255] = "";

    char* file_name = getFileName(file_path);

    //get flag value
    int flag;

    if(!isdigit(*argv[2])){
        printMessage("Error: Incorrect format for flag\n");
        return 1; 
    }

    flag = atoi(argv[2]);
    string flag_check = argv[2];
    if(to_string(flag).length() != flag_check.length()){
        printMessage("Error: Incorrect value for input flag!\n");
        return 1;
    }
    
    string file_prefix;

    long long start_index;
    long long end_index;

    if(flag == 0){
        file_prefix = "Assignment1/0_";
        if(argc != 3){
            printMessage("Error: Incorrect input format for flag 0\n");
            return 1;
        }
    }
    else if(flag == 1){
        file_prefix = "Assignment1/1_";
        if(argc != 5){
            printMessage("Error: Incorrect input format for flag 1\n");
            return 1;
        }
        
        if(!isdigit(*argv[3])){
            printMessage("Error: Incorrect value for start index!\n");
            return 1; 
        }

        string start_check = argv[3];
        if(to_string(flag).length() != start_check.length()){
            printMessage("Error: Incorrect value for start index!\n");
            return 1;
        }

        if(!isdigit(*argv[4])){
            printMessage("Error: Incorrect value for end index!\n");
            return 1; 
        }

        string end_check = argv[4];
        if(to_string(flag).length() != end_check.length()){
            printMessage("Error: Incorrect value for end index!\n");
            return 1;
        }


        start_index = atoi(argv[3]);
        end_index = atoi(argv[4]);
    }
    else{
        printMessage("Error: Incorrect value for input flag!\n");
        return 1;
    }

    //create file path for output
    char* file_prefix_name = &file_prefix[0];
    strcat(out_file_name,file_prefix_name);
    strcat(out_file_name,file_name);
    
    //open input file
    int in_file_decriptor = open(file_path, O_RDONLY);
    if(in_file_decriptor == -1){
        perror("Error when opening input file");
        return 1;
    }

    int out_file_creator = creat(out_file_name, 0000600);
    if(out_file_creator == -1){
        perror("Error when creating output file");
        return 1;
    }

    int out_file_decriptor = open(out_file_name, O_RDWR);
    if(out_file_decriptor == -1){
        perror("Error when opening output file");
        return 1;
    }

    //get file size of input file
    file_size = getFileSize(in_file_decriptor);
    if(file_size == -1 or file_size == 0)
        return 1;

    if(flag == 1 && (start_index < 0 || start_index > file_size || end_index < 0 || end_index > file_size)){
        printMessage("Error: Please enter correct range of indices!\n");
        return 1;
    }

    if(flag == 1 && (start_index > end_index)){
        printMessage("Error: Start index cannot be greater than end index!\n");
        return 1;
    }

    //long long offset = file_size;

    char data[BUFFER_SIZE];
    int chunk_size;

    written_size = 0;

    int status;
    if(flag == 0)
        status = writeFile(in_file_decriptor, out_file_decriptor, 0, file_size, true);
    else if(flag == 1){
        status = writeFile(in_file_decriptor, out_file_decriptor, 0, start_index, true);
        status = writeFile(in_file_decriptor, out_file_decriptor, start_index, end_index+1, false);
        status = writeFile(in_file_decriptor, out_file_decriptor, end_index+1, file_size, true);
    }

    if (lseek(out_file_decriptor, 0, SEEK_END) == -1) {
        perror("Error when getting size of file!");
        close(out_file_decriptor);
        return 1;
    }
    //write(out_file_decriptor, "\n", 1);
    //written_size = written_size+1;

    string progress = "Progress: " + to_string(written_size*100/(file_size)) + "%\n";
    char* ptrProgress = &progress[0];
    write(1, ptrProgress, progress.length());

    close(in_file_decriptor);
    close(out_file_decriptor);

    return 0;
}