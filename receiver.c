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
    struct sockaddr_in serv_addr, cli_addr;
    struct packet recv_packet, ack_packet;
    socklen_t cli_len = sizeof(cli_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        die("Error al abrir socket");

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12345);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        die("Error al enlazar");

    // Receive the filename first
    n = recvfrom(sockfd, &recv_packet, sizeof(recv_packet), 0, (struct sockaddr *)&cli_addr, &cli_len);
    if (n < 0)
        die("Error al recibir el nombre del archivo");

    char filename[PACKET_SIZE];
    strcpy(filename, recv_packet.data);
    printf("Nombre del archivo recibido: %s\n", filename);

    FILE *file = fopen(filename, "wb");
    if (file == NULL)
        die("Error al abrir el archivo de escritura");

    int expected_seq_num = 1;
    while (1)
    {
        n = recvfrom(sockfd, &recv_packet, sizeof(recv_packet), 0, (struct sockaddr *)&cli_addr, &cli_len);
        if (n < 0)
            die("Error al recibir el paquete");

        printf("Paquete recibido con número de sequencia: %d, tamaño de datos: %d\n", recv_packet.seq_num, recv_packet.data_size);

        if (recv_packet.seq_num == expected_seq_num)
        {
            fwrite(recv_packet.data, 1, recv_packet.data_size, file);
            printf("Bytes escritos en el archivo: %d\n", recv_packet.data_size);
            expected_seq_num++;
        }

        // Enviar ACK
        ack_packet.seq_num = recv_packet.seq_num;
        sendto(sockfd, &ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)&cli_addr, cli_len);
        printf("ACK enviado para el paquete con número de secuencia: %d\n", recv_packet.seq_num);

        if (recv_packet.data_size < PACKET_SIZE)
        {
            // Si data_size es menor a PACKET_SIZE, es el final del archivo
            break;
        }
    }

    fclose(file);
    close(sockfd);
    return 0;
}
