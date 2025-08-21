#pragma once

#include <cstddef>
#include <cstdint>

/*!
Abstract Base Class defining a receiver.
*/
class ReceiverBase {
public:
    enum { STICK_COUNT = 4 };
    enum { MOTOR_ON_OFF_SWITCH = 0 };
    enum { CHANNEL_LOW =  1000, CHANNEL_HIGH = 2000, CHANNEL_MIDDLE = 1500, CHANNEL_RANGE = CHANNEL_HIGH - CHANNEL_LOW };
    enum { // standardize receivers to use AETR (Ailerons, Elevator, Throttle, Rudder), ie ROLL, PITCH, THROTTLE, YAW
        ROLL,
        PITCH,
        THROTTLE,
        YAW,
        AUX1,
        AUX2,
        AUX3,
        AUX4,
        AUX5,
        AUX6,
        AUX7,
        AUX8,
        AUX9,
        AUX10,
        AUX11,
        AUX12,
        AUX13,
        AUX14,
        AUX15,
        AUX16,
    };
public:
     //! 48-bit extended unique identifier (often synonymous with MAC address)
    struct EUI_48_t {
        uint8_t octet[6];
    };
     //! unscaled control values from receiver as Q12.4 fixed point integer, ie in range [-2048, 2047]
    struct controls_t {
        int32_t throttleStickQ12dot4;
        int32_t rollStickQ12dot4;
        int32_t pitchStickQ12dot4;
        int32_t yawStickQ12dot4;
    };
    //! controls mapped to the Pulse Width Modulation (PWM) range [1000, 2000]
    struct controls_pwm_t {
        uint16_t throttleStick;
        uint16_t rollStick;
        uint16_t pitchStick;
        uint16_t yawStick;
    };
public:
    virtual ~ReceiverBase() = default;
    // BaseType_t is int, TickType_t is uint32_t
    virtual int32_t WAIT_FOR_DATA_RECEIVED(uint32_t ticksToWait) = 0;
    virtual bool update(uint32_t tickCountDelta) = 0;
    bool update() { return update(0); }
    virtual void getStickValues(float& throttleStick, float& rollStick, float& pitchStick, float& yawStick) const = 0;
    // 48-bit Extended Unique Identifiers, usually the MAC address if the receiver has one, but may be an alternative provided by the receiver.
    virtual EUI_48_t getMyEUI() const { const EUI_48_t ret {}; return ret; }
    virtual EUI_48_t getPrimaryPeerEUI() const  { const EUI_48_t ret {}; return ret; }
    virtual void broadcastMyEUI() const {}

    inline controls_t getControls() const { return _controls; }
    virtual controls_pwm_t getControlsPWM() const {
        return controls_pwm_t {
            .throttleStick = static_cast<uint16_t>((_controls.throttleStickQ12dot4 >> 2) + CHANNEL_MIDDLE),
            .rollStick = static_cast<uint16_t>((_controls.rollStickQ12dot4 >> 2) + CHANNEL_MIDDLE),
            .pitchStick = static_cast<uint16_t>((_controls.pitchStickQ12dot4 >> 2) + CHANNEL_MIDDLE),
            .yawStick = static_cast<uint16_t>((_controls.yawStickQ12dot4 >> 2) + CHANNEL_MIDDLE)
        };
    }

    uint32_t getAuxiliaryChannelCount() const { return _auxiliaryChannelCount; }
    virtual uint32_t getAuxiliaryChannel(size_t index) const = 0;

    inline uint32_t getSwitch(size_t index) const { return (_switches & (0b11U << (2*index))) >> (2*index); }
    inline void setSwitch(size_t index, uint8_t value) { _switches &= ~(0b11U << (2*index)); _switches |= (value & 0b11U) << (2*index); }
    inline uint32_t getSwitches() const { return _switches; }

    inline int32_t getDroppedPacketCountDelta() const { return _droppedPacketCountDelta; }
    inline uint32_t getTickCountDelta() const { return _tickCountDelta; }
    inline static float Q12dot4_to_float(int32_t q4dot12) { return static_cast<float>(q4dot12) * (1.0F / 2048.0F); } //<! convert Q12dot4 fixed point number to floating point

    inline bool isPacketReceived() const { return _packetReceived; }
    inline bool isNewPacketAvailable() const { return _newPacketAvailable; }
    inline void clearNewPacketAvailable() { _newPacketAvailable = false; }
protected:
    int32_t _packetReceived {false}; // may be invalid packet
    int32_t _newPacketAvailable {false};
    int32_t _droppedPacketCountDelta {0};
    uint32_t _tickCountDelta {0};
    uint32_t _switches {0}; // 16 2 or 3 positions switches, each using 2-bits
    controls_t _controls {}; //!< the main 4 channels
    uint32_t _auxiliaryChannelCount {0};
};

class ReceiverWatcher {
public:
    virtual void newReceiverPacketAvailable() = 0;
};
