
#include <ProtoBufGuard.h>

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    ProtoBufGuard pbGuard;
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}