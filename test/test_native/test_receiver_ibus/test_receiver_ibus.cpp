#include "ReceiverIBUS.h"

#include <unity.h>

void setUp()
{
}

void tearDown()
{
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,misc-const-correctness,readability-magic-numbers)
void test_receiver_ibus()
{
    static ReceiverIBUS receiver{ReceiverSerial::pins_t{}, 0, 0};

    receiver.setPacketEmpty();
    TEST_ASSERT_TRUE(receiver.isPacketEmpty());
    TEST_ASSERT_EQUAL(0, receiver.getPacketIndex());

    std::array<uint8_t, 32> data = {
        0x20, 0x40, 0xDB, 0x05, 0xDC, 0x05, 0x54, 0x05,
        0xDC, 0x05, 0xE8, 0x03, 0xD0, 0x07, 0xD2, 0x05,
        0xE8, 0x03, 0xDC, 0x05, 0xDC, 0x05, 0xDC, 0x05,
        0xDC, 0x05, 0xDC, 0x05, 0xDC, 0x05, 0x80, 0x4F
    };

    TEST_ASSERT_FALSE(receiver.onDataReceived(data[0]));
    TEST_ASSERT_EQUAL(ReceiverIBUS::MODEL_IA6B, receiver.getModel());
    TEST_ASSERT_EQUAL(32, receiver.getSyncByte());
    TEST_ASSERT_EQUAL(32, receiver.getFrameSize());
    TEST_ASSERT_EQUAL(2, receiver.getChannelOffset());
    TEST_ASSERT_EQUAL(1, receiver.getPacketIndex());

    for (size_t ii = 1; ii < 30; ++ii) {
        const bool ret = receiver.onDataReceived(data[ii]);
        TEST_ASSERT_FALSE(ret);
        TEST_ASSERT_EQUAL(ii + 1, receiver.getPacketIndex());
    }
    TEST_ASSERT_FALSE(receiver.onDataReceived(data[30]));
    TEST_ASSERT_EQUAL(31, receiver.getPacketIndex());
    TEST_ASSERT_TRUE(receiver.onDataReceived(data[31]));
    TEST_ASSERT_EQUAL(0, receiver.getPacketIndex());

    TEST_ASSERT_EQUAL(0x80, receiver.getPacket(30));
    TEST_ASSERT_EQUAL(0x4F, receiver.getPacket(31));

    TEST_ASSERT_TRUE(receiver.unpackPacket());
    TEST_ASSERT_EQUAL(0x4F80, receiver.getReceivedChecksum());
    TEST_ASSERT_EQUAL(0x4F80, receiver.calculateChecksum());

    TEST_ASSERT_EQUAL(0x05DB, receiver.getChannelRaw(0));
    TEST_ASSERT_EQUAL(0x05DC, receiver.getChannelRaw(1));
    TEST_ASSERT_EQUAL(0x0554, receiver.getChannelRaw(2));
    TEST_ASSERT_EQUAL(0x05DC, receiver.getChannelRaw(3));
    TEST_ASSERT_EQUAL(0x03E8, receiver.getChannelRaw(4));
    TEST_ASSERT_EQUAL(0x07D0, receiver.getChannelRaw(5));
    TEST_ASSERT_EQUAL(0x05D2, receiver.getChannelRaw(6));
    TEST_ASSERT_EQUAL(0x03E8, receiver.getChannelRaw(7));
    TEST_ASSERT_EQUAL(0x05DC, receiver.getChannelRaw(8));
    TEST_ASSERT_EQUAL(0x05DC, receiver.getChannelRaw(9));
    TEST_ASSERT_EQUAL(0x05DC, receiver.getChannelRaw(10));
    TEST_ASSERT_EQUAL(0x05DC, receiver.getChannelRaw(11));
    TEST_ASSERT_EQUAL(0x05DC, receiver.getChannelRaw(12));
    TEST_ASSERT_EQUAL(0x05DC, receiver.getChannelRaw(13));
 }

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,misc-const-correctness,readability-magic-numbers)

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_receiver_ibus);

    UNITY_END();
}
