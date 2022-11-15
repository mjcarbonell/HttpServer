#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "bind.h"
#include "string.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/stat.h>
/* filename server_ipaddress portno 

argv[0] filename  
argv[1] server_ipaddress 
argv[2] 
*/
// LSEEK() finds bytes of file 
//STRTOK() 
unsigned contentLength = 0; 
char contentType[500]; 
char acceptField[500]; 
char userAgent[500];
char host[500];
char expect[500]; 
char URI[500];
char CreatedStatus[49] = "HTTP/1.1 201 Created\r\nContent-Length:7\r\n\r\nCreated";
char NotImplemented[72] = "HTTP/1.1 501 Not Implemented\r\nContent-Length:15\r\n\r\nNot Implemented";
char BadRequestStatus[58] = "HTTP/1.1 400 Bad Request\r\nContent-Length:11\r\n\r\nBad Request";
char NotFoundStatus[53] = "HTTP/1.1 404 Not Found\r\nContent-Length:9\r\n\r\nNot Found";
char ForbiddenStatus[53] = "HTTP/1.1 403 Forbidden\r\nContent-Length:9\r\n\r\nForbidden";
void error(const char *msg){
	perror(msg);	
	exit(1);
}
int readFile(int newSockfd, char* URI, int contentLength) {

    int count = 0;
    int bufferSize = 4096;
    char buffer[bufferSize];
    int fd = open(URI, O_WRONLY | O_TRUNC, 0777);
    if (fd < 0){
    	close(fd);
    	fd = open(URI, O_CREAT | O_RDWR, 0777);
    }
    ssize_t fileSize = read(newSockfd, buffer, bufferSize);
    count += fileSize; 
    //printf("FILESIZE: %ld\n", fileSize); 
    if (fileSize < 0) {
        char w5[50] = "httpserver: missing: No such file or directory\n";
        write(2, w5, 50);
        return -1;
    }
    
    write(fd, buffer, fileSize);
    if (fileSize < bufferSize) {
        return 0;
    }
    //memset(buffer, 0, bufferSize); // reset buffer
    while (count != contentLength) { // fileSize == bufferSize
    	//printf("fileSize: %ld\n", fileSize);
        fileSize = read(newSockfd, buffer, bufferSize);
        count += fileSize; 
        //printf("count : %d and contentLength: %d\n", count, contentLength);
        write(fd, buffer, fileSize);
        //memset(buffer, 0, bufferSize); // reset buffer
    }
    memset(buffer, 0, 4096); 
    printf("end of readFile\n");
    close(fd);
    return 0;
}
int readBinary(int newSockfd, int URI, int contentLength) {
    int count = 0;
    int bufferSize = 4096;
    char buffer[bufferSize];
    int fd = URI;
    ssize_t fileSize = read(newSockfd, buffer, bufferSize);
    count += fileSize; 
    //printf("FILESIZE: %ld\n", fileSize); 
    if (fileSize < 0) {
        char w5[50] = "httpserver: missing: No such file or directory\n";
        write(2, w5, 50);
        return -1;
    }
    
    write(fd, buffer, fileSize);
    if (fileSize < bufferSize) {
        return 0;
    }
    //memset(buffer, 0, bufferSize); // reset buffer
    while (count != contentLength) { // fileSize == bufferSize
    	//printf("fileSize: %ld\n", fileSize);
        fileSize = read(newSockfd, buffer, bufferSize);
        count += fileSize; 
        //printf("count : %d and contentLength: %d\n", count, contentLength);
        write(fd, buffer, fileSize);
        //memset(buffer, 0, bufferSize); // reset buffer
    }
    memset(buffer, 0, 4096); 
    printf("end of readFile\n");
    close(fd);
    return 0;
}
unsigned concatenate(unsigned a, unsigned b){
	unsigned power = 10;
	while(b >= power){
		power *= 10;
	}
	return (a * power) + b;
}

void grabFields(char* headerFields, char* header){
	
	char *lengthString = strstr(headerFields, header);
	if (lengthString != NULL){
		int index = lengthString - headerFields; 
		int offset = 0; 
		while(headerFields[index+offset] != ':'){
			offset += 1;
			
		}
		offset += 2;
		char length[200];
		char temp[2]; 
		memset(length, 0, 200);
		memset(temp, 0, 2);
		while(headerFields[index+offset] != '\n'){
			//printf("character at index: %c\n", headerFields[index+offset]);
			temp[0] = headerFields[index+offset]; 
			temp[1] = 0; 
			strcat(length, temp);
			offset += 1;
		}
		if(strcmp(header, "Content-Type") == 0){
			strcpy(contentType, length);
			memset(length, 0, 200);
			memset(temp, 0, 2);
			return; 
		}
		if(strcmp(header, "Accept") == 0){
			strcpy(acceptField, length);
			memset(length, 0, 200);
			memset(temp, 0, 2);
			return; 
		}
		if(strcmp(header, "Content-Type") == 0){
			strcpy(contentType, length);
			memset(length, 0, 200);
			memset(temp, 0, 2);
			return; 
		}
		if(strcmp(header, "User-Agent") == 0){
			strcpy(userAgent, length);
			memset(length, 0, 200);
			memset(temp, 0, 2);
			return; 
		}
		if(strcmp(header, "Host") == 0){
			strcpy(host, length);
			memset(length, 0, 200);
			memset(temp, 0, 2);
			return; 
		}
		if(strcmp(header, "Expect") == 0){
			strcpy(expect, length);
			memset(length, 0, 200);
			memset(temp, 0, 2);
			return; 
		}
		
	}
	
}
bool getFunction(int newSockfd, char* headerFields){
	//char* OKStatus = NULL;
	char OKStatus[200] = "HTTP/1.1 200 OK\r\nContent-Length:";
	//printf("%s\n", headerFields);
	
	char *lengthString = strstr(headerFields, "Content-Length"); 
	if (lengthString != NULL){
		int index = lengthString - headerFields; 
		int offset = 16; 
		char length[200];
		char temp[2]; 
		memset(length, 0, 200);
		memset(temp, 0, 2);
		while(headerFields[index+offset] != '\n'){
			//printf("character at index: %c\n", headerFields[index+offset]);
			temp[0] = headerFields[index+offset]; 
			temp[1] = 0; 
			strcat(length, temp);
			offset += 1;
		}
		unsigned a = 0;
		unsigned b = 0;
		for(size_t i=0; i<strlen(length) - 1; i++){
			//printf("char %d\n", length[i]);
			if (length[i] > 57 || length[i] < 48){
				continue;
			}
			b = length[i] - '0';
			a = concatenate(a, b);
			//printf("A is NOW: %d\n", a);
		}
		contentLength = a;
		memset(length, 0, 200);
		memset(temp, 0, 2);
	}
	grabFields(headerFields, "Content-Type");
	printf("contentType: %s\n", contentType);
	grabFields(headerFields, "Host");
	printf("host: %s\n", host);
	grabFields(headerFields, "User-Agent");
	printf("userAgent: %s\n", userAgent);
	grabFields(headerFields, "Accept");
	printf("accept: %s\n", acceptField);
	grabFields(headerFields, "Expect");
	printf("expect: %s\n", expect);
	//printf("contentLength : %d\n", contentLength);
	// TIME TO PRINT THE MESSAGE BODY TO CLIENT 
	lengthString = strstr(headerFields, "GET");
	if(lengthString != NULL){
		int index = lengthString - headerFields; 
		int offset = 5; 
		char length[200]; 
		char temp[2]; 
		memset(length, 0, 200);
		memset(temp, 0, 2);
		while(headerFields[index+offset] != ' '){
			//printf("character at index: %c\n", headerFields[index+offset]);
			temp[0] = headerFields[index+offset]; 
			temp[1] = 0; 
			strcat(length, temp);
			offset += 1;
		}
		//printf("length is: %s\n", length);
		strcpy(URI, length);
		memset(length, 0, 200);
		memset(temp, 0, 2);
		if (strlen(URI) > 19){
			send(newSockfd, BadRequestStatus, 58, 0);
			return false; 
		}
	}
	printf("URI: %s\n", URI);
	if (access(URI, F_OK) == 0){
		printf("exists\n");
		
	}
	else{
		write(newSockfd, NotFoundStatus, 53);
		return false; 
	}
	struct stat fileStat; 
	stat(URI, &fileStat);
	
	
	if( ((fileStat.st_mode & S_IRGRP) == 0) & ((fileStat.st_mode & S_IROTH) == 0) & ((fileStat.st_mode & S_IWGRP) == 0) & ((fileStat.st_mode & S_IWOTH) == 0)){
		send(newSockfd, ForbiddenStatus, 53, 0);
		return false;
	}
	if(S_ISREG(fileStat.st_mode) == 0){ // IF EQUAL TO ZERO THEN ITS A DIRECTORY
		send(newSockfd, ForbiddenStatus, 53, 0);
		return false; 
	}
	contentLength = fileStat.st_size; 
	char temp[200]; 
	sprintf(temp, " %d\r\n\r\n",contentLength);
	strcat(OKStatus, temp);
	send(newSockfd, OKStatus, strlen(OKStatus), 0);
	
	int fd = open(URI, O_RDONLY , 0777);
	printf("FD IS: %d\n", fd);
	if (fd < 0){
		close(fd);
		send(newSockfd, NotFoundStatus, 53, 0);
		return false; 	
	}
	
	readBinary(fd, newSockfd, contentLength);
	memset(OKStatus, 0, 200); 
	memset(temp, 0, 200);
	return true; 
}
bool putFunction(int newSockfd, char* headerFields){ // IF A PUT REQUEST WE WANT  TO MAKE A FILE AND RETURN A RESPONSE CODE...
	//printf("put function: %d\n", newSockfd);
	//printf("%s\n", headerFields);
	bool didExist = false; 
	char OKStatus[200] = "HTTP/1.1 200 OK\r\n";
	char *lengthString = strstr(headerFields, "Content-Length"); 
	if (lengthString != NULL){
		int index = lengthString - headerFields; 
		int offset = 16; 
		char length[200];
		char temp[2]; 
		memset(length, 0, 200); 
		memset(temp, 0, 2); 
		while(headerFields[index+offset] != '\n'){
			//printf("character at index: %c\n", headerFields[index+offset]);
			temp[0] = headerFields[index+offset]; 
			temp[1] = 0; 
			strcat(length, temp);
			offset += 1;
		}
		//printf("STRLEN: %ld\n", strlen(length)); 
		if (strlen(length) == 0){
			send(newSockfd, BadRequestStatus, 58, 0); 
			return false;
		}
		unsigned a = 0;
		unsigned b = 0;
		for(size_t i=0; i<strlen(length) - 1; i++){
			//printf("char %d\n", length[i]);
			if (length[i] > 57 || length[i] < 48){
				continue;
			}
			b = length[i] - '0';
			a = concatenate(a, b);
			//printf("A is NOW: %d\n", a);
		}
		contentLength = a;
		memset(length, 0, 200);
		memset(temp, 0, 2);
	}
	else{
		send(newSockfd, BadRequestStatus, 58, 0);
		return false; 
	}
	lengthString = strstr(headerFields, "PUT");
	if(lengthString != NULL){
		int index = lengthString - headerFields; 
		int offset = 5; 
		char length[200]; 
		char temp[2]; 
		memset(length, 0, 200);
		memset(temp, 0, 2);
		while(headerFields[index+offset] != ' '){
			//printf("character at index: %c\n", headerFields[index+offset]);
			temp[0] = headerFields[index+offset]; 
			temp[1] = 0; 
			strcat(length, temp);
			offset += 1;
		}
		//printf("length is: %s\n", length);
		strcpy(URI, length);
		memset(length, 0, 200);
		memset(temp, 0, 2);
		if (strlen(URI) > 19){
			send(newSockfd, BadRequestStatus, 58, 0);
			return false; 
		}
	}
	grabFields(headerFields, "Content-Type");
	grabFields(headerFields, "Accept");
	grabFields(headerFields, "User-Agent");
	grabFields(headerFields, "Host");
	grabFields(headerFields, "Expect");
	grabFields(headerFields, "Content-Type");
	printf("contentType: %s\n", contentType);
	printf("accept: %s\n", acceptField);
	printf("contentLength: %d\n", contentLength);
	printf("contentType: %s\n", contentType);
	printf("userAgent: %s\n", userAgent);
	printf("host: %s\n", host);
	printf("expect: %s\n", expect);
	printf("URI: %s\n", URI);
	if (access(URI, F_OK) == 0){ // if the file existed then 200 ok 
		printf("exists\n");
		didExist = true; 
		
	}
	if (strlen(expect) == 0){ // no EXPECT FIELD 
		lengthString = strstr(headerFields, "\r\n\r\n");
		if(lengthString == NULL){
			read(newSockfd, headerFields, 2049);
		}
		printf("if statement\n");	
		if (lengthString != NULL){
			//printf("n is: %d\n", n);
			int index = lengthString - headerFields; 
			int offset = 4; 
			//printf("character at index: %c\n", headerFields[index + offset]);
			unsigned restOfFile = strlen(headerFields) - (index + offset);
			//printf("REST OF FILE %d\n", restOfFile);
			
			char *start = &headerFields[index + offset];
			char *end = &headerFields[index+offset+restOfFile];
			/*
			if ( (start - end) == 0){
				send(newSockfd, BadRequestStatus, 58, 0);
				return false; 
			}
			*/
			char *messageBody = (char *)calloc(1, end - start + 1);
			memcpy(messageBody, start, end - start); 
			int fd = open(URI, O_RDWR | O_TRUNC, 0777);
    			//printf("URI IS NOW : %s\n", URI);
			if (fd < 0){
				close(fd);
				fd = open(URI, O_CREAT | O_RDWR, 0777);
			}
			unsigned count = 0; 
			write(fd, messageBody, restOfFile);
			count += restOfFile; 
			char buffer[2048];
			if (count != contentLength){
				printf("count %d: \n", count);
				size_t fileSize = read(newSockfd, buffer, 2048);
				write(fd, buffer, fileSize);
				count += fileSize; 
				while(count != contentLength){
					fileSize = read(newSockfd, buffer, 2048);
					write(fd, buffer, fileSize);
					count += fileSize; 
				}
			}
			memset(buffer, 0, 2048);
			free(messageBody);
			close(fd);
			if (didExist == true){
				send(newSockfd, OKStatus, 200, 0);
				return true; 
			}
			send(newSockfd, CreatedStatus, 49, 0);
			return true; 
		}
	}
	else{
		printf("else statement w/ contentlengt: %d\n", contentLength);	
		readFile(newSockfd, URI, contentLength);
		if (didExist == true){
			send(newSockfd, OKStatus, 200, 0);
			return true; 
		}	
		send(newSockfd, CreatedStatus, 49, 0);
		return true; 
	}
	return false; 
}
bool headFunction(int newSockfd, char* headerFields){
	//char* OKStatus = NULL;
	char OKStatus[200] = "HTTP/1.1 200 OK\r\nContent-Length:";
	//printf("%s\n", headerFields);
	
	char *lengthString = strstr(headerFields, "Content-Length"); 
	if (lengthString != NULL){
		int index = lengthString - headerFields; 
		int offset = 16; 
		char length[200];
		char temp[2]; 
		while(headerFields[index+offset] != '\n'){
			//printf("character at index: %c\n", headerFields[index+offset]);
			temp[0] = headerFields[index+offset]; 
			temp[1] = 0; 
			strcat(length, temp);
			offset += 1;
		}
		unsigned a = 0;
		unsigned b = 0;
		for(size_t i=0; i<strlen(length) - 1; i++){
			//printf("char %d\n", length[i]);
			if (length[i] > 57 || length[i] < 48){
				continue;
			}
			b = length[i] - '0';
			a = concatenate(a, b);
			//printf("A is NOW: %d\n", a);
		}
		contentLength = a;
		memset(length, 0, 200);
		memset(temp, 0, 2);
	}
	grabFields(headerFields, "Content-Type");
	printf("contentType: %s\n", contentType);
	grabFields(headerFields, "Host");
	printf("host: %s\n", host);
	grabFields(headerFields, "User-Agent");
	printf("userAgent: %s\n", userAgent);
	grabFields(headerFields, "Accept");
	printf("accept: %s\n", acceptField);
	grabFields(headerFields, "Expect");
	printf("expect: %s\n", expect);
	//printf("contentLength : %d\n", contentLength);
	// TIME TO PRINT THE MESSAGE BODY TO CLIENT 
	lengthString = strstr(headerFields, "HEAD");
	if(lengthString != NULL){
		int index = lengthString - headerFields; 
		int offset = 6; 
		char length[200]; 
		char temp[2]; 
		memset(length, 0, 200);
		memset(temp, 0, 2);
		while(headerFields[index+offset] != ' '){
			//printf("character at index: %c\n", headerFields[index+offset]);
			temp[0] = headerFields[index+offset]; 
			temp[1] = 0; 
			strcat(length, temp);
			offset += 1;
		}
		//printf("length is: %s\n", length);
		strcpy(URI, length);
		memset(length, 0, 200);
		memset(temp, 0, 2);
		if (strlen(URI) > 19){
			send(newSockfd, BadRequestStatus, 58, 0);
			return false; 
		}
	}
	printf("URI: %s\n", URI);
	if (access(URI, F_OK) == 0){
		printf("exists\n");
	}
	else{
		write(newSockfd, NotFoundStatus, 53);
		return false; 
	}
	struct stat fileStat; 
	stat(URI, &fileStat);
	
	if( ((fileStat.st_mode & S_IRGRP) == 0) & ((fileStat.st_mode & S_IROTH) == 0) & ((fileStat.st_mode & S_IWGRP) == 0) & ((fileStat.st_mode & S_IWOTH) == 0)){
		send(newSockfd, ForbiddenStatus, 53, 0);
		return false;
	}
	if(S_ISREG(fileStat.st_mode) == 0){ // IF EQUAL TO ZERO THEN ITS A DIRECTORY
		send(newSockfd, ForbiddenStatus, 53, 0);
		return false; 
	}
	contentLength = fileStat.st_size; 
	char temp[200]; 
	sprintf(temp, " %d\r\n\r\n", contentLength);
	strcat(OKStatus, temp);
	send(newSockfd, OKStatus, strlen(OKStatus), 0);
	memset(OKStatus, 0, 200); 
	memset(temp, 0, 200);
	return true; 
}
int main(int argc, char **argv){
	if (argc < 2){
		fprintf(stderr, "port no. not provided. Program terminated\n");
		exit(1);
	}
	uint16_t sockfd;
	int newSockfd;
	int portNumber; 
	int n;
     	char headerFields[2049];
     	portNumber = atoi(argv[1]);
     	sockfd = create_listen_socket(portNumber);
    	while(true){
    		contentLength = 0; 
		memset(contentType, 0, 500);
		memset(acceptField, 0, 500);  
		memset(userAgent, 0, 500); 
		memset(host, 0, 500);  
		memset(expect, 0, 500); 
		memset(headerFields, 0, 2049);
    		newSockfd = accept(sockfd, NULL, NULL);
    		if (newSockfd < 0){
    			fprintf(stderr, "error on accept\n");
    		}
    		
    		n = read(newSockfd, headerFields, 2049);
    		if(n < 0){ // WE DID NOT READ ANYTHING 
    			fprintf(stderr, "Error on reading.\n");
    		}
    		char* lengthString = strstr(headerFields, ";");
    		if(lengthString !=NULL){
    			send(newSockfd, BadRequestStatus, 58, 0);
    			close(newSockfd);
    			continue; 
    		}
    		char *getRequest = "GET"; 
    		char *putRequest = "PUT"; 
    		char *headRequest = "HEAD"; 
    		char *getType = strstr(headerFields, getRequest);
    		char *putType = strstr(headerFields, putRequest);
    		char *headType = strstr(headerFields, headRequest);

    		for(int i=0; i<3; i++){
    			if(getType != NULL){
    				getFunction(newSockfd, headerFields);    				
    				break;
    			}
    			if(putType != NULL){
    				putFunction(newSockfd, headerFields);
    				break;
    			}
    			if(headType != NULL){
    				headFunction(newSockfd, headerFields);
    				break;
    			}
    			send(newSockfd, NotImplemented, 72, 0);
    			break;
    		}
    		close(newSockfd);
    	} 
 
    	close(sockfd);
    	return 0; 
    	
}














