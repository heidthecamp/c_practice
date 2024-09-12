#include <stdarg.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <opencv/cv.h> 
#include <opencv/highgui.h>
#include <opencv2/imgcodecs/imgcodecs_c.h>

void log_m(const char* __restrict__ format, ...) {
    va_list args;
    va_start(args, format);
    char message[1024];
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    printf("%s\n", message);
}


// Image url
// https://picsum.photos/200/300
// https://web.mit.edu/files/images/202409/240626-WSP-MIT-Eisinger_Portrait-0313-SL.jpg

int main(int argc, const char* argv[])
{
    const char *domain = "web.mit.edu";
    const char *path = "/files/images/202409/240626-WSP-MIT-Eisinger_Portrait-0313-SL.jpg";
    if (argc == 3) {
        domain = argv[1];
        path = argv[2];
    } 
    struct addrinfo hints, *address;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int result;
    result = getaddrinfo(domain, "80", &hints, &address);
    if (result != 0) {
        log_m("Failed to getaddrinfo");
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(address->ai_family,address->ai_socktype,address->ai_protocol);
    if (sockfd == -1) {
        log_m("Failed to get socket");
        exit(EXIT_FAILURE);
    }


    result = connect(sockfd, address->ai_addr, address->ai_addrlen);
    if (result != 0) {
        log_m("Failed to connect");
        exit(EXIT_FAILURE);
    }

    char header[1024];
    snprintf(header, sizeof(header), "GET %s HTTP/1.1\nHOST: %s\n\n",
        path, domain);

    result = send(sockfd,header,strlen(header),0);
    if (result == -1 ) {
        log_m("Failed to send");
        exit(EXIT_FAILURE);
    }

    char response_buf[4096];
    int byte_count = recv(sockfd, response_buf, sizeof(response_buf) - 1, 0);
    if (byte_count == -1) {
        log_m("Failed to recv response");
        exit(EXIT_FAILURE);
    }

    response_buf[byte_count] = '\0';

    char *header_end = strstr(response_buf, "\r\n\r\n") + 4;

    size_t data_size = byte_count - (header_end - response_buf);

    int http_status = 0;
    int scan_res = sscanf(response_buf, "HTTP/1.1 %d ", &http_status);
    if (scan_res < 1){
        log_m("Could not find HTTP code");
        exit(EXIT_FAILURE);
    }

    if (http_status != 200) {
        log_m("HTTP ERROR: %d", http_status);
        exit(EXIT_FAILURE);
    }

    char *header_ptr = strstr(response_buf, "\r\n") + 2;
    if (!header_ptr){
        log_m("Could not find header_ptr");
        exit(EXIT_FAILURE);
    }

    char key[15];
    // char str_value[64];
    int int_value = 0;

    char search_key[15] = "Content-Length";
    
    int content_length = 0;
    bool found = false;
    while (header_ptr < header_end - 5 && !found) {
        scan_res = sscanf(header_ptr, "%14s: %d\r\n", key, &int_value);
        if (scan_res == 2) {
            if (strcmp(key, search_key) == 0) {
                content_length = int_value;
                found = true;
            }
        }
        header_ptr = strstr(header_ptr, "\r\n") + 2;
        if (!header_ptr){
            log_m("Could not find header_ptr");
            exit(EXIT_FAILURE);
        }
    }

    if (!content_length){
        log_m("Could not get content length");
        exit(EXIT_FAILURE);
    }

    char *body_buf = malloc(content_length);
    size_t remaining_length = content_length;
    memcpy(body_buf, header_end, data_size);
    remaining_length -= data_size;

    char *bodyputr = body_buf + data_size;
    while (remaining_length > 0) {
        byte_count = recv(sockfd, bodyputr, remaining_length, 0);
        if (byte_count == -1) {
            free(body_buf);
            log_m("Error: Did not recv response.");
            exit(EXIT_FAILURE);
        }

        remaining_length -= byte_count;
        bodyputr += byte_count;
    }

    FILE *fptr = fopen("test.jpg", "wb");
    if (!fptr) {
        free(body_buf);
        log_m("Failed to open the ");
        exit(EXIT_FAILURE);
    }
    

    byte_count = fwrite(body_buf, 1, content_length, fptr);
    if (byte_count < content_length) {
        free(body_buf);
        log_m("Failed to write entire file");
        exit(EXIT_FAILURE);
    }

    fclose(fptr);
    free(body_buf);

    log_m("Successfully saved image to test.jpg");

    return EXIT_SUCCESS;
}
