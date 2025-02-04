#include <gtest/gtest.h>

#include <Generator.h>

using namespace Generator;

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
    EXPECT_EQ(password.length(), passwordLength);
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
    EXPECT_EQ(password.length(), passwordLength);
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
    EXPECT_EQ(passwords.size(), nPasswords);
}

TEST_F(PasswordGenerationTests1, AutomaticallyGeneratedAndHashedPasswordCorrectlyHashes)
{
    // given:
    passwordGenerator.SetPolicyEncryptionStrength(EncryptionStrength::Medium);

    // when:
    const auto [password, hash] = passwordGenerator.GenerateHashedPassword();

    // then:
    EXPECT_TRUE(passwordGenerator.VerifyPassword(password, hash));
}

TEST_F(PasswordGenerationTests1, HashingPasswordsCorrectlyHash)
{
    // given:
    const std::string password = "<PASSWORD1?2.3!4@hello>";
    passwordGenerator.SetPolicyEncryptionStrength(EncryptionStrength::Medium);

    // when:
    const std::string encrypted = passwordGenerator.HashPassword(password);

    // then:
    EXPECT_TRUE(passwordGenerator.VerifyPassword(password, encrypted));
}