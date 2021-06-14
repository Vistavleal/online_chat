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
#include <chrono>
#include "../Crypto/hpp/RSA.hpp"

using namespace std;


int main(int argc, char* argv[]) {
    int server;
    struct sockaddr_in addr;

    // Генерация открытого и закрытого ключа
    public_key pub;
    private_key priv;
    RSA_gen_keys(&pub, &priv);

    // Создание сокета клиента 
    server = socket(AF_INET, SOCK_STREAM, 0);
    if(server < 0) {
        perror("socket");
        exit(1);
    }

    // Добавление данных сервера
    addr.sin_family = AF_INET;
   // addr.sin_port = htons(atoi(argv[2])); // или любой другой порт...
    addr.sin_port = htons(atoi(argv[2])); // или любой другой порт...
    // addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Подключение к серверу
    if(connect(server, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(2);
    }
    int exp[2] = {5, 21};
    send(server, exp, sizeof(exp), 0);


    // Распаралеливаем приём и отправку сообщений серверу
    int result = fork();
    if (result < 0) {
        cout << "Failed to fork\n";
        exit(1);
    } else if (result > 0) 
    {
        // приём сообщений
        char respone_buf [1044] ;
        while (true) {
            recv(server, respone_buf, 1044, 0);
            if (strcmp(respone_buf, "$(off)")  == 0){
                close(server);
                return 0;
            }
            else if(
                respone_buf[0] == '$' && 
                respone_buf[1] == '(' &&
                respone_buf[2] == 'S' &&
                respone_buf[3] == ')'
            )
            {
                string response, buffer= respone_buf;
                string ch = "";
                buffer.erase(buffer.begin() + 0);
                buffer.erase(buffer.begin() + 0);
                buffer.erase(buffer.begin() + 0);
                buffer.erase(buffer.begin() + 0);
                buffer.erase(buffer.begin() + 0);

                int arr_size = 0;
                string temp;

                // Вытаскиваем из строки размер массива
                // UPD: только потом заметил, что он даже и не нужен =)
                while (ch != ";")
                {
                    ch = buffer.at(0);
                    if (ch != ";")
                    {
                        temp += ch;
                        buffer.erase(buffer.begin() + 0);
                    } else 
                    {
                        arr_size = atoi(temp.c_str());
                        temp.erase();
                        ch.erase();
                        buffer.erase(buffer.begin() + 0);
                        break;
                    }
                }
                    
                // Вытаскиваем зашифрованное сообщение и переводим его в числа
                vector<long long> decrypt;
                while (ch != ";")
                {
                    // Суть в том, что по сообщению проходимся по символьно и каждый символ проверяем
                    ch = buffer.at(0);
                    if (ch == ";")
                        break;
                        
                    if (ch != ".")
                    {
                        temp += buffer.at(0);
                        buffer.erase(buffer.begin()+0);
                    } else 
                    {
                        buffer.erase(buffer.begin() + 0);
                        decrypt.push_back(atoi(temp.c_str()));
                        temp.erase();
                    }
                }                    

                // Дешифровка 
                vector<char> decrypted = RSA_decrypt(&priv, decrypt);

                // Вывод
                buffer.erase(buffer.begin() + 0);
                cout << "From " << buffer << ": ";
                for (auto elem: decrypted)
                    cout << elem;
                cout << endl;
            }
            else
                cout <<  respone_buf << endl;
            memset(respone_buf, 0, 1044);
        }
    } else {
        // отправка сообщений
        string temp_str;
        while(true)
        {
            getline(cin, temp_str);
            char* chat_buff = new char [temp_str.length()]; 
            for (int i = 0; i <  temp_str.length(); i++)
            {
                chat_buff[i] = temp_str.at(i);
            }

            if (strcmp(chat_buff, "$(online)") == 0 )
                send(server, chat_buff, strlen(chat_buff), 0);
            else if (strcmp(chat_buff, "$(off)") == 0) 
            {
                send(server, chat_buff, strlen(chat_buff), 0);
                close(server);
                return 0;
            }
            else if (chat_buff[0] == '>')
            {
                /*
                    Весь прикол в том, что всю нужную для шифрования/расшифрования
                    сообщения закидываем в одну строку и её отправляем, чтобы не было
                    проблем с синхронизацией клиента-сервера-клиента.
                */
                char secret [strlen(chat_buff)-7];
                char name [5];

                for (int i = 0; i < strlen(chat_buff)-6; i++)
                    secret[i] = chat_buff[i+7];
                
                for (int i = 0; i < 5; i++)
                    name[i] = chat_buff[i+1];

                vector<long long> vec_encrypted = RSA_encrypt(&pub, secret);
                int encrypted[vec_encrypted.size()];
                for (int i = 0; i < vec_encrypted.size(); i++)
                    encrypted[i] = vec_encrypted.at(i);


                string temp;
                temp += "$(S);";

                int array_size = vec_encrypted.size();
                temp += to_string(array_size);
                temp += ";";
                for (int i = 0; i < vec_encrypted.size(); i++)
                {
                    temp += to_string(encrypted[i]);
                    temp +=".";
                }
                temp += ";";
                for (int i = 0; i < 5; i++)
                    temp += name[i];
                send(server, temp.c_str(), temp.length(), 0);
            }
            else
                send(server, chat_buff, sizeof(chat_buff), 0);
        }
    }
    close(server);
    return 0;
}
