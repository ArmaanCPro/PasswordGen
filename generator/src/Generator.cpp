#include "Generator.h"

#include <sodium.h>
#include <tuple>
#include <tuple>
#include <tuple>
#include <tuple>

std::string Generator::PasswordGenerator::GenerateSimplePassword(bool intelligible) const
{
    std::string password;

    std::random_device rd;
    auto rng = std::mt19937_64(rd());

    if (intelligible)
    {
        std::uniform_int_distribution<int> dist('a', 'a' + 25); // ASCII letters range

        for (uint64_t i = 0; i < policy.passwordLength; i++)
        {
            char c = (char)dist(rng);
            while (policy.excludedCharacters.find(c) != std::string::npos)
                c = (char)dist(rng);

            password.push_back(c);
        }
    }
    else
    {
        std::uniform_int_distribution<int> dist(33, 126); // ASCII full range

        for ( int i = 0; i < policy.passwordLength; i++)
        {
            char c = (char)dist(rng);
            while (policy.excludedCharacters.find(c) != std::string::npos)
                c = (char)dist(rng);

            password.push_back(c);
        }
    }

    return password;
}

std::string Generator::PasswordGenerator::GenerateIntermediatePassword() const {
    std::string password;

    std::random_device rd;
    auto rng = std::mt19937_64(rd());

    std::uniform_int_distribution<short> groupDist(0, 3); // we have 4 categories, lowercase, uppercase, numbers, and symbols

    for (uint64_t i = 0; i < policy.passwordLength; i++)
    {
        short charGroup = groupDist(rng);

        // TODO: bad code. please fix in future
        while (charGroup == 0 && !policy.requireLowercase)
            charGroup = groupDist(rng);
        while (charGroup == 1 && !policy.requireUppercase)
            charGroup = groupDist(rng);
        while (charGroup == 2 && !policy.requireNumbers)
            charGroup = groupDist(rng);
        while (charGroup == 3 && !policy.requireSymbols)
            charGroup = groupDist(rng);


        switch (charGroup)
        {
            case 0: {
                std::uniform_int_distribution<size_t> dist(0, s_LowerCaseChars.length() - 1);
                password.push_back(s_LowerCaseChars[dist(rng)]);
                break;
            }
            case 1: {
                std::uniform_int_distribution<size_t> dist(0, s_UpperCaseChars.length() - 1);
                password.push_back(s_UpperCaseChars[dist(rng)]);
                break;
            }
            case 2: {
                std::uniform_int_distribution<size_t> dist(0, s_NumbersChars.length() - 1);
                password.push_back(s_NumbersChars[dist(rng)]);
                break;
            }
            case 3: {
                std::uniform_int_distribution<size_t> dist(0, s_SymbolsChars.length() - 1);
                password.push_back(s_SymbolsChars[dist(rng)]);
                break;
            }
            default:
                std::cerr << "invalid char group";
        }
    }

    // compiler does Return Value Optimization automatically, no need for std::move
    return password;
}

std::tuple<std::string, std::string> Generator::PasswordGenerator::GenerateHashedPassword() const
{
    std::string password = GenerateIntermediatePassword();
    char encryptedPw[crypto_pwhash_STRBYTES];
    int hashSuccess = crypto_pwhash_str(encryptedPw, password.c_str(), password.length(),
        sodiumOpsLimitFromEncryptionStrength(policy.encryptionStrength), sodiumMemLimitFromEncryptionStrength(policy.encryptionStrength));
    if (hashSuccess == -1)
        throw std::runtime_error("Failed to encrypt password");
    std::string encryptedPassword(encryptedPw);
    delete[] encryptedPw;
    return encryptedPassword;
}

std::tuple<std::string, std::string> Generator::PasswordGenerator::HashPassword(const std::string& password) const
{
    char encryptedPw[crypto_pwhash_STRBYTES];
    int hashSuccess = crypto_pwhash_str(encryptedPw, password.c_str(), password.length(),
        sodiumOpsLimitFromEncryptionStrength(policy.encryptionStrength), sodiumMemLimitFromEncryptionStrength(policy.encryptionStrength));
    if (hashSuccess == -1)
        throw std::runtime_error("Failed to encrypt password");

    crypto_pwhash_str_verify(encryptedPw, password.c_str(), password.length());

    std::string encryptedPassword(encryptedPw);
    delete[] encryptedPw;
    return encryptedPassword;
}