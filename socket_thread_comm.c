#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define port 8080
#define size 30 
//#define ip 172.20.24.133

int client_fd ;
void *thread_client_fun(void *args);
void read_func();

int main()
{

	pthread_t thread_client ;
	printf("hii \n");
	client_fd = socket(AF_INET , SOCK_STREAM , 0);
	if(client_fd < 0)
	{
		perror("Error While Creating Socket ..\n");
		_exit(3);
	}

	struct sockaddr_in clientaddr ;
	clientaddr.sin_family = AF_INET ;
	clientaddr.sin_port = htons(port) ;
	clientaddr.sin_addr.s_addr = inet_addr("172.16.0.73") ;

	int client_status = connect(client_fd , (struct sockaddr *)&clientaddr , sizeof(clientaddr));

	if(client_status < 0)
	{
		perror("Error While Connecting ...\n");
		_exit(3);
	}

	int thread_status = pthread_create(&thread_client , NULL , thread_client_fun , NULL);
	if(thread_status == -1)
	{

		perror("Error While Creating Thread \n");
		_exit(3);
	}

	pthread_join(thread_client , NULL);
	close(client_fd);
	return 0;

}

void *thread_client_fun(void *args)
{
    int array_buffer[size - 20];
    char *client_buffer = (char *)malloc(size * sizeof(char));
    if (client_buffer == NULL)
    {
        perror("Error While Allocating Memory\n");
        _exit(3);
    }

    for (int i = 0; i < size - 20; i++)
    {
        printf("Enter The Array [%d] is : ", i);
        scanf("%d", &array_buffer[i]);
    }

    int buffer_index = 0;
    for (int i = 0; i < size - 20; i++)
    {
        char buffer[6];
        snprintf(buffer, sizeof(buffer), "%d", array_buffer[i]);
        int len = strlen(buffer);

        if (buffer_index + len + 1 < size)
        {
            strncpy(&client_buffer[buffer_index], buffer, len);
            buffer_index += len;
            client_buffer[buffer_index] = ' ';
            buffer_index++;
        }
    }

    if (buffer_index > 0)
    {
        client_buffer[buffer_index - 1] = '\0'; 
    }
    else
    {
        client_buffer[0] = '\0'; 
    }

    write(client_fd, client_buffer, buffer_index);
    printf("Sent\n");


    read_func();
    
    free(client_buffer);
    client_buffer = NULL;


    return NULL;
}

void read_func()
{
    char *server_buffer = (char *)malloc(size * sizeof(char));
    if (server_buffer == NULL)
    {
        perror("Error While Allocating Memory \n");
        _exit(3);
    }
    size_t read_bytes = read(client_fd, server_buffer, size - 1);
    if (read_bytes < 0)
    {
        perror("Error While Reading Data \n");
        free(server_buffer);
        _exit(3);
    }
    server_buffer[read_bytes] = '\0';

    int server_buf[size];
    int k = 0;

    for (int i = 0; i < read_bytes;)
    {
        int data;
        int consumed_chars = 0;

        if (sscanf(&server_buffer[i], "%d%n", &data, &consumed_chars) == 1)
        {
            server_buf[k++] = data;
            i += consumed_chars;
        }
        else
        {
            i++; 
        }
    }

    printf("The Received Array = ");
    for (int i = 0; i < k; i++)
    {
        printf("%d ", server_buf[i]);
    }
    printf("\n");

    
    free(server_buffer);
    server_buffer = NULL;

}

