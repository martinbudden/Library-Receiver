#pragma once

#include "ReceiverBase.h"
#include <TimeMicroseconds.h>
#include <array>

#if defined(FRAMEWORK_USE_FREERTOS)
#if defined(FRAMEWORK_ESPIDF) || defined(FRAMEWORK_ARDUINO_ESP32)
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#else
#if defined(FRAMEWORK_ARDUINO_STM32)
#include <STM32FreeRTOS.h>
#endif
#include <FreeRTOS.h>
#include <queue.h>
#endif
#endif

#if defined(FRAMEWORK_RPI_PICO)
#include <hardware/uart.h>
#include <pico/mutex.h>
#elif defined(FRAMEWORK_ESPIDF)
#elif defined(FRAMEWORK_STM32_CUBE) || defined(FRAMEWORK_ARDUINO_STM32)

#if defined(FRAMEWORK_STM32_CUBE_F1)
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_uart.h>
#elif defined(FRAMEWORK_STM32_CUBE_F3)
#include <stm32f3xx_hal.h>
#include <stm32f3xx_hal_gpio.h>
#include <stm32f3xx_hal_uart.h>
#elif defined(FRAMEWORK_STM32_CUBE_F4)
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_uart.h>
#elif defined(FRAMEWORK_STM32_CUBE_F7)
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_gpio.h>
#include <stm32f7xx_hal_uart.h>
#endif
#elif defined(FRAMEWORK_TEST)
#else // defaults to FRAMEWORK_ARDUINO
#if defined(FRAMEWORK_ARDUINO_ESP32)
#include <HardwareSerial.h>
#endif
#endif


class ReceiverSerial : public ReceiverBase {
public:
    enum { PARITY_NONE, PARITY_EVEN, PARITY_ODD };
    struct port_pin_t {
        uint8_t port;
        uint8_t pin;
    };
    struct pins_t {
        uint8_t rx;
        uint8_t tx;
    };
    struct stm32_rx_pins_t {
        port_pin_t rx;
        port_pin_t tx;
    };
public:
    ReceiverSerial(const stm32_rx_pins_t& pins, uint8_t uartIndex, uint32_t baudrate, uint8_t dataBits, uint8_t stopBits, uint8_t parity);
    void init();
private:
    // Receiver is not copyable or moveable
    ReceiverSerial(const ReceiverSerial&) = delete;
    ReceiverSerial& operator=(const ReceiverSerial&) = delete;
    ReceiverSerial(ReceiverSerial&&) = delete;
    ReceiverSerial& operator=(ReceiverSerial&&) = delete;
public:
    virtual int32_t WAIT_FOR_DATA_RECEIVED(uint32_t ticksToWait) override;
    virtual bool isDataAvailable() const override;
    virtual uint8_t getByte() override;
    virtual bool update(uint32_t tickCountDelta) override;
    FAST_CODE static void dataReadyISR();
    bool isPacketEmpty() const { return _packetIsEmpty; }
    void setPacketEmpty() { _packetIsEmpty = true; }
    size_t getPacketIndex() const { return _packetIndex; } // for testing
protected:
    uint32_t _packetIsEmpty {true};
    uint32_t _receivedPacketCount {0};
    int32_t _errorPacketCount {0};
    size_t _packetIndex {};
    timeUs32_t _startTime {};
private:
    static ReceiverSerial* self; //!< alias of `this` to be used in interrupt service routine
    stm32_rx_pins_t _pins {};
    const uint8_t _uartIndex;
    const uint8_t _dataBits;
    const uint8_t _stopBits;
    const uint8_t _parity;
    const uint32_t _baudrate;
#if defined(FRAMEWORK_RPI_PICO) || defined(FRAMEWORK_ARDUINO_RPI_PICO)
    uart_inst_t* _uart {};
#elif defined(FRAMEWORK_STM32_CUBE) || defined(FRAMEWORK_ARDUINO_STM32)
    UART_HandleTypeDef _uart {};
#elif defined(FRAMEWORK_TEST)
#else // defaults to FRAMEWORK_ARDUINO
#if defined(FRAMEWORK_ARDUINO_ESP32)
    HardwareSerial _uart;
#endif
#endif

#if defined(FRAMEWORK_USE_FREERTOS)

    mutable uint32_t _dataReadyQueueItem {}; // this is just a dummy item whose value is not used
    enum { IMU_DATA_READY_QUEUE_LENGTH = 1 };
    std::array<uint8_t, IMU_DATA_READY_QUEUE_LENGTH * sizeof(_dataReadyQueueItem)> _dataReadyQueueStorageArea {};
    StaticQueue_t _dataReadyQueueStatic {};
    QueueHandle_t _dataReadyQueue {};
public:
    inline int32_t WAIT_DATA_READY(uint32_t ticksToWait) const { return xQueueReceive(_dataReadyQueue, &_dataReadyQueueItem, ticksToWait); } // returns pdPASS(1) if queue read, pdFAIL(0) if timeout
    inline void SIGNAL_DATA_READY_FROM_ISR() const { xQueueSendFromISR(_dataReadyQueue, &_dataReadyQueueItem, nullptr); }
#else

#if defined(FRAMEWORK_RPI_PICO)
    mutable mutex_t _dataReadyMutex{};
public:
    inline int32_t WAIT_DATA_READY(uint32_t ticksToWait) const { return mutex_enter_timeout_ms(&_dataReadyMutex, ticksToWait); } // returns true if mutex owned, false if timeout
    inline void SIGNAL_DATA_READY_FROM_ISR() const { mutex_exit(&_dataReadyMutex); }
#else
public:
    inline int32_t WAIT_DATA_READY(uint32_t ticksToWait) const { (void)ticksToWait; return 0; }
    inline void SIGNAL_DATA_READY_FROM_ISR() const {}
#endif // FRAMEWORK_RPI_PICO

#endif // FRAMEWORK_USE_FREERTOS
};
