#pragma once

#include "ReceiverBase.h"


class ReceiverNull : public ReceiverBase {
public:
    virtual ~ReceiverNull() = default;
    explicit ReceiverNull(uint32_t auxiliaryChannelCount) { _auxiliaryChannelCount = auxiliaryChannelCount; }
    ReceiverNull() : ReceiverNull(0) {}
private:
    // ReceiverNull is not copyable or moveable
    ReceiverNull(const ReceiverNull&) = delete;
    ReceiverNull& operator=(const ReceiverNull&) = delete;
    ReceiverNull(ReceiverNull&&) = delete;
    ReceiverNull& operator=(ReceiverNull&&) = delete;
public:
    virtual int32_t WAIT_FOR_DATA_RECEIVED() override;
    virtual int32_t WAIT_FOR_DATA_RECEIVED(uint32_t ticksToWait) override;
    virtual bool update(uint32_t tickCountDelta) override;
    virtual void getStickValues(float& throttleStick, float& rollStick, float& pitchStick, float& yawStick) const override;
    virtual EUI_48_t getMyEUI() const override;
    virtual EUI_48_t getPrimaryPeerEUI() const override;
    virtual void broadcastMyEUI() const override;
    virtual uint32_t getAuxiliaryChannel(size_t index) const override;
public: // for testing
    void setControls(const controls_t& controls) { _controls = controls; }
private:
    uint32_t _packetCount {0};
    uint32_t _receivedPacketCount {0};
    int32_t _droppedPacketCount {0};
    int32_t _droppedPacketCountPrevious {0};
};
