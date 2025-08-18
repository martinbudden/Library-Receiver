#include "ReceiverSBUS.h"

#pragma pack(push, 1)
struct sbus_channels_t { // NOLINT(altera-struct-pack-align)
    // 176 bits of data (11 bits per channel * 16 channels) = 22 bytes.
    unsigned int channel0 : 11;
    unsigned int channel1 : 11;
    unsigned int channel2 : 11;
    unsigned int channel3 : 11;
    unsigned int channel4 : 11;
    unsigned int channel5 : 11;
    unsigned int channel6 : 11;
    unsigned int channel7 : 11;
    unsigned int channel8 : 11;
    unsigned int channel9 : 11;
    unsigned int channel10 : 11;
    unsigned int channel11 : 11;
    unsigned int channel12 : 11;
    unsigned int channel13 : 11;
    unsigned int channel14 : 11;
    unsigned int channel15 : 11;
    uint8_t flags;
};
#pragma pack(pop)


ReceiverSBUS::ReceiverSBUS()
{
    _auxiliaryChannelCount = CHANNEL_COUNT - STICK_COUNT;
}

int32_t ReceiverSBUS::WAIT_FOR_DATA_RECEIVED()
{
    return 0;
}

int32_t ReceiverSBUS::WAIT_FOR_DATA_RECEIVED(uint32_t ticksToWait)
{
    (void)ticksToWait;
    return 0;
}

/*!
If a packet was received from the atomJoyStickReceiver then unpack it and inform the receiver target that new stick values are available.

Returns true if a packet has been received.
*/
bool ReceiverSBUS::update(uint32_t tickCountDelta)
{
    if (isPacketEmpty()) {
        return false;
    }

    _packetReceived = true;

    // record tickoutDelta for instrumentation
    _tickCountDelta = tickCountDelta;

    // track dropped packets
    ++_packetCount;

    if (unpackPacket(CHECK_PACKET)) {

        // Save the stick values.
        _controls.throttleStickQ12dot4 = 0;
        _controls.rollStickQ12dot4 = 0;
        _controls.pitchStickQ12dot4 = 0;
        _controls.yawStickQ12dot4 = 0;

        // now we have copied all the packet values, set the _newPacketAvailable flag
        // NOTE: there is no mutex around this flag
        _newPacketAvailable = true;
        return true;
    }
    return true;
}

void ReceiverSBUS::getStickValues(float& throttleStick, float& rollStick, float& pitchStick, float& yawStick) const
{
    throttleStick = static_cast<float>(_sticks[THROTTLE]);
    rollStick = static_cast<float>(_sticks[ROLL]);
    pitchStick = static_cast<float>(_sticks[PITCH]);
    yawStick = static_cast<float>(_sticks[YAW]);
}

ReceiverBase::EUI_48_t ReceiverSBUS::getMyEUI() const
{
    EUI_48_t ret {};
    return ret;
}

ReceiverBase::EUI_48_t ReceiverSBUS::getPrimaryPeerEUI() const
{
    EUI_48_t ret {};
    return ret;
}

void ReceiverSBUS::broadcastMyEUI() const
{
}

uint32_t ReceiverSBUS::getAuxiliaryChannel(size_t index) const
{
    return (index >= _auxiliaryChannelCount) ? CHANNEL_LOW : CHANNEL_HIGH;
}

/*!
Check the packet if `checkPacket` set. If the packet is valid then unpack it into the member data and set the packet to empty.

Returns true if a valid packet received, false otherwise.
*/
bool ReceiverSBUS::unpackPacket(checkPacket_t checkPacket)
{
    (void)checkPacket;

    if (isPacketEmpty()) {
        return false;
    }

    const sbus_channels_t& channels = *reinterpret_cast<sbus_channels_t*>(&_packet[0]);
    _channels[0] = channels.channel0;
    _channels[1] = channels.channel1;
    _channels[2] = channels.channel2;
    _channels[3] = channels.channel3;
    _channels[4] = channels.channel4;
    _channels[5] = channels.channel5;
    _channels[6] = channels.channel6;
    _channels[7] = channels.channel7;
    _channels[8] = channels.channel8;
    _channels[9] = channels.channel9;
    _channels[10] = channels.channel10;
    _channels[11] = channels.channel11;
    _channels[12] = channels.channel12;
    _channels[13] = channels.channel13;
    _channels[14] = channels.channel14;
    _channels[15] = channels.channel15;

    enum { FLAG_CHANNEL_16 = 0x01, FLAG_CHANNEL_17 = 0x02, FLAG_FAILSAFE_ACTIVE = 0x04, FLAG_SIGNAL_LOSS = 0x08 };

    _channels[16] = (channels.flags & FLAG_CHANNEL_16) ? CHANNEL_HIGH : CHANNEL_LOW;
    _channels[17] = (channels.flags & FLAG_CHANNEL_17) ? CHANNEL_HIGH : CHANNEL_LOW;

    _sticks[ROLL] = _channels[0];
    _sticks[PITCH] = _channels[1];
    _sticks[YAW] = _channels[2];
    _sticks[THROTTLE] = _channels[3];

    setPacketEmpty();
    return true;
}

