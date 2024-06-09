#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define WINDOW_SIZE 5
#define PACKET_SIZE 1024

struct packet
{
    int seq_num;
    int data_size;
    char data[PACKET_SIZE];
};

void die(char *msg)
{
    perror(msg);
    exit(1);
}

int main()
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct packet send_packet, recv_packet;
    socklen_t serv_len = sizeof(serv_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        die("Error opening socket");

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12345);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    FILE *file = fopen("archivo.txt", "rb");
    if (file == NULL)
        die("Error opening file");

    int base = 0, nextseqnum = 0;
    while (1)
    {
        if (nextseqnum < base + WINDOW_SIZE && !feof(file))
        {
            send_packet.seq_num = nextseqnum;
            send_packet.data_size = fread(send_packet.data, 1, PACKET_SIZE, file);
            if (send_packet.data_size < 0)
                die("Error reading from file");

            sendto(sockfd, &send_packet, sizeof(send_packet), 0, (struct sockaddr *)&serv_addr, serv_len);
            printf("Sent packet with seq num: %d, data size: %d\n", nextseqnum, send_packet.data_size);

            if (base == nextseqnum)
            {
                alarm(2); // Set a timer for ACK
            }

            nextseqnum++;
        }

        // Receive ACK
        n = recvfrom(sockfd, &recv_packet, sizeof(recv_packet), 0, (struct sockaddr *)&serv_addr, &serv_len);
        if (n < 0)
            die("Error receiving ACK");

        printf("Received ACK for packet with seq num: %d\n", recv_packet.seq_num);

        if (recv_packet.seq_num >= base)
        {
            base = recv_packet.seq_num + 1;
            printf("Updated base to: %d\n", base);
            alarm(0); // Stop the timer
        }

        if (feof(file) && base == nextseqnum)
        {
            break;
        }
    }

    fclose(file);
    close(sockfd);
    return 0;
}
