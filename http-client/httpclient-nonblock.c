/*
 * This code is to demonstrate an implementation of a http client, which
 * requests three web pages: www.baidu.com, www.google.com and
 * www.microsoft.com. Compared to the blocking client, this client is
 * non-blocking in the sense that it is not blocked on slow servers. It always
 * receives quick responses first, which makes it more efficient than the
 * blocking client.
 *
 * Specifically, this implementation creates a socket for each page. It calls
 * build_connection() to connect the server of each page. And then it calls
 * send() to send http requests. After that, it uses a loop to iteratively
 * reveive http responses from those web servers. Within each loop, it uses
 * pselect() (select() can also work) to select a socket that is available to
 * receive. This ensures that the client always receives responses from fast
 * servers first. It exits the loop when all three pages have been received.
 *
 * */

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 80
#define MAX_LINE 1024
#define NUM_REQUESTS 3

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
  // initialize buf
  memset(buf, 0, sizeof(buf));

  if (build_connection(hostnames, queries, sockets))
    exit(1);  // something bad happened.

  // send the GETs requests
  for (int i = 0; i < NUM_REQUESTS; i++)
    send(sockets[i], queries[i], strlen(queries[i]) + 1, 0);

  int is_exit = 0;
  int count_down = NUM_REQUESTS;
  // MAIN EVENT LOOP: continuously executes until all sockets are closed,
  // i.e., servers of the three pages have sent the full responses and
  // terminated the connections.
  while (!is_exit) {
    fd_set rfds;
    // adding all open sockets to the set
    FD_ZERO(&rfds);
    for (int i = 0; i < NUM_REQUESTS; i++) {
      if (sockets[i] <= 0)  // this socket has been closed
        continue;
      fcntl(sockets[i], F_SETFL, O_NONBLOCK);
      FD_SET(sockets[i], &rfds);
    }
    int max_fd = 0;
    for (int i = 0; i < NUM_REQUESTS; i++)
      if (sockets[i] > max_fd)
        max_fd = sockets[i];

    // EVENT HANDLING
    // pselect() blocks until one of the sockets is ready to receive
    int res_select = pselect(max_fd + 1, &rfds, NULL, NULL, NULL, NULL);
    if (res_select < 0) {
      perror("pselect");
      exit(1);
    } else if (res_select == 0)
      continue;

    // iterates through the sockets to find the one that is ready to receive
    // i.e., has data to read
    for (int i = 0; i < NUM_REQUESTS; i++) {
      // this socket has data to read; proceed to receive
      if (FD_ISSET(sockets[i], &rfds)) {
        int res_recv = recv(sockets[i], buf, sizeof(buf), 0);

        // null-terminate the buffer
        buf[res_recv] = '\0';

        // on error: close the socket and set the socket to -1
        if (res_recv < 0) {
          is_exit = 1;
          perror("recv");
          break;
        }
        // on success (connection not closed yet)
        else if (res_recv > 0) {
          printf("\n\n--------Received from %s:---------\nLength: %lu\n",
                 hostnames[i], strlen(buf));
          // Uncomment the following line to see the content of the response!
          // printf("%s\n", buf);
          memset(buf, 0, sizeof(buf));
        }
        // full message has been sent by the server and received by the client
        // and the server has terminated the connection.
        else {
          close(sockets[i]);
          sockets[i] = -1;
          if ((--count_down) == 0)
            is_exit = 1;
        }
      }
    }
  }
}
