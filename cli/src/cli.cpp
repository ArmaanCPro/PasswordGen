#include <iostream>
#include "Generator.h"

int main()
{
    Generator::PasswordPolicy policy;
    Generator::PasswordGenerator pwdGen(policy);
    std::cout << "Password length?\n";
    std::cin >> policy.passwordLength;

    char buf;
    std::cout << "Use symbols? y/n\n";
    std::cin >> buf;
    policy.requireSymbols = buf == 'y';
    std::cout << "Use numbers? y/n\n";
    std::cin >> buf;
    policy.requireNumbers = buf == 'y';
    std::cout << "Use uppercase? y/n\n";
    std::cin >> buf;
    policy.requireUppercase = buf == 'y';
    std::cout << "Use lowercase? y/n\n";
    std::cin >> buf;
    policy.requireLowercase = buf == 'y';

    std::cout << "Excluded characters? y/n\n";
    std::cin >> buf;
    if ( buf == 'y')
    {
        std::cout << "Enter excluded characters: \n";
        std::cin >> policy.excludedCharacters;
    }

    pwdGen.SetPolicy(policy);
    std::cout << "Password: " << std::endl;
    std::cout << pwdGen.GenerateAdvancedPassword();
    return 0;
}