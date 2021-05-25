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
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

const int BUFF_SIZE = 1024;
const int PORT = 1236;
int counter = 0;

struct Socket {
   char name[20];
    int client;
    sockaddr_in adress;
};

int new_user_chatting(vector<Socket> &, int);

int main() {
    // Сокет и его дескриптор
    int listener;
    // Структура с адресом
    struct sockaddr_in server_address;
    char buf[1024];
    int bytes_read;

    //  Получение дескриптора сокета
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0) {
        perror("socket");
        exit(1);
    }
    
    server_address.sin_family = AF_INET; // Задание домена
    server_address.sin_port = htons(PORT); // Задание порта
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // IP -адрес хоста

    // Именование сокета (связывание его с адресом хоста)
    if(bind(listener, (struct sockaddr *)&server_address, sizeof(server_address))< 0) {
        perror("bind");
        exit(2);
    }
    // Количество возможных подключений к серверу
    listen(listener, 2);
    cout << "Server started at port: " << server_address.sin_port << endl
        << "with ip " << server_address.sin_addr.s_addr << endl << endl;

    vector<Socket> data;
    socklen_t size = sizeof(data[0].adress);

    while(true) {
        Socket temp;
        data.push_back(temp);
        data[counter].client = accept(listener, reinterpret_cast<struct sockaddr*>(&data[counter].adress), &size); 
        if(!data[counter].client) {
            perror("accept");
            exit(3);
        } else {
            switch (fork())
            {
            case -1:
                send(data[counter].client, "Failed to connect =)\n", sizeof("Failed to connect =)\n"), 0);
                close(data[counter].client);
                break;

            // Потомок
            case 0:
                new_user_chatting(data, counter);
                break;
            
            default:
                counter += 1;
                break;
            }
         }
    }
    return 0;
 }

int new_user_chatting(vector<Socket> &data, int count) {
        cout << "IP: " << data[count].adress.sin_addr.s_addr << endl << endl;
        char buf[1024];
        send(data[count].client, "Введите имя пользователя", sizeof("Введите имя пользователя"), 0);
        recv(data[count].client, data[count].name, 20, 0);
        strcpy(buf, "Добро пожаловать в чат, ");
        strcat(buf, data[count].name);
        send(data[count].client, buf, strlen(buf), 0);
        memset(buf, 0, 1024);

        int bytes_read;
        while(true) {
            bytes_read = recv(data[count].client, buf, 1024, 0);
            if(bytes_read <= 0) break;
            char* temp_buf = new char[1044];
            strcpy(temp_buf, data[count].name);
            strcat(temp_buf, ": ");
            strcat(temp_buf, buf);

            if(strcmp(buf, "$(online)") == 0){
                send(data[count].client, "Now online", sizeof("Now online"), 0);
            } else if(strcmp(buf, "$(off)") == 0){
                cout << "User with ip: " << data[count].adress.sin_addr.s_addr << " disconected\n";
                counter -= 1;
                send(data[count].client, "$(off)", sizeof("$(off)"), 0);
                close(data[count].client);
                data.erase(data.begin() + count);
            } else {
                for (int i = 0; i < data.size() -1; i++) {
                    if (i != count)
                        send(data[i].client, temp_buf, strlen(temp_buf), 0);
                }
            }
            memset(temp_buf, 0, 1044);
            memset(buf, 0, 1024);
        }
        return 0;
}
