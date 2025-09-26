#pragma once

#include "ReceiverSerial.h"
#include <TimeMicroseconds.h>
#include <array>

/*!
SBUS receiver protocol, used primarily by Futaba and FrSky receivers.
*/
class ReceiverSBUS : public ReceiverSerial {
public:
     // 16 11-bit channels (includes 4 main stick channels) and 2 flag channels
    enum { CHANNEL_11_BIT_COUNT = 16 };
    enum { CHANNEL_COUNT = 18 };
    enum { SBUS_START_BYTE = 0x0F, SBUS_END_BYTE = 0x00 };
    enum { BAUD_RATE = 100000, FAST_BAUDRATE = 200000 };
    enum { DATA_BITS = 8, PARITY = PARITY_EVEN, STOP_BITS = 2 }; // 8E2
    enum { TIME_NEEDED_PER_FRAME_US = 3000 };
public:
    ReceiverSBUS(const rx_pins_t& pins, uint8_t uartIndex, uint32_t baudrate);
    ReceiverSBUS(const stm32_rx_pins_t& pins, uint8_t uartIndex, uint32_t baudrate);
private:
    // Receiver is not copyable or moveable
    ReceiverSBUS(const ReceiverSBUS&) = delete;
    ReceiverSBUS& operator=(const ReceiverSBUS&) = delete;
    ReceiverSBUS(ReceiverSBUS&&) = delete;
    ReceiverSBUS& operator=(ReceiverSBUS&&) = delete;
public:
    virtual bool onDataReceived(uint8_t data) override;
    virtual void getStickValues(float& throttleStick, float& rollStick, float& pitchStick, float& yawStick) const override;
    virtual uint16_t getChannelRaw(size_t index) const override;
    virtual bool unpackPacket() override;
private:
    enum { PACKET_SIZE = 25 };
    std::array<uint8_t, PACKET_SIZE> _packetISR {};
    std::array<uint8_t, PACKET_SIZE> _packet {};
    std::array<uint16_t, CHANNEL_COUNT> _channels {};
};
