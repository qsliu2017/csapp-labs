#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

char OBJECT[MAX_OBJECT_SIZE], CACHE[MAX_CACHE_SIZE / MAX_OBJECT_SIZE][MAX_OBJECT_SIZE];

typedef struct object_metadata
{
    char *url;
    size_t size;
    int lru;
} object_metadata;
object_metadata OBJECTS[MAX_CACHE_SIZE / MAX_OBJECT_SIZE];

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n",
                  *connection_hdr = "Connection: close\r\n",
                  *proxy_connection_hdr = "Proxy-Connection: close\r\n",
                  *host_hdr_format = "Host: %s\r\n";

void doit(int fd);
int search_cache(char const *url);
void add_cache(char const *url, size_t size);
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg);
/* parse url into hostname and other parts */
void parse_url(char const *url, char *hostname, char *port, char *other);

#ifndef DEBUG
int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int listenfd = Open_listenfd(argv[1]);
    struct sockaddr_storage clientaddr;
    char hostname[MAXLINE], port[MAXLINE];
    while (1)
    {
        socklen_t clientlen = sizeof(struct sockaddr_storage);
        int connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", hostname, port);
        // if (Fork())
        // {
        doit(connfd);
        Close(connfd);
        //     break;
        // }
    }
    // Wait(NULL);
}
#else
int main(int argc, char const *argv[])
{
    char hostname[MAXLINE], port[MAXLINE], other[MAXLINE];
    parse_url("http://www.cmu.edu/hub/index.html", hostname, port, other);
    if (strcmp(hostname, "www.cmu.edu") || strcmp(port, "80") || strcmp(other, "/hub/index.html"))
    {
        exit(1);
    }
    parse_url("http://www.cmu.edu/", hostname, port, other);
    if (strcmp(hostname, "www.cmu.edu") || strcmp(port, "80") || strcmp(other, "/"))
    {
        exit(1);
    }
    parse_url("http://www.cmu.edu:15213/", hostname, port, other);
    if (strcmp(hostname, "www.cmu.edu") || strcmp(port, "15213") || strcmp(other, "/"))
    {
        exit(1);
    }
}
#endif

void doit(int fd)
{
    char buf[MAXLINE], method[MAXLINE], url[MAXLINE], version[MAXLINE];
    rio_t client_rio;
    Rio_readinitb(&client_rio, fd);
    printf("> %s", buf);
    if (!Rio_readlineb(&client_rio, buf, MAXLINE))
        return;
    sscanf(buf, "%s %s %s", method, url, version);

    if (strcasecmp(method, "GET"))
    {
        clienterror(fd, method, "501", "Not Implemented",
                    "Proxy does not implement this method");
        return;
    }

    int cacheline = search_cache(url);
    if (cacheline >= 0)
    {
        printf("cache hit");
        Rio_writen(fd, CACHE[cacheline], OBJECTS[cacheline].size);
        return;
    }

    char hostname[MAXLINE], port[MAXLINE], other[MAXLINE];
    parse_url(url, hostname, port, other);

    int clientfd = Open_clientfd(hostname, port);
    rio_t server_rio;
    Rio_readinitb(&server_rio, clientfd);
    // printf("> %s", buf);

    sprintf(buf, "GET %s HTTP/1.0\r\n", other);
    Rio_writen(clientfd, buf, strlen(buf));
    // printf(">> %s", buf);
    sprintf(buf, host_hdr_format, hostname);
    Rio_writen(clientfd, buf, strlen(buf));
    // printf(">> %s", buf);
    Rio_writen(clientfd, user_agent_hdr, strlen(user_agent_hdr));
    // printf(">> %s", user_agent_hdr);
    Rio_writen(clientfd, connection_hdr, strlen(connection_hdr));
    // printf(">> %s", connection_hdr);
    Rio_writen(clientfd, proxy_connection_hdr, strlen(proxy_connection_hdr));
    // printf(">> %s", proxy_connection_hdr);

    for (size_t n = 0; (n = Rio_readlineb(&client_rio, buf, MAXLINE)) != 0;)
    {
        // printf("> %s", buf);
        if (strstr(buf, "User-Agent: ") || strstr(buf, "Connection: ") || strstr(buf, "Proxy-Connection: ") || strstr(buf, "Host: "))
            continue;
        Rio_writen(clientfd, buf, n);
        // printf(">> %s", buf);
        if (!strcmp(buf, "\r\n"))
            break;
    }

    size_t object_size = 0;
    char *next = OBJECT;
    for (size_t n = 0; (n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0;)
    {
        // printf("<< %s", buf);
        object_size += n;
        if (object_size < MAX_OBJECT_SIZE)
        {
            sprintf(next, "%s", buf);
            next += n;
        }
        Rio_writen(fd, buf, n);
        // printf("< %s", buf);
        // if (!strcmp(buf, "\r\n"))
        //     break;
    }

    if (object_size < MAX_OBJECT_SIZE)
        add_cache(url, object_size);

    Close(clientfd);
}

int search_cache(char const *url)
{
    for (int i = 0; i < MAX_CACHE_SIZE / MAX_OBJECT_SIZE; i++)
    {
        sem_
        if (OBJECTS[i].url && !strcmp(OBJECTS[i].url, url))
        {
            if (OBJECTS[i].lru)
            {
                for (int idx = 0; idx < MAX_CACHE_SIZE / MAX_OBJECT_SIZE; idx++)
                    OBJECTS[idx].lru++;
                OBJECTS[i].lru = 0;
            }
            return i;
        }
    }
    return -1;
}

void add_cache(char const *url, size_t size)
{
    int index;
    for (int i = 0; i < MAX_CACHE_SIZE / MAX_OBJECT_SIZE; i++)
        if (!OBJECTS[i].url || ++(OBJECTS[i].lru) == MAX_CACHE_SIZE / MAX_OBJECT_SIZE)
        {
            index = i;
            break;
        }
    free(OBJECTS[index].url);
    OBJECTS[index].url = malloc(strlen(url) + 1);
    strcpy(OBJECTS[index].url, url);
    OBJECTS[index].size = size;
    OBJECTS[index].lru = 0;
    strncpy(CACHE[index], OBJECT, size);
}

void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg)
{
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Proxy Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor="
                 "ffffff"
                 ">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Proxy Lab</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}

void parse_url(char const *url, char *hostname, char *port, char *other)
{
    char *ptr1 = strstr(url, "http://");
    if (ptr1)
        ptr1 += strlen("http://");
    else
        ptr1 = url;
    char *ptr2 = index(ptr1, '/');
    if (ptr2)
    {
        strncpy(hostname, ptr1, ptr2 - ptr1);
        strcpy(other, ptr2);
    }
    else
    {
        strcpy(hostname, ptr1);
        strcpy(other, "/");
    }
    ptr1 = index(hostname, ':');
    if (ptr1)
    {
        strcpy(port, ptr1 + 1);
        *ptr1 = '\0';
    }
    else
    {
        strcpy(port, "80");
    }
}