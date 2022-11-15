Maxim Joel Carbonell-Kiamtia
mjcarbon
Fall 2022
asgn1

---------------------
DESCRIPTION: This directory is for the purposes of starting an httpserver on a particular port number that can take in reqeusts from a different terminal i.e. GET, HEAD, and PUT. 

1. Enter "make" in the terminal
2. Enter "./httpserver <port>"
---------------------
FILES
Makefile
httpserver.c
README.md

-----------------------
httpserver.c contains the c program to perform all of the logic needed to set up a server with a socket that is capable of HTTP requests... i.e. HEAD, GET, PUT. PUT requests allow for updating/replacing contents of a file that is outlined by the URI. It's response code will be 201 for creation of a file and 200 for the update of a file. GET Requests will always return a status code of 200 on success and are for the purposes of retrieving the contents of a file i.e. the content length, and most importantly the message body. HEAD requests are identical to GET requests, however they are slightly different since they do not provide the message body. 

---------------------
DESIGN
httpserver.c has a design that outlines functions outside of main... i.e. readFile and readBinary, headFunction, getFunction, putFunction, cocncatenate, and grabFields. 

readFile() is used to read in text files and binary files from the user. It instantiates a buffer size of 4096(relatively large enough to save "trips" back and forth, while also not takign up too  unnecessary memory. It then opens and generates a file descriptor that we use to feed characters into our buffer. If the value return by our read() call is negative, it returns a missing file error. If value is positive, then that is our file size and our buffer has characters to work with. We repeat this process until the fileSize returned is less than the buffer/until the file size is zero and we reached the end. Note that we iterate through the buffer and if we find a character resembling the delimeter we replace the value of that character with a newline '\n'. At the end of the iterration we use the write() syscall to output the newly formatted text/buffer contents into stdout. 

readBinary() has very similar functionality to readFile(), although it reads from the file given through a GET request's URI and writes to the socket. It is capable of reading text and binary files. 

main() takes in all arguments and generates a seerver with a given port. It does this by using the provided function create_listen_socket(). It then peprpetuates an infinite while loop where they server a new socket descriptor is generated and closed each iteration. This way we can get a request, determine if it is a GET, PUT, or HEAD and send the program to the appropriate function. Main also checks if there are poorly formatted requests or unimplemented methods. 

getFunction() is the function that is used when we detect a get request. It is finds the URI, header fields(by using grabFields), checks if the file exists, checks if the file has appropriate permissions, and constructs an OKStatus resposne with appropriate content length and message body. 

headFunction() is identical to the getFunction() with the exception of sending the message body. 

putFunction() is the function that is used when we detect a put request. It finds rhe URI and other header fields with grabFields() and determines if the content length exists. If it doesn't it sends a bad request response. If it does it determiens if it should read the file once more if it has an "expect" header. If it doesn't have an expect header, then putFunction() iterates the file more and locates the "\r\n\r\n", since the message body is directly udnerneath it. If it finds that the content length is not equal to the rest of the file the server will hang. If it does then we keep reading until we have written enough characters that is equivalent to the content length. 

grabFields() is the function we used to store the header fields that we mostly care about i.e. Accept, Content-Length, Host, User-Agent, Expect, URI, ...  
---------------------
TESTING AND ERROR HANDLING 

To operate the program the user types... "./httpserver <port>"... Now the server is live and the user can open another terminal elsewhere to send requests. 

1.) The user can type "curl -X GET localhost:<port>/<file>" to perform a GET request and retrieve the contents of <file> 
2.) The user can type "curl -X PUT localhost:<port>/<outFile> -T <inFile>" to perform a PUT request and updating/writing the contents of inFile (from the client's directory) into the contents of outFile (from the server's directory)
3.) The user can type "curl -I localhost:<port>/<file>" to perform a HEAD request and retrieve details of <file>
4.) The user can type "curl -X POST localhost:<port>/<file>" to perform recieve an unimplemted error, as we did not implement post 
5.) the user can type ""printf "GET /<infile> HTTP/1.1\r\nMalformed-Header; meow\r\n\r\n" | nc -q -1 localhost <port> &" to recieved a 400 error, since the header has a ';' instread of a ':'
6.) the user can type "curl -X GET localhost:<port>/<noPermissionFile>" to perform a GET request and receive 403 error since the file cannot be opened






