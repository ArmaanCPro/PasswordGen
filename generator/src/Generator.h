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

    inline double CalculatePasswordEntropy(const std::string& password)
    {
        size_t pool = 0;
        if (password.find_first_of(s_LowerCaseChars) != std::string::npos)
            pool += s_LowerCaseChars.length();
        if (password.find_first_of(s_UpperCaseChars) != std::string::npos)
            pool += s_UpperCaseChars.length();
        if (password.find_first_of(s_NumbersChars) != std::string::npos)
            pool += s_NumbersChars.length();
        if (password.find_first_of(s_SymbolsChars) != std::string::npos)
            pool += s_SymbolsChars.length();

        return (double)password.length() * std::log2(pool);
    }
}

/// A multitude of parameters to generate passwords using
struct Generator::PasswordPolicy
{
    uint64_t passwordLength;
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
    inline void SetPolicy(const PasswordPolicy& newPolicy) { policy = newPolicy;}

    /**
     * Generates a simple password based on the current policy (only password length is used).
     * @param intelligible A boolean flag to indicate whether the password should consist
     *                     of only intelligible characters (a-z). Default is false.
     * @return A randomly generated password as a string adhering to the current policy.
     */
    [[nodiscard]] std::string GenerateSimplePassword(bool intelligible = false) const;

    /**
     * Generates an intermediate password adhering to the current password policy.
     * Invalid character groups are bypassed dynamically during generation.
     *
     * @return A randomly generated intermediate password as a string.
     */
    [[nodiscard]] std::string GenerateIntermediatePassword() const;

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