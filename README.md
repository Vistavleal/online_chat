# online_chat
tcp/ip онлайн чат с реализацией шифрования RSA.

Для запуска необходимо скомпилировать исходники сервера и клиента.

```
g++ -o server server.cpp ../Crypto/cpp/RSA.cpp -pthread
g++ -o client client.cpp ../Crypto/cpp/RSA.cpp
```
После достаточно запустить сервер
```
./server
```
и клиент
```
./client <ip> <port>
```
где ``<ip>`` - ip адрес сервера, ``<port>`` - порт на котором работает сервер. По стандарту это 5633.

