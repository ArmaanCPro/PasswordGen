#include <gtest/gtest.h>

#include <Generator.h>

using namespace Generator;

class PasswordGenerationTests1 : public testing::Test
{
public:
    PasswordGenerator passwordGenerator = PasswordGenerator(PasswordPolicy{10, true, true, true, true, ""});
    void SetUp() override
    {
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