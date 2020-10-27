  /**
@file datavis.c
@Author: Samuel Gregory (samuel_gregory@student.uml.edu)
@brief This file declares all routines and allocates memory for all cross-module variables in the datavis thread
@Date 9/13/2020

Note: 
Lines 64, 69, 71, 76, 82-83, 86, 91-93, and 96-97 all contain commented out code 

**/


#include <datavis.h>
#include <main.h>
#include <pthread.h>
#include <sys/socket.h> 
#include <stdio.h>
#include <arpa/inet.h> 


//Note: No cross-module variables to declare

//routine declarations

typedef struct sockaddr sk_sockaddr; 

void * datavis_thread(void *t)
{
	int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address);  
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        //exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        //exit(EXIT_FAILURE); 
    } 

	struct timeval timeout;      
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt (server_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        perror("setsockopt failed\n");

    if (setsockopt (server_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        perror("setsockopt failed\n");

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 

    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (sk_sockaddr *)&address,sizeof(address))<0) 
    { 
        perror("bind failed"); 
        //exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 32) < 0) 
    { 
        perror("listen"); 
        //exit(EXIT_FAILURE); 
    }
	// cerr << "DataVis: Main: Server File: " << server_fd << endl ;
	while(!done)
	{
		valread = 0 ;
        char recv_buf[32] = {0} ;
		//cerr << "DataVis: " << global_p.a.exposure << endl ;
        for ( int i = 0 ; (i < sizeof(datavis_p)/PACK_SIZE) ; i++ ){
			if (done)
				break ;
			if ((new_socket = accept(server_fd, (sk_sockaddr *)&address, (socklen_t*)&addrlen))<0) 
        	{ 
            	// perror("accept"); 
				// cerr << "DataVis: Accept from socket error!" <<endl ;
        	}
            ssize_t numsent = send(new_socket,&global_p.buf[i],PACK_SIZE,0);
			//cerr << "DataVis: Size of sent data: " << PACK_SIZE << endl ;
			if ( numsent > 0 && numsent != PACK_SIZE ){
				perror("DataVis: Send: ");
				cerr << "DataVis: Reported sent data: " << numsent << "/" << PACK_SIZE << endl;
			}
            //cerr << "DataVis: Data sent" << endl ;
            //valread = read(sock,recv_buf,32);
            //cerr << "DataVis: " << recv_buf << endl ;
			close(new_socket);
		}
		//sleep(1);
		// cerr << "DataVis thread: Sent" << endl ;
	}
	close(server_fd);
	pthread_exit(NULL);
}


