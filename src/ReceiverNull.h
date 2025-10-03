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
    virtual int32_t WAIT_FOR_DATA_RECEIVED(uint32_t ticksToWait) override;
    virtual bool update(uint32_t tickCountDelta) override;
    virtual bool unpackPacket() override;
    virtual void getStickValues(float& throttleStick, float& rollStick, float& pitchStick, float& yawStick) const override;
    virtual uint16_t getChannelRaw(size_t index) const override;
public: // for testing
    void setControls(const controls_t& controls) { _controls = controls; }
private:
    uint32_t _receivedPacketCount {};
};
