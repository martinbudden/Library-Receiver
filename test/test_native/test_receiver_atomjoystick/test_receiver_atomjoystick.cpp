#include "ReceiverAtomJoyStick.h"

#include <unity.h>

void setUp()
{
}

void tearDown()
{
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
void test_receiver_atom_joystick_auxiliary_channels()
{
    enum { AUXILIARY_CHANNEL_COUNT = 3};
    std::array<uint8_t, 6> macAddress;

    static ReceiverAtomJoyStick receiver(&macAddress[0]);

    uint8_t switchIndex = 0;
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    TEST_ASSERT_EQUAL(0, receiver.getAuxiliaryChannel(switchIndex));
    receiver.setSwitch(switchIndex, 1);
    TEST_ASSERT_GREATER_THAN(500, receiver.getAuxiliaryChannel(switchIndex));

    switchIndex = 1;
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    TEST_ASSERT_EQUAL(0, receiver.getAuxiliaryChannel(switchIndex));
    receiver.setSwitch(switchIndex, 1);
    TEST_ASSERT_GREATER_THAN(500, receiver.getAuxiliaryChannel(switchIndex));

    switchIndex = 2;
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    TEST_ASSERT_EQUAL(0, receiver.getAuxiliaryChannel(switchIndex));
    receiver.setSwitch(switchIndex, 1);
    TEST_ASSERT_GREATER_THAN(500, receiver.getAuxiliaryChannel(switchIndex));
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_receiver_atom_joystick_auxiliary_channels);

    UNITY_END();
}
