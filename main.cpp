#include <iostream>
#include <string>

#include <random>

int main()
{
    int passwordLen = 0;
    std::cout << "Enter password length:\n";
    std::cin >> passwordLen;
    std::cout << std::endl;

    std::string password;

    std::cout << "intelligible? y/n ";
    bool intelligible = false;
    {
        char result;
        std::cin >> result;
        switch (result)
        {
            case 'y':
                intelligible = true;
            break;
            case 'n':
                intelligible = false;
            break;
            default:
                std::cout << "Invalid input. Defaulting to intelligible = false" << std::endl;
            intelligible = false;
        }
    }
    std::cout << std::endl;

    std::random_device rd;
    std::mt19937_64 rng = std::mt19937_64(rd());


    if (intelligible)
    {
        std::uniform_int_distribution<int> dist('a', 'a' + 25); // ASCII letters range

        for ( int i = 0; i < passwordLen; i++)
        {
            password.push_back((char)dist(rng));
        }
    }
    else
    {
        std::uniform_int_distribution<int> dist(33, 126); // ASCII full range

        for ( int i = 0; i < passwordLen; i++)
        {
            password.push_back((char)dist(rng));
        }
    }

    std::cout << "Password: " << password << std::endl;

    return 0;
}