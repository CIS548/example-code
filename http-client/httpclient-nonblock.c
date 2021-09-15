/*
 * This code is to demonstrate an implementation of a http client, which
 * requests three web pages: www.baidu.com, www.google.com and www.microsoft.com.
 * Compared to the blocking client, this client is non-blocking in
 * the sense that it is not blocked on slow servers. It always receives quick 
 * responses first, which makes it more efficient than the blocking client.
 *
 * Specifically, this implementation creates a socket for each page. It calls 
 * build_connection() to connect the server of each page. And then it calls send()
 * to send http requests. After that, it uses a loop to iteratively reveive http
 * responses from those web servers. Within each loop, it uses pselect() (select()
 * can also work) to select a socket that is available to receive. This ensures
 * that the client always receives responses from fast servers first. It exits the
 * loop when all three pages have been received.
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define SERVER_PORT 80
#define MAX_LINE 1024
#define NUM_REQUESTS 3

int build_connection(const char** hostnames, const char** queries, int* sockets)
{
	for(int i = 0; i < NUM_REQUESTS; i++)
	{
		/* translate host name into peer’s IP address */
		struct hostent *hp;
		struct sockaddr_in sin;
		hp = gethostbyname(hostnames[i]);
		if (!hp) {
			fprintf(stderr, "unknown host: %s\n", hostnames);
			return 1;
		}
		/* build address data structure */
		bzero((char *)&sin, sizeof(sin));
		sin.sin_family = AF_INET;
		bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
		sin.sin_port = htons(80);
		/* open a socket */
		if ((sockets[i] = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
			perror("socket");
			return 1;
		}
		/* connect */
		if (connect(sockets[i], (struct sockaddr *)&sin, sizeof(sin)) < 0) {
			perror("connect");
			close(sockets[i]);
			return 1;
		}
	}

	return 0;
}

int main(int argc, char * argv[])
{
	const char* hostnames[NUM_REQUESTS] = {"www.baidu.com", "www.google.com", "www.microsoft.com"};
	const char* queries[NUM_REQUESTS] = {"GET / HTTP/1.0\r\nHost: www.baidu.com\r\n\r\n", "GET / HTTP/1.0\r\nHost: www.google.com\r\n\r\n", "GET / HTTP/1.0\r\nHost: www.microsoft.com\r\n\r\n"};
	
	int sockets[NUM_REQUESTS];
	char buf[MAX_LINE];

	if(build_connection(hostnames, queries, sockets))
		exit(1); //something bad happened.
	
	// send the queries
	for(int i = 0; i < NUM_REQUESTS; i++)
		send(sockets[i], queries[i], strlen(queries[i])+1, 0);
	

	int is_exit = 0;
	int count_down = NUM_REQUESTS;
	while(!is_exit)
	{
		fd_set rfds; // need to be initialized everytime we call p/select
		FD_ZERO(&rfds);
		for(int i = 0; i < NUM_REQUESTS; i++)
		{
			if(sockets[i] <= 0)
				continue;
			fcntl(sockets[i], F_SETFL, O_NONBLOCK);
			FD_SET(sockets[i], &rfds);
		}
		int max_fd = 0;
		for(int i = 0; i< NUM_REQUESTS; i++)
			if(sockets[i] > max_fd)
				max_fd = sockets[i];

		int res_select = pselect(max_fd+1, &rfds, NULL, NULL, NULL, NULL);
		if(res_select < 0)
		{
			perror("pselect");
			exit(1);
		}
		else if(res_select == 0)
			continue;
		for(int i = 0; i < NUM_REQUESTS; i++)
		{
			if(FD_ISSET(sockets[i], &rfds))
			{
				int res_recv = recv(sockets[i], buf, sizeof(buf), 0);
				if(res_recv < 0)
				{
					is_exit = 1;
					perror("recv");
					break;
				}
				else if(res_recv > 0)
					fputs(buf, stdout);
				else
				{
					close(sockets[i]);
					sockets[i] = -1;
					if((--count_down) == 0)
						is_exit = 1;
				}
			}
		}
	}
}
