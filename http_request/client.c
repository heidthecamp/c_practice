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

int main(int argc, char const* argv[])
{
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int result;
    getaddrinfo("picsum.photos", "80", &hints, &res);

    int sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

    log_m("Connecting...");

    connect(sockfd, res->ai_addr, res->ai_addrlen);
    
    log_m("Connected!!!");

    char *header = "GET /files/images/202409/240626-WSP-MIT-Eisinger_Portrait-0313-SL.jpg HTTP/1.1\nHOST: web.mit.edu\n\n";

    send(sockfd,header,strlen(header),0);

    log_m("GET sent...");
    char response_buf[4096];
    int byte_count = recv(sockfd, response_buf, sizeof(response_buf) - 1, 0);

    response_buf[byte_count] = '\0';

    // char *header_end = strstr(response_buf, "\n") + 2;
    char *header_end = strstr(response_buf, "\r\n\r\n") + 4;

    // log_m("%d - (%p - %p)", byte_count, header_end, response_buf);

    size_t data_size = byte_count - (header_end - response_buf);
    
    // log_m("%zu", data_size);

    // log_m("%s", response_buf);

    int http_status = 0;
    int scan_res = sscanf(response_buf, "HTTP/1.1 %d ", &http_status);
    log_m("HTTP RESPONSE CODE: %d", http_status);

    if (http_status != 200) {
        log_m("HTTP ERROR: %d", http_status);
        exit(EXIT_FAILURE);
    }

    char *header_ptr = strstr(response_buf, "\r\n") + 2;

    char key[15];
    // char str_value[64];
    int int_value = 0;

    char search_key[15] = "Content-Length";
    
    int content_length = 0;
    bool found = false;
    while(header_ptr < header_end - 5 && !found) {
        scan_res = sscanf(header_ptr, "%14s: %d\r\n", key, &int_value);
        if(scan_res == 2) {
            if(strcmp(key, search_key) == 0) {
                content_length = int_value;
                found = true;
            }
        }
        header_ptr = strstr(header_ptr, "\r\n") + 2;
    }

    if (!content_length){
        log_m("Could not get content length");
        exit(EXIT_FAILURE);
    }

    log_m("Content-length: %d", content_length);

    char *body_buf = malloc(content_length);
    size_t remaining_length = content_length;
    memcpy(body_buf, header_end, data_size);
    remaining_length -= data_size;

    char *bodyputr = body_buf + data_size;
    while (remaining_length > 0) {
        log_m("remaining length: %d", remaining_length);
        byte_count = recv(sockfd, bodyputr, remaining_length, 0);
        remaining_length -= byte_count;
        bodyputr += byte_count;
    }

    // IplImage *im = cvDecodeImage(body_buf, CV_LOAD_IMAGE_COLOR);

    FILE *fptr = fopen("test.jpg", "wb");
    
    fwrite(body_buf, 1, sizeof(body_buf), fptr);

    fclose(fptr);
    free(body_buf);

    log_m("Successfully saved image to test.jpg");

    exit(EXIT_SUCCESS);
}
