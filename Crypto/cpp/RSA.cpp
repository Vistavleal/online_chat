#include "../hpp/RSA.hpp"

void RSA_gen_keys(public_key* pub, private_key* priv)
{
        long long p, q;
        long long n, phi;
        //std::ifstream primes("primes.txt");
        if (true)
        {
                // Считываем все простые числа из файла
                //std::string line;
                //std::vector<int> primes_list;
                //while(std::getline(primes, line))
                 //       primes_list.push_back(std::atoi(line.c_str()));

                // Выбираем случайные простые числа из последовательности
                p = 3;
                q = 7;
                //  Вычисляем их произведение и значение функции Эйлера
                n = p*q;
                phi = (p-1)*(q-1);
                // Подбор открытой экспоненты
                long long e = 5;
                // Вычисление закрытой экспоненты
                int x, y;
                long long d = 5; //invmod(e, phi);

                // Сохранение открытого ключа
                priv->modules = n;
                priv->exponental = d;

                // Сохранение закрытого ключа 
                pub->modules = n;
                pub->exponental = e;

                //primes.close();
        }
}

/*
        Шифрование при помощи публичного ключа.
        Смысл в том, чтобы каждый символ перевести в ASCII-код и зашифровать
        следующим образом: pow(message[i], e) mod n (pow(х, у) - возведение х в степень у)
        Функция возвращает зашифрованные элементы сообщения
*/
std::vector<long long> RSA_encrypt(public_key* pub, char* msg)
{
        std::vector<long long> temp;
        for (int i = 0; i < std::strlen(msg); i++)
        {
                //long long t = degree(int(msg[i]), pub->exponental);
                //temp.push_back( t % pub->modules);
                int* tmp = cmprss(int(msg[i]));
                long long t = degree(tmp[0], pub->exponental);
                temp.push_back(t % pub->modules);
                temp.push_back(tmp[1]);
        }
        return temp;
}

/*
        Дешифрование аналогично шифрованию лишь с тем различием, что
        каждый элемент вектора (который представляет зашифрованный символ сообщения)
        дешифруется следующим образом: pow(encrypted[i], d) mod n и записывается в
        строку по ASCII-коду.
        Ничего не возвращает, только изменяет переданную строку.
*/
std::vector<char> RSA_decrypt(private_key* key, char* message, std::vector<long long> encrypted)
{
        std::vector<long long> temp;
        for (int i = 0; i < encrypted.size(); i+=2)
        {
                int shrink_times = encrypted[i+1]; 
                unsigned long long t = degree(encrypted[i], key->exponental);
                unsigned long long t1 = t % key->modules;
                temp.push_back(t1 * shrink_times);
        }
        // Проход по всему вектору temp
        std::vector<char> msg;
        for (int i = 0; i < temp.size(); i++)
        {
                // Преведение ASCII-кода к char 
                msg.push_back(char(temp[i]));
        }
        return msg;
}

/*
        Реализация расширенного алгоритма Евклида
        x и y - это коэффициенты Безу
        по другому это можно записать, как
        НОД(a, b) = x*a - y*b.
        Возвращает НОД(а, b) и изменяет x и y.
 */
long long extented_GDC(long long a, long long b, int &x, int &y) 
{
        if (a == 0) 
        {
                x = 0;
                y = 1;
                return b;
        }
        int x1, y1;
        long long d = extented_GDC(b % a, a, x1, y1);
        x = y1 - (b / a) * x1;
        y = x1;
        return d;
}

// Нахождения НОД'a
long long gdc(long long a, long long b)
{
        return b==0 ? a : gdc(b, a%b);
}

// Возведение а в степень b
unsigned long long degree(int a, long long b)
{
        unsigned long long temp = a;
        for (int i = 0; i < b-1; i++)
                temp *= a;
        return temp; 
}

// Получение обратного по модулю числа
long long invmod(long long a, long long m) {
    int x, y;
    extented_GDC(a, m, x, y);
    x = (x % m + m) % m;
    return x;
}

/* 
        Сжатие числа. На вход поступают только целые числа.
        Возвращает сжатое число a и коэф. сжатия
*/
int* cmprss (int a)
{
        int* temp = new int[2];
        for (int i = a-1; i >1; i--)
                if (a % i == 0)
                {
                        temp[0] = a / i;
                        temp[1] = i;
                        return temp;
                }
}

// Быстрое возведение в степень
unsigned long long binpow(unsigned long long a, unsigned long long e, unsigned long long mod = LLONG_MAX)
{
    return e == 0 ? 1 : (e & 1U ? a * binpow(a, e - 1, mod) % mod : sqr(binpow(a, e / 2, mod)) % mod);
}

// Тут нужны комментарии???
unsigned long long sqr(unsigned long long x)
{
    return x * x;
}
