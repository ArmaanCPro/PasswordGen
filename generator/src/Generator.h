#pragma once

#include <algorithm>
#include <iostream>
#include <string>

#include <random>
#include <future>
#include <utility>

#include <sodium.h>

namespace Generator
{
    static const std::string s_LowerCaseChars = "abcdefghijklmnopqrstuvwxyz";
    static const std::string s_UpperCaseChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static const std::string s_NumbersChars = "0123456789";
    static const std::string s_SymbolsChars = "!@#$%^&*()_+=-[]{}|;':\",./<>?";

    struct PasswordPolicy;
    class PasswordGenerator;

    enum class EncryptionStrength
    {
        Low,
        Medium,
        High
    };

    inline int sodiumOpsLimitFromEncryptionStrength(const EncryptionStrength& strength)
    {
        switch (strength)
        {
            case EncryptionStrength::Low:
                return crypto_pwhash_OPSLIMIT_MIN;
            case EncryptionStrength::Medium:
                return crypto_pwhash_OPSLIMIT_INTERACTIVE;
            case EncryptionStrength::High:
                return crypto_pwhash_OPSLIMIT_SENSITIVE;
            default:
                return crypto_pwhash_OPSLIMIT_MIN;
        }
    }

    inline int sodiumMemLimitFromEncryptionStrength(const EncryptionStrength& strength)
    {
        switch (strength)
        {
            case EncryptionStrength::Low:
                return crypto_pwhash_MEMLIMIT_MIN;
            case EncryptionStrength::Medium:
                return crypto_pwhash_MEMLIMIT_INTERACTIVE;
            case EncryptionStrength::High:
                return crypto_pwhash_MEMLIMIT_SENSITIVE;
            default:
                return crypto_pwhash_MEMLIMIT_MIN;
        }
    }

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
    PasswordPolicy() = default;
    explicit PasswordPolicy(uint64_t passwordLength = 10, bool requireLowercase = true, bool requireUppercase = true,
        bool requireNumbers = true, bool requireSymbols = true, std::string excludedCharacters = "",
        EncryptionStrength encryptionStrength = EncryptionStrength::Medium)
        :
        passwordLength(passwordLength),
        requireLowercase(requireLowercase),
        requireUppercase(requireUppercase),
        requireNumbers(requireNumbers),
        requireSymbols(requireSymbols),
        excludedCharacters(std::move(excludedCharacters)),
        encryptionStrength(encryptionStrength)
    {}
    uint64_t passwordLength = 10;
    bool requireLowercase = true;
    bool requireUppercase = true;
    bool requireNumbers = true;
    bool requireSymbols = true;
    std::string excludedCharacters;
    EncryptionStrength encryptionStrength = EncryptionStrength::Low;
};

/// Class for generating passwords. Only member is a password policy
class Generator::PasswordGenerator
{
public:
    explicit PasswordGenerator(PasswordPolicy policy)
        :
        policy(std::move(policy))
    {
        // not sure if it is good practice to init libsodium here
        if (sodium_init() < 0)
        {
            std::cerr << "Failed to initialize libsodium" << std::endl;
            throw std::runtime_error("Failed to initialize libsodium");
        }
    }

    /**
     * Updates the current password policy with a new policy definition.
     * @param newPolicy The new password policy to be set, which defines rules for the password generation
     */
    inline void SetPolicy(const PasswordPolicy& newPolicy) { policy = newPolicy;}
    /// Update specifically the encryption strength of the policy
    inline void SetPolicyEncryptionStrength(const EncryptionStrength& newEncryptionStrength) { policy.encryptionStrength = newEncryptionStrength; }

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

    /** Encrypts a password using libsodium crypto_pwhash_str. The password is generated from GenerateIntermediatePassword
     * @returns The hash and the salt
     */
    [[nodiscard]] std::tuple<std::string, std::string> GenerateHashedPassword() const;

    /**
     * Hash a password using libsodium.
     * @returns The hash and the salt
     */
    [[nodiscard]] std::tuple<std::string, std::string> HashPassword(const std::string& password) const;

private:
    PasswordPolicy policy;

};