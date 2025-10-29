#include "ReceiverNull.h"


int32_t ReceiverNull::WAIT_FOR_DATA_RECEIVED(uint32_t ticksToWait)
{
    (void)ticksToWait;
    return 0;
}

/*!
If a packet was received then unpack it and inform the motor controller there are new stick values.

Returns true if a packet has been received.
*/
bool ReceiverNull::update(uint32_t tickCountDelta)
{
    (void)tickCountDelta;

    ++_packetCount;
    _droppedPacketCount = static_cast<int32_t>(_receivedPacketCount) - _packetCount;
    _droppedPacketCountDelta = _droppedPacketCount - _droppedPacketCountPrevious;
    _droppedPacketCountPrevious = _droppedPacketCount;

    _newPacketAvailable = true;
    return true;
}

bool ReceiverNull::unpackPacket()
{
    return true;
}

void ReceiverNull::getStickValues(float& throttleStick, float& rollStick, float& pitchStick, float& yawStick) const
{
    throttleStick = _controls.throttle;
    rollStick = _controls.roll;
    pitchStick = _controls.pitch;
    yawStick = _controls.yaw;
}

uint16_t ReceiverNull::getChannelRaw(size_t index) const
{
    // map switches to the auxiliary channels
    if (index < STICK_COUNT) {
        return CHANNEL_LOW;
    }
    if (index >= _auxiliaryChannelCount + STICK_COUNT) {
        return CHANNEL_LOW;
    }
    return getSwitch(index - STICK_COUNT) ? CHANNEL_HIGH : CHANNEL_LOW;
}
