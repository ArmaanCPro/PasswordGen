#include <iostream>
#include "Generator.h"

int main()
{
    Generator::PasswordPolicy policy;
    Generator::PasswordGenerator pwdGen(policy);
    bool isRunning = true;

    while (isRunning)
    {
        std::cout << "1. Set length\n2. Set use lowercase\n3. Set use uppercase\n4. Set use numbers"
                     "\n5. Set use symbols\n6. Generate password\n7. Hash password\n8. Exit\n\nEnter your choice: " << std::flush;

        int choice = 0;
        std::cin >> choice;

        std::string password;

        switch (choice)
        {
            case 1:
                std::cout << "Enter length: " << std::endl;
                std::cin >> policy.passwordLength;
                break;
            case 2:
                policy.requireLowercase = !policy.requireLowercase;
                std::cout << "Lowercase: " << policy.requireLowercase << std::endl;
                break;
            case 3:
                policy.requireUppercase = !policy.requireUppercase;
                std::cout << "Uppercase: " << policy.requireUppercase << std::endl;
                break;
            case 4:
                policy.requireNumbers = !policy.requireNumbers;
                std::cout << "Numbers: " << policy.requireNumbers << std::endl;
                break;
            case 5:
                policy.requireSymbols = !policy.requireSymbols;
                std::cout << "Symbols: " << policy.requireSymbols << std::endl;
                break;
            case 6:
                password = pwdGen.GenerateAdvancedPassword();
                std::cout << password << std::endl;
                break;
            case 7:
                if (password.empty())
                {
                    std::cout << "No password is set, enter a password: " << std::endl;
                    std::cin >> password;
                }
                std::cout << pwdGen.HashPasswordSafe(std::move(password)) << std::endl;
                break;
            case 8:
                isRunning = false;
                break;

            default:
                std::cout << "Invalid choice" << std::endl;
        }
    }
    return 0;
}