#include <iostream>
#include <string>

#include <random>
#include <future>

// password generation through uniform int distribution across ASCII
std::string simplePasswordGen(int passwordLen, bool intelligible)
{
    std::string password;

    std::random_device rd;
    auto rng = std::mt19937_64(rd());


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

    return password;
}

std::string intermediatePasswordGen(int passwordLen, const std::string& lowerCaseChars, const std::string& upperCaseChars, const std::string& numbersChars, const std::string& symbolsChars)
{
    static constexpr int charCategories = 4;

    std::string password;

    std::random_device rd;
    auto rng = std::mt19937_64(rd());

    std::uniform_int_distribution<size_t> groupDist(0, charCategories - 1);

    for (int i = 0; i < passwordLen; i++)
    {
        const size_t charGroup = groupDist(rng);
        switch (charGroup)
        {
            case 0: {
                std::uniform_int_distribution<size_t> dist(0, lowerCaseChars.length() - 1);
                password.push_back(lowerCaseChars[dist(rng)]);
                break;
            }
            case 1: {
                std::uniform_int_distribution<size_t> dist(0, upperCaseChars.length() - 1);
                password.push_back(upperCaseChars[dist(rng)]);
                break;
            }
            case 2: {
                std::uniform_int_distribution<size_t> dist(0, numbersChars.length() - 1);
                password.push_back(numbersChars[dist(rng)]);
                break;
            }
            case 3: {
                std::uniform_int_distribution<size_t> dist(0, symbolsChars.length() - 1);
                password.push_back(symbolsChars[dist(rng)]);
                break;
            }
            default:
                std::cerr << "invalid char group";
        }
    }


    // compiler does Return Value Optimization automatically, no need for std::move
    return password;
}

int main()
{
    int passwordLen = 0;
    std::cout << "Enter password length:\n";
    std::cin >> passwordLen;

    std::cout << "intelligible? y/n \n";
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


    std::future<std::string> simplePassword = std::async(std::launch::async, simplePasswordGen, passwordLen, intelligible);
    std::cout << "Simple Password: " << simplePassword.get() << std::endl;

    const std::string lowerCase = "abcdefghijklmnopqrstuvwxyz";
    const std::string upperCase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string numbers = "0123456789";
    const std::string symbols = "!@#$%^&*()_+=-[]{}|;':\",./<>?";

    std::future<std::string> intermediatePassword = std::async(std::launch::async, intermediatePasswordGen, passwordLen, lowerCase, upperCase, numbers, symbols);
    std::cout << "\nIntermediate Password: " << intermediatePassword.get() << std::endl;

    return 0;
}