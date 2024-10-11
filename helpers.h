#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "parson.h"

#define BUFLEN 4096
#define LINELEN 1000
#define MAX_INPUT 100
#define MAX_JSON_STRING 1500
#define MAX_LENGTH 1000
#define PORT 8080
#define HOST "34.246.184.49"

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char *receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

struct book {
    int id;
    char title[MAX_INPUT];
    char author[MAX_INPUT];
    char publisher[MAX_INPUT];
    char genre[MAX_INPUT];
    int page_count;
};

struct user {
    char username[MAX_INPUT];
    char password[MAX_INPUT];
};

struct serialize_ret {
    char *serialized_string;
    JSON_Value *root_value;
};

//  Serialize user
struct serialize_ret serialize_user(struct user us);
struct serialize_ret serialize_book(struct book b);
void free_serialized(char *serialized_string, JSON_Value *root_value);

void read_input(char *display, char *save);
int validate_book(struct book b);

//  Macro for error checking
#define DIE(assertion, call_description)                                       \
  do {                                                                         \
    if (assertion) {                                                           \
      fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                       \
      perror(call_description);                                                \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

