/*
 * This code is to demonstrate an implementation of a http client, which
 * requests three web pages: www.baidu.com, www.google.com and
 * www.microsoft.com. This client is blocking in the sense that it requests
 * those three pages one after another. The next page is to be received only
 * when the previous page is entirely received, which can be slow.
 *
 * Specifically, this implementation creates a socket for each page. It calls
 * build_connection() to connect the server of each page. And then it calls
 * send() to send http requests. Finally, it calls recv() to read http responses
 * from corresponding sockets.
 *
 * */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 80
#define MAX_LINE 1024
#define NUM_REQUESTS 3

/** boilerplate code to build connections to the three servers (baidu, google,
 * microsoft). Works by identifying the server by resolving the hostname, then
 * creating the sockaddr_in struct. A socket is then creating connecting to
 * port 80 of the server (http).
 */

int build_connection(const char** hostnames,
                     const char** queries,
                     int* sockets) {
  for (int i = 0; i < NUM_REQUESTS; i++) {
    /* translate host name into peer’s IP address */
    struct hostent* hp;
    struct sockaddr_in sin;
    hp = gethostbyname(hostnames[i]);
    if (!hp) {
      fprintf(stderr, "unknown host: %s\n", *hostnames);
      return 1;
    }
    /* build address data structure */
    bzero((char*)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char*)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(80);
    /* open a socket */
    if ((sockets[i] = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket");
      return 1;
    }
    /* connect */
    if (connect(sockets[i], (struct sockaddr*)&sin, sizeof(sin)) < 0) {
      perror("connect");
      close(sockets[i]);
      return 1;
    }
  }

  return 0;
}

int main(int argc, char* argv[]) {
  const char* hostnames[NUM_REQUESTS] = {"www.baidu.com", "www.google.com",
                                         "www.microsoft.com"};
  const char* queries[NUM_REQUESTS] = {
      "GET / HTTP/1.0\r\nHost: www.baidu.com\r\n\r\n",
      "GET / HTTP/1.0\r\nHost: www.google.com\r\n\r\n",
      "GET / HTTP/1.0\r\nHost: www.microsoft.com\r\n\r\n"};

  int sockets[NUM_REQUESTS];
  char buf[MAX_LINE];

  if (build_connection(hostnames, queries, sockets))
    exit(1);  // something bad happened.

  // Send the GET requests: one for each page.
  // The GET requests will return a HTTP GET response header, followed by the
  // HTML content of the page.
  for (int i = 0; i < NUM_REQUESTS; i++)
    send(sockets[i], queries[i], strlen(queries[i]) + 1, 0);

  // receives response from each of the servers (_linearly_)
  for (int i = 0; i < NUM_REQUESTS; i++) {
    printf("\n\n######################### FROM %s #########################\n",
           hostnames[i]);
    // we are waiting until the entire response is received, blocking
    // on sending other requests.
    int s;
    while ((s = recv(sockets[i], buf, sizeof(buf), 0)) > 0) {
      // null terminate the buffer
      buf[s] = '\0';
      printf("----------Length: %lu------------\n", strlen(buf));
      // Uncomment the following line to see the content of the response!
      // printf("Content: %s\n", buf);
    }
    close(sockets[i]);
  }
}
