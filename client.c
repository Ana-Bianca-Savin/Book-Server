#include "helpers.h"
#include "requests.h"

void run_client() {
    //  Check when client loop closes
    int hasClosed = 0;

    //  Cookie for logged in user
    char *cookie = malloc(MAX_LENGTH * sizeof(char));
    DIE(cookie == NULL, "Memory");
    memset(cookie, 0, MAX_LENGTH);

    //  Token for entering the library
    char *token = malloc(MAX_LENGTH * sizeof(char));
    DIE(token == NULL, "Memory");
    memset(token, 0, MAX_LENGTH);

    while (!hasClosed) {
        //  Read input from stdin
        char *str = malloc(MAX_INPUT * sizeof(char));
        char *res = fgets(str, MAX_INPUT, stdin);
        DIE(!res, "fgets");

        //  Replace new line character
        if (str[strlen(str)-1] == '\n')
            str[strlen(str)-1] = '\0';

        //  Check what the client wants
        if (strncmp(str, "exit", 5) == 0) {
            //  Client wants to close
            hasClosed = 1;
        } else if (strncmp(str, "register", 9) == 0) {
            //  Client wants to register
            struct user us;
            //  Read username
            read_input("username", us.username);
            //  Read password
            read_input("password", us.password);

            //  Serialize read user, so that it has the JSON format
            struct serialize_ret ret = serialize_user(us);

            //  Open connection
            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            //  Format post request with given information to register route
            char *post = compute_post_request(HOST, "/api/v1/tema/auth/register", "application/json",
                    &ret.serialized_string, 1, NULL, 0, NULL, 0);
            //  Send post request to server
            send_to_server(sockfd, post);

            //  Receive status from server
            char *status = receive_from_server(sockfd);
            DIE(status == NULL, "receive_from_server");

            //  Check if error occurred
            if (strstr(status, "error")) {
                //  Check what kind of error it is
                if (strstr(status, "taken!")) {
                    //  Check if error is that the username is taken
                    printf("ERROR - Username is already taken! - 400\n");
                } else {
                    //  Server error
                    printf("ERROR - Server error, please try again later! - 500\n");
                }
            } else {
                //  Success
                printf("SUCCESS - User was registered! - 200\n");
            }

            //  Close connection
            close_connection(sockfd);

            //  Free space
            free_serialized(ret.serialized_string, ret.root_value);
        } else if (strncmp(str, "login", 6) == 0) {
            //  Client wants to login
            struct user us;
            //  Read username
            read_input("username", us.username);
            //  Read password
            read_input("password", us.password);

            //  Serialize read user, so that it has the JSON format
            struct serialize_ret ret = serialize_user(us);

            //  Open connection
            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            //  Format post request with given information to login route
            char *post = compute_post_request(HOST, "/api/v1/tema/auth/login", "application/json",
                    &ret.serialized_string, 1, NULL, 0, NULL, 0);
            //  Send post request to server
            send_to_server(sockfd, post);

            //  Receive status from server
            char *status = receive_from_server(sockfd);
            DIE(status == NULL, "receive_from_server");

            //  Check if error occurred
            if (strstr(status, "error")) {
                //  Check what kind of error it is
                if (strstr(status, "No account")) {
                    //  Check if the username does not exist
                    printf("ERROR - No account with this username exists! - 400\n");
                } else if (strstr(status, "Credentials")) {
                    //  Check if password did not match
                    printf("ERROR - Password does not match! - 400\n");
                } else {
                    //  Server error
                    printf("ERROR - Server error, please try again later! - 500\n");
                }
            } else {
                //  Success
                printf("SUCCESS - User is now logged in! - 200\n");
                //  Get cookie
                int cookie_len = strlen( strstr(status, "connect"))
                    - strlen(strchr(strstr(status, "connect"), ';'));
                strncpy(cookie, strstr(status, "connect"), cookie_len);
                cookie[cookie_len + 1] = '\0';
            }

            //  Close connection
            close_connection(sockfd);

            //  Free space
            free_serialized(ret.serialized_string, ret.root_value);
        } else if (strncmp(str, "enter_library", 14) == 0) {
            //  Open connection
            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            //  Format get request to enter_library route
            char *get = compute_get_request(HOST, "/api/v1/tema/library/access", NULL, &cookie, 1, NULL, 0);
            //  Send get request to server
            send_to_server(sockfd, get);

            //  Receive status from server
            char *status = receive_from_server(sockfd);
            DIE(status == NULL, "receive_from_server");

            //  Check if error occurred
            if (strstr(status, "error")) {
                if (strstr(status, "You are not logged in!")) {
                    //  User is not logged in
                    printf("ERROR - User must be logged in to enter the library - 401\n");
                } else {
                    //  Server error
                    printf("ERROR - Server error, please try again later! - 500\n");
                }
            } else {
                //  Success
                printf("SUCCESS - User has access to the library! - 200\n");
                //  Get token
                int token_len = strlen(strstr(status, "\":\"") + 3) - 2;
                strncpy(token, strstr(status, "\":\"") + 3, token_len);
                token[token_len + 1] = '\0';
            }

            //  Close connection
            close_connection(sockfd);
        } else if (strncmp(str, "get_books", 10) == 0) {
            //  Open connection
            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            //  Format get request to enter_library route
            char *get = compute_get_request(HOST, "/api/v1/tema/library/books", NULL, &cookie, 1, &token, 1);
            //  Send get request to server
            send_to_server(sockfd, get);

            //  Receive status from server
            char *status = receive_from_server(sockfd);
            DIE(status == NULL, "receive_from_server");

            //  Check if error occurred
            if (strstr(status, "error")) {
                printf("ERROR - User must be logged in and have entered the library - 403\n");
            } else {
                //  Success
                printf("%s\n", strchr(status, '['));
            }

            //  Close connection
            close_connection(sockfd);
        } else if (strncmp(str, "get_book", 9) == 0) {
            //  Client wants to see data about a certain book
            char id_string[MAX_INPUT];
            read_input("id", id_string);

            //  Open connection
            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            //  Format get request to enter_library route
            char route_name[MAX_LENGTH];
            strcpy(route_name, "/api/v1/tema/library/books/\0");
            strcat(route_name, id_string);
            char *get = compute_get_request(HOST, route_name, NULL, &cookie, 1, &token, 1);
            //  Send get request to server
            send_to_server(sockfd, get);

            //  Receive status from server
            char *status = receive_from_server(sockfd);
            DIE(status == NULL, "receive_from_server");

            //  Check if error occurred
            if (strstr(status, "error")) {
                if (strstr(status, "No book was found!")) {
                    printf("ERROR - Book with id %s not found - 404\n", id_string);
                } else {
                    printf("ERROR - User must be logged in and have entered the library - 403\n");
                }
            } else {
                //  Success
                printf("%s\n", strchr(status, '{'));
            }

            //  Close connection
            close_connection(sockfd);
        } else if (strncmp(str, "add_book", 9) == 0) {
            //  Client wants to add a book
            struct book b;
            char pc_string[MAX_INPUT];
            //  Read all fields
            read_input("title", b.title);
            read_input("author", b.author);
            read_input("genre", b.genre);
            read_input("publisher", b.publisher);
            read_input("page_count", pc_string);
            b.page_count = atoi(pc_string);

            //  Skip if book information is invalid
            if (!validate_book(b))
                continue;

            //  Serialize book, so that it has the JSON format
            struct serialize_ret ret = serialize_book(b);

            //  Open connection
            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            //  Format post request with given information to register route
            char *post = compute_post_request(HOST, "/api/v1/tema/library/books", "application/json",
                    &ret.serialized_string, 1, &cookie, 1, &token, 1);
            //  Send post request to server
            send_to_server(sockfd, post);

            //  Receive status from server
            char *status = receive_from_server(sockfd);
            DIE(status == NULL, "receive_from_server");

            if (strstr(status, "error")) {
                //  Cookie or token is wrong
                printf("ERROR - User must be logged in and have entered the library - 403\n");
            } else {
                //  Success
                printf("SUCCESS - Book was added! - 200\n");
            }

            //  Close connection
            close_connection(sockfd);

            //  Free space
            free_serialized(ret.serialized_string, ret.root_value);
        } else if (strncmp(str, "delete_book", 12) == 0) {
            //  Client wants to see data about a certain book
            char id_string[MAX_INPUT];
            read_input("id", id_string);

            //  Open connection
            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            //  Format get request to enter_library route
            char route_name[MAX_LENGTH];
            strcpy(route_name, "/api/v1/tema/library/books/\0");
            strcat(route_name, id_string);
            char *get = compute_delete_request(HOST, route_name, NULL, &cookie, 1, &token, 1);
            //  Send get request to server
            send_to_server(sockfd, get);

            //  Receive status from server
            char *status = receive_from_server(sockfd);
            DIE(status == NULL, "receive_from_server");

            //  Check if error occurred
            if (strstr(status, "error")) {
                if (strstr(status, "No book was found!")) {
                    //  Book with id does not exist
                    printf("ERROR - Book with id %s not found - 404\n", id_string);
                } else {
                    //  User is either not logged in or not in library
                    printf("ERROR - User must be logged in and have entered the library! - 403\n");
                }
            } else {
                //  Success
                printf("SUCCESS - Book deleted successfully! - 200\n");
            }

            //  Close connection
            close_connection(sockfd);
        } else if (strncmp(str, "logout", 7) == 0) {
            //  Open connection
            int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            //  Format get request to enter_library route
            char *get = compute_get_request(HOST, "/api/v1/tema/auth/logout", NULL, &cookie, 1, &token, 1);
            //  Send get request to server
            send_to_server(sockfd, get);

            //  Receive status from server
            char *status = receive_from_server(sockfd);
            DIE(status == NULL, "receive_from_server");

            //  Check if error occurred
            if (strstr(status, "error")) {
                printf("ERROR - User must be logged in to logout - 403\n");
            } else {
                //  Success
                printf("SUCCESS - User logged out - 200\n");

                //  Delete cookie and token
                memset(cookie, 0, MAX_LENGTH);
                memset(token, 0, MAX_LENGTH);
            }

            //  Close connection
            close_connection(sockfd);
        }

        free(str);
    }
}

int main(int argc, char *argv[]) {
    run_client();
    return 0;
}
