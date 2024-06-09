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

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s <nombre_del_archivo>\n", argv[0]);
        exit(1);
    }

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct packet send_packet, recv_packet;
    socklen_t serv_len = sizeof(serv_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        die("Error al abrir socket");

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12345);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL)
        die("Error al abrir el archivo");

    // Envia el nombre del archivo primero 
    send_packet.seq_num = 0;
    send_packet.data_size = strlen(argv[1]) + 1;
    strcpy(send_packet.data, argv[1]);

    sendto(sockfd, &send_packet, sizeof(send_packet), 0, (struct sockaddr *)&serv_addr, serv_len);
    printf("Nombre del archivo enviado: %s\n", argv[1]);

    int base = 1, nextseqnum = 1;
    while (1)
    {
        if (nextseqnum < base + WINDOW_SIZE && !feof(file))
        {
            send_packet.seq_num = nextseqnum;
            send_packet.data_size = fread(send_packet.data, 1, PACKET_SIZE, file);
            if (send_packet.data_size < 0)
                die("Error al leer del archivo");

            sendto(sockfd, &send_packet, sizeof(send_packet), 0, (struct sockaddr *)&serv_addr, serv_len);
            printf("Paquete enviado con número de sequencia: %d, tamaño de datos: %d\n", nextseqnum, send_packet.data_size);

            if (base == nextseqnum)
            {
                alarm(2); // Establecer temporizador del ACK
            }

            nextseqnum++;
        }

        // Recibir ACK
        n = recvfrom(sockfd, &recv_packet, sizeof(recv_packet), 0, (struct sockaddr *)&serv_addr, &serv_len);
        if (n < 0)
            die("Error al recibir ACK");

        printf("ACK recibido para el paquete con número de sequencia: %d\n", recv_packet.seq_num);

        if (recv_packet.seq_num >= base)
        {
            base = recv_packet.seq_num + 1;
            printf("Base actualizada a: %d\n", base);
            alarm(0); // Detiene el temporizador
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
