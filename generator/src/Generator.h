#pragma once

#include <algorithm>
#include <iostream>
#include <string>

#include <random>
#include <future>
#include <utility>

namespace Generator
{
    static const std::string s_LowerCaseChars = "abcdefghijklmnopqrstuvwxyz";
    static const std::string s_UpperCaseChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static const std::string s_NumbersChars = "0123456789";
    static const std::string s_SymbolsChars = "!@#$%^&*()_+=-[]{}|;':\",./<>?";

    struct PasswordPolicy;
    class PasswordGenerator;

    static double CalculatePasswordEntropy(const std::string& password) { return log2((double)password.length());}
}

/// A multitude of parameters to generate passwords using
struct Generator::PasswordPolicy
{
    int passwordLength;
    bool requireLowercase;
    bool requireUppercase;
    bool requireNumbers;
    bool requireSymbols;
    std::string excludedCharacters;
};

/// Class for generating passwords. Only member is a password policy
class Generator::PasswordGenerator
{
public:
    explicit PasswordGenerator(PasswordPolicy policy)
        :
        policy(std::move(policy))
    {}

    /**
     * Updates the current password policy with a new policy definition.
     * @param newPolicy The new password policy to be set, which defines rules for the password generation
     */
    void SetPolicy(const PasswordPolicy& newPolicy) { policy = newPolicy;}

    /**
     * Generates a simple password based on the current policy (only password length is used).
     * @param intelligible A boolean flag to indicate whether the password should consist
     *                     of only intelligible characters (a-z). Default is false.
     * @return A randomly generated password as a string adhering to the current policy.
     */
    [[nodiscard]] std::string GenerateSimplePassword(bool intelligible = false) const
    {
        std::string password;

        std::random_device rd;
        auto rng = std::mt19937_64(rd());

        if (intelligible)
        {
            std::uniform_int_distribution<int> dist('a', 'a' + 25); // ASCII letters range

            for ( int i = 0; i < policy.passwordLength; i++)
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

    /**
     * Generates an intermediate password adhering to the current password policy.
     * Invalid character groups are bypassed dynamically during generation.
     *
     * @return A randomly generated intermediate password as a string.
     */
    [[nodiscard]] std::string GenerateIntermediatePassword() const
    {
        std::string password;

        std::random_device rd;
        auto rng = std::mt19937_64(rd());

        std::uniform_int_distribution<short> groupDist(0, 3); // we have 4 categories, lowercase, uppercase, numbers, and symbols

        for (int i = 0; i < policy.passwordLength; i++)
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

    /// Generates a vector of passwords. Uses GenerateIntermediatePassword()
    [[nodiscard]] std::vector<std::string> GenerateIntermediatePasswords(int numPasswords) const
    {
        std::vector<std::string> passwords;
        std::generate_n(std::back_inserter(passwords), numPasswords, [this]() { return this->GenerateIntermediatePassword(); });
        return passwords;
    }

private:
    PasswordPolicy policy;

};