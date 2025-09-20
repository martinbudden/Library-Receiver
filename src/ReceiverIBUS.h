#pragma once

#include "ReceiverSerial.h"
#include <TimeMicroseconds.h>
#include <array>


/*!
IBUS receiver protocol, used by Flysky receivers.
*/
class ReceiverIBUS : public ReceiverSerial {
public:
    enum { CHANNEL_COUNT = 18, SLOT_COUNT = 14 };
    enum { BAUD_RATE = 115200 };
    enum { DATA_BITS = 8, PARITY = PARITY_NONE, STOP_BITS = 1 }; // 8N1
    enum { TIME_NEEDED_PER_FRAME = 3000 };
    enum { MODEL_IA6, MODEL_IA6B };
    enum { SERIAL_RX_PACKET_LENGTH = 32, TELEMETRY_PACKET_LENGTH = 4 };
public:
    ReceiverIBUS(const pins_t& pins, uint8_t uartIndex, uint32_t baudrate);
    ReceiverIBUS(const port_pins_t& pins, uint8_t uartIndex, uint32_t baudrate);
    void init();
private:
    // Receiver is not copyable or moveable
    ReceiverIBUS(const ReceiverIBUS&) = delete;
    ReceiverIBUS& operator=(const ReceiverIBUS&) = delete;
    ReceiverIBUS(ReceiverIBUS&&) = delete;
    ReceiverIBUS& operator=(ReceiverIBUS&&) = delete;
public:
    virtual bool onDataReceived(uint8_t data) override;
    virtual void getStickValues(float& throttleStick, float& rollStick, float& pitchStick, float& yawStick) const override;
    virtual uint16_t getChannelRaw(size_t index) const override;
    virtual bool unpackPacket() override;
    uint16_t calculateChecksum() const;
    uint16_t getReceivedChecksum() const { return _packet[_frameSize - 2] + static_cast<uint16_t>(_packet[_frameSize - 1] << 8U); }
// for testing;
    uint8_t getModel() const { return _model; }
    uint8_t getSyncByte() const { return _syncByte; }
    uint8_t getFrameSize() const { return _frameSize; }
    uint8_t getChannelOffset() const { return _channelOffset; }
    uint8_t getPacket(size_t index) const { return _packet[index]; }
private:
    enum { PACKET_SIZE = 32 };
    std::array<uint8_t, PACKET_SIZE> _packetISR {};
    std::array<uint8_t, PACKET_SIZE> _packet {};
    std::array<uint16_t, CHANNEL_COUNT> _channels {};
    uint8_t _model {};
    uint8_t _syncByte {};
    uint8_t _frameSize {};
    uint8_t _channelOffset {};
};
