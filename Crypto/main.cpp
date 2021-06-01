#include "hpp/RSA.hpp"

int main() 
{
        public_key pub;
        private_key priv;

        RSA_gen_keys(&pub, &priv);
        // std::string msg = "Hello, RSA";
        char mesg[] = "111";
        std::cout << "Сообщение до шифрования: 111\n";
        std::vector<long long> encrypted = RSA_encrypt(&pub, mesg);
        
        std::cout << "Сообщение после шифрования\n";
        for (auto element: encrypted)
        {
                std::cout << element;
        }
        std::cout << std::endl;

        std::vector<char> decrypted = RSA_decrypt(&priv, encrypted);
        std::cout << "Сообщение после шифрования: ";
        for (char element: decrypted)
                std::cout << element;
        std::cout << std::endl;
        return 0;
}