#pragma once

#include "ReceiverBase.h"
#include <array>


class ReceiverSBUS : public ReceiverBase {
public:
    ReceiverSBUS();
private:
    // Receiver is not copyable or moveable
    ReceiverSBUS(const ReceiverSBUS&) = delete;
    ReceiverSBUS& operator=(const ReceiverSBUS&) = delete;
    ReceiverSBUS(ReceiverSBUS&&) = delete;
    ReceiverSBUS& operator=(ReceiverSBUS&&) = delete;
public:
 // 16 11-bit channels (includes 4 main stick channels) and 2 flag channels
    enum { CHANNEL_COUNT = 18 };
public:
    virtual int32_t WAIT_FOR_DATA_RECEIVED() override;
    virtual int32_t WAIT_FOR_DATA_RECEIVED(uint32_t ticksToWait) override;
    virtual bool update(uint32_t tickCountDelta) override;
    virtual void getStickValues(float& throttleStick, float& rollStick, float& pitchStick, float& yawStick) const override;
    virtual EUI_48_t getMyEUI() const override;
    virtual EUI_48_t getPrimaryPeerEUI() const override;
    virtual void broadcastMyEUI() const override;
    virtual uint32_t getAuxiliaryChannel(size_t index) const override;

    bool isPacketEmpty() const { return _packetIsEmpty; }
    void setPacketEmpty() { _packetIsEmpty = true; }
    enum checkPacket_t { CHECK_PACKET, DONT_CHECK_PACKET };
    bool unpackPacket(checkPacket_t checkPacket);
private:
    uint32_t _packetIsEmpty {true};
    uint32_t _packetCount {0};
    uint32_t _receivedPacketCount {0};
    int32_t _droppedPacketCount {0};
    int32_t _droppedPacketCountPrevious {0};
    enum { THROTTLE = 0, ROLL = 1, PITCH = 2, YAW = 3, STICK_COUNT = 4 };
    std::array<uint32_t, STICK_COUNT> _sticks {};
    enum { PACKET_SIZE = 22 + 2 }; // 22 bytes for first 16 channels, 1 for flags, one to round up size
    std::array<uint8_t, PACKET_SIZE> _packet {};
    std::array<uint16_t, CHANNEL_COUNT> _channels {};
};
