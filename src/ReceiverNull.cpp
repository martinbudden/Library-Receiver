#include "ReceiverNull.h"


void ReceiverNull::WAIT_FOR_DATA_RECEIVED()
{
}

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
    throttleStick = Q12dot4_to_float(_controls.throttleStickQ12dot4);
    rollStick = Q12dot4_to_float(_controls.rollStickQ12dot4);
    pitchStick = Q12dot4_to_float(_controls.pitchStickQ12dot4);
    yawStick = Q12dot4_to_float(_controls.yawStickQ12dot4);
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
    return (index >= _auxiliaryChannelCount) ? CHANNEL_LOW : getSwitch(index) ? CHANNEL_HIGH : CHANNEL_LOW;
}
