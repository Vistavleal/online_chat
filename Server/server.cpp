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
#include <mutex>

using namespace std;

const int BUFF_SIZE = 1024;
const int PORT = 5634;
int counter = 0;

struct Socket
{
   char name[20];
    int client;
    sockaddr_in adress;
};

int new_user_chatting(vector<Socket> &, int);
mutex mtx;

int main() 
{
    // Сокет и его дескриптор
    int listener;
    // Структура с адресом
    struct sockaddr_in server_address;
    char buf[1024];
    int bytes_read;

    //  Получение дескриптора сокета
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }
    
    server_address.sin_family = AF_INET; // Задание домена
    server_address.sin_port = htons(PORT); // Задание порта
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // IP -адрес хоста

    // Именование сокета (связывание его с адресом хоста)
    if(bind(listener, (struct sockaddr *)&server_address, sizeof(server_address))< 0) 
    {
        perror("bind");
        exit(2);
    }
    cout << "Server started at port: " << server_address.sin_port << endl
        << "with ip " << server_address.sin_addr.s_addr << endl << endl;

    //  Устанавливаем максимальное количество подключений к серверу
    listen(listener, 3);

    vector<Socket> data;
    socklen_t size = sizeof(data[0].adress);
    vector<thread> threads;

    while(true) {
        Socket temp;
        data.push_back(temp);
        data[counter].client = accept(listener, reinterpret_cast<struct sockaddr*>(&data[counter].adress), &size); 
        if(!data[counter].client) {
            perror("accept");
            exit(3);
        } else {
            threads.push_back(thread(new_user_chatting, ref(data), counter));
            counter++;
        }
    }
    for (auto& thread : threads) {
        thread.join();
}
    return 0;
 }

int new_user_chatting(vector<Socket> &data, int count) {
        mtx.unlock();
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

            string send_buf;
            send_buf += data[count].name;
            send_buf += ": ";
            send_buf += buf;

            if(strcmp(buf, "$(online)") == 0){
                string temp;
                temp += "Now online: ";
                for (Socket d: data){
                    temp += d.name;
                    temp += "\n";
                } 
                send(data[count].client, temp.c_str(), sizeof(temp), 0);
            } else if(strcmp(buf, "$(off)") == 0){
                cout << "User with ip: " << data[count].adress.sin_addr.s_addr << " disconected\n";
                counter -= 1;
                send(data[count].client, "$(off)", sizeof("$(off)"), 0);
                close(data[count].client);
                data.erase(data.begin() + count);

                // Проверка на онлайн одного юзера
            } else if (buf[0] == '?' && (strlen(buf) >= 5)) {
                string name;
                for (int i = 1; i < strlen(buf);  i++)
                    name += buf[i];
                for (int i = 0; i < data.size(); i++) {
                    if (data[i].name == name){
                        name += " is online now\n";
                        send(data[count].client, name.c_str(), name.length(), 0);
                    }
                }

                // Личное сообщение
            } else if(buf[0]== '>' && strlen(buf) >= 5  ) {
                string name, buff;
                for (int i = 1; i < 6; i++) 
                    name += buf[i];
                
                buff += "Secret chat with ";
                buff += data[count].name;
                buff += ": ";
                for (int i = 7; i < strlen(buf); i++)
                    buff += buf[i];
                    // FIXME
                bool is_send = false;
                for (int i = 0; i < data.size() - 1; i++) {
                    if (data[i].name == name){
                        send(data[i].client, buff.c_str(), buff.length(), 0);
                        is_send = true;
                    }
                }
                if (!is_send) 
                {
                    send(data[count].client, "No such user", sizeof("No such user"), 0);
                }
            } else {
                for (int i = 0; i < data.size() - 1; i++) {
                    if (i != count)
                        send(data[i].client, send_buf.c_str(), send_buf.length(), 0);
                }
            }
            //memset(temp_buf, 0, 1044);
            send_buf.erase();
            memset(buf, 0, 1024);
        }
        return 0;
}
