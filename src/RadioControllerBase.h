#pragma once

#include "ReceiverBase.h"

/*!
The RadioController is what connects the Receiver to the VehicleController.
*/
class RadioControllerBase {
public:
    struct controls_t {
        uint32_t tickCount;
        float throttleStick;
        float rollStick;
        float pitchStick;
        float yawStick;
    };
public:
    explicit RadioControllerBase(ReceiverBase& receiver) : _receiver(receiver) {}
    const ReceiverBase& getReceiver() const { return _receiver; }
    uint32_t getTimeoutTicks() const { return _timeoutTicks; }
    void setTimeoutTicks(uint32_t timeoutTicks) { _timeoutTicks = timeoutTicks; }

    virtual void updateControls(const controls_t& controls) = 0;
    virtual void checkFailsafe(uint32_t tickCount) = 0;
    virtual uint32_t getFailsafePhase() const = 0;
protected:
    ReceiverBase& _receiver;
    uint32_t _timeoutTicks {100};
};
