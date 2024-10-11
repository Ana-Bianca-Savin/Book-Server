#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
 
// Function used to compute a GET request
char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char **tokens, int tokens_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    DIE(message == NULL, "Memory");
    char *line = calloc(LINELEN, sizeof(char));
    DIE(line == NULL, "Memory");

    //  Write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    //  Add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    //  Extract cookies and add them
    if (cookies != NULL) {
        memset(line, 0, LINELEN);

        //  Get cookies_count cookies and format them
        sprintf(line, "Cookie: ");
 
        for (int i = 0; i < cookies_count; i++) {
            strcat(line, cookies[i]);

            if (i != cookies_count - 1)
                strcat(line, "; ");
        }

        compute_message(message, line);
    }

    //  Extract JWT tokens and add them
    if (tokens != NULL) {
        memset(line, 0, LINELEN);

        //  Get token_count tokens and format them
        sprintf(line, "Authorization: Bearer ");

        for (int i = 0; i < tokens_count; i++) {
            strcat(line, tokens[i]);

            if (i != cookies_count - 1)
                strcat(line, "; ");
        }

        compute_message(message, line);
    }

    //  Add final new line
    compute_message(message, "");
    return message;
}
 
// Function used to compute a POST requests
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count,
                            char **cookies, int cookies_count,
                            char **tokens, int tokens_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    DIE(message == NULL, "Memory");
    char *line = calloc(LINELEN, sizeof(char));
    DIE(line == NULL, "Memory");
    char *body_data_buffer = calloc(LINELEN, sizeof(char));
    DIE(body_data_buffer == NULL, "Memory");

    //  Write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    //  Add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    //  Add necessary headers (Content-Type and Content-Length are mandatory)
    //  in order to write Content-Length you must first compute the message size
    //  Add content type
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    //  Count length
    int content_length = 0;
    for (int i = 0; i < body_data_fields_count; i++) {
        strcat(body_data_buffer, body_data[i]);
        content_length += strlen(body_data[i]);
    }

    //  Add content length
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %d", content_length);
    compute_message(message, line);

    //  Extract cookies and add them
    if (cookies != NULL) {
        memset(line, 0, LINELEN);

        //  Get cookies_count cookies and format them
        sprintf(line, "Cookie: ");
 
        for (int i = 0; i < cookies_count; i++) {
            strcat(line, cookies[i]);

            if (i != cookies_count - 1)
                strcat(line, "; ");
        }

        compute_message(message, line);
    }

    //  Extract JWT tokens and add them
    if (tokens != NULL) {
        memset(line, 0, LINELEN);

        //  Get token_count tokens and format them
        sprintf(line, "Authorization: Bearer ");

        for (int i = 0; i < tokens_count; i++) {
            strcat(line, tokens[i]);

            if (i != cookies_count - 1)
                strcat(line, "; ");
        }

        compute_message(message, line);
    }

    //  Add new line at end of header
    compute_message(message, "");

    //  Add the actual payload data
    memset(line, 0, LINELEN);
    strcat(message, body_data_buffer);

    free(line);
    return message;
}

//  Function used to compute a delete request
char *compute_delete_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char **tokens, int tokens_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    DIE(message == NULL, "Memory");
    char *line = calloc(LINELEN, sizeof(char));
    DIE(line == NULL, "Memory");

    //  Write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    //  Add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    //  Extract cookies and add them
    if (cookies != NULL) {
        memset(line, 0, LINELEN);

        //  Get cookies_count cookies and format them
        sprintf(line, "Cookie: ");
 
        for (int i = 0; i < cookies_count; i++) {
            strcat(line, cookies[i]);

            if (i != cookies_count - 1)
                strcat(line, "; ");
        }

        compute_message(message, line);
    }

    //  Extract JWT tokens and add them
    if (tokens != NULL) {
        memset(line, 0, LINELEN);

        //  Get token_count tokens and format them
        sprintf(line, "Authorization: Bearer ");

        for (int i = 0; i < tokens_count; i++) {
            strcat(line, tokens[i]);

            if (i != cookies_count - 1)
                strcat(line, "; ");
        }

        compute_message(message, line);
    }

    //  Add final new line
    compute_message(message, "");
    return message;
}