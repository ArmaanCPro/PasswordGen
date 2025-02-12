#include "Generator.h"

#include <sodium.h>
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

        // this is bad code. in fact, this whole function sucks. but I'm not going to refactor it because GenerateAdvancedPassword() exists
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
                char c = s_LowerCaseChars[dist(rng)];
                while (policy.excludedCharacters.find(c) != std::string::npos)
                    c = s_LowerCaseChars[dist(rng)];
                password.push_back(s_LowerCaseChars[c]);
                break;
            }
            case 1: {
                std::uniform_int_distribution<size_t> dist(0, s_UpperCaseChars.length() - 1);
                char c = s_UpperCaseChars[dist(rng)];
                while (policy.excludedCharacters.find(c) != std::string::npos)
                    c = s_UpperCaseChars[dist(rng)];
                password.push_back(s_UpperCaseChars[c]);
                break;
            }
            case 2: {
                std::uniform_int_distribution<size_t> dist(0, s_NumbersChars.length() - 1);
                char c = s_NumbersChars[dist(rng)];
                while (policy.excludedCharacters.find(c) != std::string::npos)
                    c = s_NumbersChars[dist(rng)];
                password.push_back(s_NumbersChars[c]);
                break;
            }
            case 3: {
                std::uniform_int_distribution<size_t> dist(0, s_SymbolsChars.length() - 1);
                char c = s_SymbolsChars[dist(rng)];
                while (policy.excludedCharacters.find(c) != std::string::npos)
                    c = s_SymbolsChars[dist(rng)];
                password.push_back(s_SymbolsChars[c]);
                break;
            }
            default:
                std::cerr << "invalid char group";
        }
    }

    // compiler does Return Value Optimization automatically, no need for std::move
    return password;
}

std::future<std::vector<std::string>> Generator::PasswordGenerator::GenerateIntermediatePasswordsAsync(
    int numPasswords) const
{
    return std::async(std::launch::async, &Generator::PasswordGenerator::GenerateIntermediatePasswords, this, numPasswords);
}

std::string Generator::PasswordGenerator::GenerateAdvancedPassword() const
{
    std::string password;
    std::vector<char> availableChars;

    // Build a pool of characters based on the policy
    if (policy.requireLowercase)
    {
        availableChars.insert(availableChars.end(), s_LowerCaseChars.begin(), s_LowerCaseChars.end());
    }
    if (policy.requireUppercase)
    {
        availableChars.insert(availableChars.end(), s_UpperCaseChars.begin(), s_UpperCaseChars.end());
    }
    if (policy.requireNumbers)
    {
        availableChars.insert(availableChars.end(), s_NumbersChars.begin(), s_NumbersChars.end());
    }
    if (policy.requireSymbols)
    {
        availableChars.insert(availableChars.end(), s_SymbolsChars.begin(), s_SymbolsChars.end());
    }

    // Remove excluded characters
    std::erase_if(availableChars, [this](const char& c) {
                      return policy.excludedCharacters.find(c) != std::string::npos;
                  });

    if (availableChars.empty()) {
        throw std::runtime_error("No valid characters available for password generation");
    }

    // Generate password respecting the required length
    for (uint64_t i = 0; i < policy.passwordLength; i++)
    {
        unsigned char randomIndex;
        randombytes_buf(&randomIndex, sizeof(randomIndex)); // Generate a random index
        randomIndex %= availableChars.size();                   // Map to available characters
        password += availableChars[randomIndex];
    }

    return password;
}

std::future<std::vector<std::string>> Generator::PasswordGenerator::GenerateAdvancedPasswordsAsync(
    int numPasswords) const
{
    return std::async(std::launch::async, &Generator::PasswordGenerator::GenerateAdvancedPasswords, this, numPasswords);
}

std::tuple<std::string, std::string> Generator::PasswordGenerator::GenerateHashedPassword() const
{
    std::string password = GenerateAdvancedPassword();
    char encryptedPw[crypto_pwhash_STRBYTES];
    const int hashSuccess = crypto_pwhash_str(encryptedPw, password.c_str(), password.length(),
        sodiumOpsLimitFromEncryptionStrength(policy.encryptionStrength), sodiumMemLimitFromEncryptionStrength(policy.encryptionStrength));
    if (hashSuccess == -1)
        throw std::runtime_error("Failed to encrypt password");
    std::string encryptedPassword(encryptedPw);

    return std::make_tuple(std::move(password), std::move(encryptedPassword));
}

std::string Generator::PasswordGenerator::HashPassword(const std::string& password) const
{
    char encryptedPw[crypto_pwhash_STRBYTES];
    const int hashSuccess = crypto_pwhash_str(encryptedPw, password.c_str(), password.length(),
        sodiumOpsLimitFromEncryptionStrength(policy.encryptionStrength), sodiumMemLimitFromEncryptionStrength(policy.encryptionStrength));
    if (hashSuccess == -1)
        throw std::runtime_error("Failed to encrypt password");

    std::string encryptedPassword(encryptedPw);
    return encryptedPassword;
}

std::string Generator::PasswordGenerator::HashPasswordSafe(std::string password) const
{
    // Safeguard: Ensure password is non-empty
    if (password.empty())
    {
        throw std::invalid_argument("Password cannot be empty");
    }

    sodium_mlock(&password[0], password.length());
    std::string hashedPassword = HashPassword(password);
    sodium_munlock(&password[0], password.length());
    return hashedPassword;
}

std::future<std::vector<std::string>> Generator::PasswordGenerator::HashPasswordsSafeAsync(
    std::vector<std::string> passwords) const
{
    return std::async(std::launch::async, &Generator::PasswordGenerator::HashPasswordsSafe, this, std::move(passwords));
}

bool Generator::PasswordGenerator::VerifyPassword(const std::string& password, const std::string& hash) const // NOLINT(*-convert-member-functions-to-static)
{
    return crypto_pwhash_str_verify(hash.c_str(), password.c_str(), password.length()) == 0;
}

bool Generator::PasswordGenerator::VerifyPasswordSafe(std::string password, const std::string& hash) const
{
    // Safeguard: Ensure password is non-empty
    if (password.empty())
    {
        throw std::invalid_argument("Password cannot be empty");
    }

    sodium_mlock(&password[0], password.length());
    bool result = VerifyPassword(password, hash);
    sodium_munlock(&password[0], password.length());
    return result;
}