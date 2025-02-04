#include <gtest/gtest.h>

#include <Generator.h>

using namespace Generator;

/// For now, just using simple test fixtures.
class PasswordGenerationTests1 : public testing::Test
{
public:
    PasswordGenerator passwordGenerator = PasswordGenerator(PasswordPolicy{10, true, true, true, true, ""});
    void SetUp() override
    {
        // not sure if it is good practice to init libsodium here
        if (sodium_init() < 0)
        {
            std::cerr << "Failed to initialize libsodium" << std::endl;
            throw std::runtime_error("Failed to initialize libsodium");
        }
        passwordGenerator = PasswordGenerator(PasswordPolicy{10, true, true, true, true, ""});
    }
};

TEST_F(PasswordGenerationTests1, SimplePasswordGeneratesTheRightAmountOfCharacters)
{
    // given:
    constexpr int passwordLength = 10;
    const PasswordPolicy& policy = PasswordPolicy{passwordLength, true, true, true, true, ""};

    // when:
    passwordGenerator.SetPolicy(policy);
    const std::string password = passwordGenerator.GenerateSimplePassword();

    // then:
    EXPECT_EQ(password.length(), passwordLength) << "Incorrect password length";
}


TEST_F(PasswordGenerationTests1, IntermediatePasswordGeneratesTheRightAmountOfCharacters)
{
    // given:
    constexpr int passwordLength = 10;
    const PasswordPolicy& policy = PasswordPolicy{passwordLength, true, true, true, true, ""};

    // when:
    passwordGenerator.SetPolicy(policy);
    const std::string password = passwordGenerator.GenerateIntermediatePassword();

    // then:
    EXPECT_EQ(password.length(), passwordLength) << "Incorrect password length";
}

TEST_F(PasswordGenerationTests1, GenerateMultiplePasswordsIntermediateGeneratesTheRightAmountOfPasswords)
{
    // given:
    constexpr int nPasswords = 1000;
    const PasswordPolicy& policy = PasswordPolicy{10, true, true, true, true, ""};

    // when:
    passwordGenerator.SetPolicy(policy);
    const auto passwords = passwordGenerator.GenerateIntermediatePasswords(nPasswords);

    // then:
    EXPECT_EQ(passwords.size(), nPasswords) << "Incorrect number of passwords generated";
}

TEST_F(PasswordGenerationTests1, AdvancedPasswordGeneratesAdheresToPolicy)
{
    // given:
    const PasswordPolicy& policy = PasswordPolicy{10, true, true, true, true, "cAde"};

    // when:
    passwordGenerator.SetPolicy(policy);
    const std::string password = passwordGenerator.GenerateAdvancedPassword();

    // then:
    EXPECT_EQ(password.length(), policy.passwordLength) << "Incorrect password length";

    const bool hasLC = std::ranges::any_of(password, [](const char& c) { return Generator::s_LowerCaseChars.find(c) != std::string::npos; });
    const bool hasUC = std::ranges::any_of(password, [](const char& c) { return Generator::s_UpperCaseChars.find(c) != std::string::npos; });
    const bool hasNum = std::ranges::any_of(password, [](const char& c) { return Generator::s_NumbersChars.find(c) != std::string::npos; });
    const bool hasSym = std::ranges::any_of(password, [](const char& c) { return Generator::s_SymbolsChars.find(c) != std::string::npos; });
    EXPECT_TRUE(hasLC) << "\nPassword does not contain lowercase characters\n";
    EXPECT_TRUE(hasUC) << "\nPassword does not contain uppercase characters\n";
    EXPECT_TRUE(hasNum) << "\nPassword does not contain numbers\n";
    EXPECT_TRUE(hasSym) << "\nPassword does not contain symbols\n";
    std::cout << password << std::endl;
}

TEST_F(PasswordGenerationTests1, AutomaticallyGeneratedAndHashedPasswordCorrectlyHashes)
{
    // given:
    passwordGenerator.SetPolicyEncryptionStrength(EncryptionStrength::Low);

    // when:
    const auto [password, hash] = passwordGenerator.GenerateHashedPassword();

    // then:
    EXPECT_TRUE(passwordGenerator.VerifyPasswordSafe(password, hash)) << "Password hash verification failed";
}

TEST_F(PasswordGenerationTests1, HashingPasswordsCorrectlyHash)
{
    // given:
    const std::string password = "<PASSWORD1?2.3!4@hello>";
    passwordGenerator.SetPolicyEncryptionStrength(EncryptionStrength::Low);

    // when:
    const std::string encrypted = passwordGenerator.HashPassword(password);

    // then:
    EXPECT_TRUE(passwordGenerator.VerifyPasswordSafe(password, encrypted)) << "Password hash verification failed";
}

TEST_F(PasswordGenerationTests1, HashPasswordSafeClearsMemory) {
    // given:
    std::string initialPassword = "<PASSWORD1?2.3!4@hello>";
    passwordGenerator.SetPolicyEncryptionStrength(EncryptionStrength::Low);

    std::string password = initialPassword;  // Create a copy for hashing
    ASSERT_FALSE(password.empty());         // Verify the password is NOT empty before hashing

    // when:
    const std::string encrypted = passwordGenerator.HashPasswordSafe(std::move(password));

    // then:
    // Verify that the hashed password matches the original input
    EXPECT_TRUE(passwordGenerator.VerifyPasswordSafe(initialPassword, encrypted))
        << "Password hash verification failed";

    for (const auto& c : password)
    {
        // Verify that the original password is cleared from memory
        EXPECT_EQ(c, '\0') << "Password was not cleared from memory";
    }

}
