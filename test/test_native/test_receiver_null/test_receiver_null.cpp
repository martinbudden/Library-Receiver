#include "ReceiverNull.h"

#include <unity.h>

void setUp()
{
}

void tearDown()
{
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
void test_receiver_switches()
{
    ReceiverNull receiver; // NOLINT(misc-const-correctness) false positive

    uint8_t switchIndex = 0;
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 1);
    TEST_ASSERT_EQUAL(1, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 0);
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 3);
    TEST_ASSERT_EQUAL(3, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 0);
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));


    switchIndex = 1;
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 1);
    TEST_ASSERT_EQUAL(1, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 0);
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 3);
    TEST_ASSERT_EQUAL(3, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 0);
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));

    switchIndex = 2;
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 1);
    TEST_ASSERT_EQUAL(1, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 0);
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 3);
    TEST_ASSERT_EQUAL(3, receiver.getSwitch(switchIndex));
    receiver.setSwitch(switchIndex, 0);
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
}

void test_receiver_controls()
{
    ReceiverNull receiver;

    ReceiverBase::controls_t controls = receiver.getControls();
    TEST_ASSERT_EQUAL(0, controls.throttleStickQ12dot4);
    TEST_ASSERT_EQUAL(0, controls.rollStickQ12dot4);
    TEST_ASSERT_EQUAL(0, controls.pitchStickQ12dot4);
    TEST_ASSERT_EQUAL(0, controls.yawStickQ12dot4);

    controls = { 2, 3, 4, 5 };
    receiver.setControls(controls);
    TEST_ASSERT_EQUAL(2, controls.throttleStickQ12dot4);
    TEST_ASSERT_EQUAL(3, controls.rollStickQ12dot4);
    TEST_ASSERT_EQUAL(4, controls.pitchStickQ12dot4);
    TEST_ASSERT_EQUAL(5, controls.yawStickQ12dot4);
}

void test_receiver_auxiliary_channels()
{
    enum { AUXILIARY_CHANNEL_COUNT = 4};
    ReceiverNull receiver(AUXILIARY_CHANNEL_COUNT);

    uint8_t switchIndex = 0;
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    TEST_ASSERT_EQUAL(ReceiverBase::CHANNEL_LOW, receiver.getAuxiliaryChannel(switchIndex));
    receiver.setSwitch(switchIndex, 1);
    TEST_ASSERT_EQUAL(ReceiverBase::CHANNEL_HIGH, receiver.getAuxiliaryChannel(switchIndex));

    switchIndex = 1;
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    TEST_ASSERT_EQUAL(ReceiverBase::CHANNEL_LOW, receiver.getAuxiliaryChannel(switchIndex));
    receiver.setSwitch(switchIndex, 1);
    TEST_ASSERT_EQUAL(ReceiverBase::CHANNEL_HIGH, receiver.getAuxiliaryChannel(switchIndex));

    switchIndex = 2;
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    TEST_ASSERT_EQUAL(ReceiverBase::CHANNEL_LOW, receiver.getAuxiliaryChannel(switchIndex));
    receiver.setSwitch(switchIndex, 1);
    TEST_ASSERT_EQUAL(ReceiverBase::CHANNEL_HIGH, receiver.getAuxiliaryChannel(switchIndex));

    switchIndex = 3;
    TEST_ASSERT_EQUAL(0, receiver.getSwitch(switchIndex));
    TEST_ASSERT_EQUAL(ReceiverBase::CHANNEL_LOW, receiver.getAuxiliaryChannel(switchIndex));
    receiver.setSwitch(switchIndex, 1);
    TEST_ASSERT_EQUAL(ReceiverBase::CHANNEL_HIGH, receiver.getAuxiliaryChannel(switchIndex));
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_receiver_switches);
    RUN_TEST(test_receiver_controls);
    RUN_TEST(test_receiver_auxiliary_channels);

    UNITY_END();
}
