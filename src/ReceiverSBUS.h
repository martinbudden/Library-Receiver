#pragma once

#include "ReceiverSerial.h"
#include <TimeMicroSeconds.h>
#include <array>


class ReceiverSBUS : public ReceiverSerial {
public:
     // 16 11-bit channels (includes 4 main stick channels) and 2 flag channels
    enum { CHANNEL_11_BIT_COUNT = 16 };
    enum { CHANNEL_COUNT = 18 };
    enum { SBUS_START_BYTE = 0x0F, SBUS_END_BYTE = 0x00 };
    enum { SBUS_BAUD_RATE = 100000, SBUS_FAST_BAUDRATE = 200000 };
    enum { SBUS_DATA_BITS = 8, SBUS_STOP_BITS = 2, SBUS_PARITY = PARITY_EVEN }; // 8E2
    enum { SBUS_TIME_NEEDED_PER_FRAME = 3000 };
public:
    ReceiverSBUS(const pins_t& pins, uint8_t uartIndex, uint32_t baudrate);
    void init();
private:
    // Receiver is not copyable or moveable
    ReceiverSBUS(const ReceiverSBUS&) = delete;
    ReceiverSBUS& operator=(const ReceiverSBUS&) = delete;
    ReceiverSBUS(ReceiverSBUS&&) = delete;
    ReceiverSBUS& operator=(ReceiverSBUS&&) = delete;
public:
    virtual bool onDataReceived(uint8_t data) override;
    virtual void getStickValues(float& throttleStick, float& rollStick, float& pitchStick, float& yawStick) const override;
    virtual uint32_t getAuxiliaryChannel(size_t index) const override;
    virtual controls_pwm_t getControlsPWM() const override;

#if defined(FRAMEWORK_ESPIDF)
    IRAM_ATTR static void dataReadyISR();
#else
    static void dataReadyISR();
#endif
    bool isPacketEmpty() const { return _packetIsEmpty; }
    void setPacketEmpty() { _packetIsEmpty = true; }
    bool unpackPacket();
private:
    timeUs32_t _startTime {};
    enum { PACKET_SIZE = 25 };
    std::array<uint8_t, PACKET_SIZE> _packet {};
    size_t _packetIndex {};
    std::array<uint16_t, CHANNEL_COUNT> _channels {};


#if defined(FRAMEWORK_RPI_PICO)
    mutable mutex_t _dataReadyMutex{};
public:
    inline int32_t WAIT_DATA_READY(uint32_t ticksToWait) const { return mutex_enter_timeout_ms(&_dataReadyMutex, ticksToWait); } // returns true if mutex owned, false if timeout
    inline void SIGNAL_DATA_READY_FROM_ISR() const { mutex_exit(&_dataReadyMutex); }
#elif defined(FRAMEWORK_USE_FREERTOS)
    mutable uint32_t _dataReadyQueueItem {}; // this is just a dummy item whose value is not used
    enum { IMU_DATA_READY_QUEUE_LENGTH = 1 };
    std::array<uint8_t, IMU_DATA_READY_QUEUE_LENGTH * sizeof(_dataReadyQueueItem)> _dataReadyQueueStorageArea {};
    StaticQueue_t _dataReadyQueueStatic {};
    QueueHandle_t _dataReadyQueue {};
public:
    inline int32_t WAIT_DATA_READY(uint32_t ticksToWait) const { return xQueueReceive(_dataReadyQueue, &_dataReadyQueueItem, ticksToWait); } // returns pdPASS(1) if queue read, pdFAIL(0) if timeout
    inline void SIGNAL_DATA_READY_FROM_ISR() const { xQueueSendFromISR(_dataReadyQueue, &_dataReadyQueueItem, nullptr); }
#else
public:
    inline int32_t WAIT_DATA_READY(uint32_t ticksToWait) const { (void)ticksToWait; return 0; }
    inline void SIGNAL_DATA_READY_FROM_ISR() const {}
#endif
};
