#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define WINDOW_SIZE 5
#define PACKET_SIZE 1024

struct packet {
    int seq_num;
    char data[PACKET_SIZE];
};

void die(char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr, cli_addr;
    struct packet recv_packet, ack_packet;
    socklen_t cli_len = sizeof(cli_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        die("Error opening socket");

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12345);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        die("Error on binding");

    FILE *file = fopen("archivo_recibido.txt", "wb");
    if (file == NULL)
        die("Error opening file for writing");

    int expected_seq_num = 0;
    while (1) {
        n = recvfrom(sockfd, &recv_packet, sizeof(recv_packet), 0, (struct sockaddr *)&cli_addr, &cli_len);
        if (n < 0)
            die("Error receiving packet");

        printf("Received packet with seq num: %d\n", recv_packet.seq_num);

        if (recv_packet.seq_num == expected_seq_num) {
            fwrite(recv_packet.data, 1, n - sizeof(int), file);
            printf("Write packet data to file\n");
            expected_seq_num++;
        }

        // Send ACK
        ack_packet.seq_num = recv_packet.seq_num;
        sendto(sockfd, &ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)&cli_addr, cli_len);
        printf("Sent ACK for packet with seq num: %d\n", recv_packet.seq_num);
    }

    fclose(file);
    close(sockfd);
    return 0;
}
