#include "ReceiverNull.h"


/*!
If a packet was received then unpack it and inform the motor controller there are new stick values.

Returns true if a packet has been received.
*/
bool ReceiverNull::update([[maybe_unused]] uint32_t tickCountDelta)
{
    ++_packetCount;
    _droppedPacketCount = static_cast<int32_t>(_receivedPacketCount - _packetCount);
    _droppedPacketCountDelta = _droppedPacketCount - _droppedPacketCountPrevious;
    _droppedPacketCountPrevious = _droppedPacketCount;

    _newPacketAvailable = true;
    return true;
}


void ReceiverNull::getStickValues(float&  throttleStick, float&  rollStick, float&  pitchStick, float&  yawStick) const
{
    throttleStick = Q4dot12_to_float(_controls.throttleStickQ4dot12);
    rollStick = Q4dot12_to_float(_controls.rollStickQ4dot12);
    pitchStick = Q4dot12_to_float(_controls.pitchStickQ4dot12);
    yawStick = Q4dot12_to_float(_controls.yawStickQ4dot12);
}

ReceiverBase::EUI_48_t ReceiverNull::getMyEUI() const
{
    EUI_48_t ret {};
    return ret;
}

ReceiverBase::EUI_48_t ReceiverNull::getPrimaryPeerEUI() const
{
    EUI_48_t ret {};
    return ret;
}

void ReceiverNull::broadcastMyEUI() const
{
}

uint32_t ReceiverNull::getAuxiliaryChannel(size_t index) const
{
    // map switches to the auxiliary channels
    enum { CHANNEL_HIGH = 2000 };
    return (index >= _auxiliaryChannelCount) ? 0 : getSwitch(index) ? CHANNEL_HIGH : 0;
}
