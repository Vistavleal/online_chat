#ifndef RSA_HPP
#define RSA_HPP

#include <string>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <string.h>
#include <limits>
#include <limits.h>

/*
        Структура, представляющая  открытый ключ.
*/
struct public_key
{
        long long modules;
        long long exponental;
};

/*
        Структура, представляющая закрытый ключ.
*/
struct private_key
{
        long long modules;
        long long exponental;
};

void RSA_gen_keys(public_key*, private_key*);
std::vector<long long> RSA_encrypt(public_key*, char*);
std::vector<char> RSA_decrypt(private_key*, std::vector<long long>);
long long extented_GDC(long long, long long, int&, int&);
long long gdc(long long, long long);
long long invmod(long long, long long);
unsigned long long degree (int, long long);
int* cmprss(int);
unsigned long long sqr(unsigned long long);

#endif 