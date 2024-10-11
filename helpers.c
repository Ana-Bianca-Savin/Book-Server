#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "buffer.h"
#include "parson.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

struct serialize_ret serialize_user(struct user us) {
    //  Build JSON string for user
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", us.username);
    json_object_set_string(root_object, "password", us.password);
    serialized_string = json_serialize_to_string_pretty(root_value);

    //  Initialize struct for return
    struct serialize_ret ret;
    ret.serialized_string = malloc(MAX_JSON_STRING * sizeof(char));
    DIE(ret.serialized_string == NULL, "Memory");

    strncpy(ret.serialized_string, serialized_string, strlen(serialized_string));
    ret.root_value = root_value;
    return ret;
}

void free_serialized(char *serialized_string, JSON_Value *root_value) {
    //  Free JSON string
    json_free_serialized_string(serialized_string);
    //  Free root value
    json_value_free(root_value);
}

struct serialize_ret serialize_book(struct book b) {
    //  Build JSON string for book
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "title", b.title);
    json_object_set_string(root_object, "author", b.author);
    json_object_set_string(root_object, "genre", b.genre);
    json_object_set_string(root_object, "publisher", b.publisher);
    json_object_set_number(root_object, "page_count", b.page_count);
    serialized_string = json_serialize_to_string_pretty(root_value);

    //  Initialize struct for return
    struct serialize_ret ret;
    ret.serialized_string = malloc(MAX_JSON_STRING * sizeof(char));
    DIE(ret.serialized_string == NULL, "Memory");

    strncpy(ret.serialized_string, serialized_string, strlen(serialized_string));
    ret.root_value = root_value;
    return ret;
}

void read_input(char *display, char *save) {
    printf("%s=", display);
    char *res = fgets(save, MAX_INPUT, stdin);
    DIE(!res, "fgets");

    //  Replace new line character
    int len = strlen(save);
    if (save[len - 1] == '\n')
        save[len - 1] = '\0';
}

int validate_book(struct book b) {
    //  Check if page count is 0
    if (b.page_count == 0) {
        printf("ERROR - Invalid number of pages - 400\n");
        return 0;
    }
    //  Check if a field is null
    if (strcmp(b.title, "") == 0 || strcmp(b.author, "") == 0 || strcmp(b.publisher, "") == 0
        || strcmp(b.genre, "") == 0) {
        printf("ERROR - Fields can not be empty - 400\n");
        return 0;
    }

    //  Valid info
    return 1;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}
