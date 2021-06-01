#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include <string>
#include <iostream>
#include "../Crypto/hpp/RSA.hpp"

using namespace std;


int main(int argc, char* argv[]) {
    int server;
    struct sockaddr_in addr;

    // Создание сокета клиента 
    server = socket(AF_INET, SOCK_STREAM, 0);
    if(server < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2])); // или любой другой порт...
    //addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    if(connect(server, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(2);
    }

    // Распаралеливаем приём и отправку сообщений серверу
    int result = fork();
    if (result < 0) {
        cout << "Failed to fork\n";
        exit(1);
     }else if (result > 0) {
        // приём сообщений
        char respone_buf [1044] ;
        while (true) {
            recv(server, respone_buf, 1044, 0);
            if (strcmp(respone_buf, "$(off)")  == 0){
                return 0;
            }
            cout <<  respone_buf << endl;
            memset(respone_buf, 0, 1044);
        }
    } else {
        // отправка сообщений
        string temp_str;
        while(true) {
            getline(cin, temp_str);
            char* chat_buff = new char [temp_str.length()]; 
            for (int i = 0; i < temp_str.length(); i++) {
                chat_buff[i] = temp_str.at(i);
            }

            if (strcmp(chat_buff, "$(online)") == 0 )
                send(server, chat_buff, strlen(chat_buff), 0);
            else if (strcmp(chat_buff, "$(off)") == 0) {
                send(server, chat_buff, strlen(chat_buff), 0);
                close(server);
                return 0;
            } else {
                send(server, chat_buff, strlen(chat_buff), 0);
            }
        }
    }
    close(server);
    return 0;
}
