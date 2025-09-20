#include "ReceiverSerial.h"

#if defined(FRAMEWORK_RPI_PICO)
#include <hardware/gpio.h>
#include <hardware/uart.h>
#elif defined(FRAMEWORK_ESPIDF)
#elif defined(FRAMEWORK_TEST)
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
static inline GPIO_TypeDef* gpioPort(uint8_t port) { return reinterpret_cast<GPIO_TypeDef*>(GPIOA_BASE + port*(GPIOB_BASE - GPIOA_BASE)); }
static inline uint16_t gpioPin(uint8_t pin) { return static_cast<uint16_t>(1U << pin); }
#else // defaults to FRAMEWORK_ARDUINO
#endif // FRAMEWORK


/*!
Pointer to the receiver used by the ISR.
*/
ReceiverSerial* ReceiverSerial::receiver;


#if defined(FRAMEWORK_RPI_PICO)
void __not_in_flash_func(ReceiverSerial::dataReadyISR)() // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
{
    //gpio_put(PICO_DEFAULT_LED_PIN, 1);
    while (uart_is_readable(uart1)) {
        // Read 1 byte from UART buffer and give it to the RX protocol parser
        const uint8_t data = uart_getc(uart1); // NOLINT(cppcoreguidelines-init-variables) false positive
        if (receiver->onDataReceived(data)) {
            // onDataReceived returns true once packet is complete
            receiver->SIGNAL_DATA_READY_FROM_ISR();
        }
    }
}
#else
FAST_CODE void ReceiverSerial::dataReadyISR()
{
    receiver->SIGNAL_DATA_READY_FROM_ISR();
}
#endif

ReceiverSerial::ReceiverSerial(const port_pins_t& pins, uint8_t uartIndex, uint32_t baudrate, uint8_t dataBits, uint8_t stopBits, uint8_t parity) :
    _pins(pins),
    _uartIndex(uartIndex),
    _dataBits(dataBits),
    _stopBits(stopBits),
    _parity(parity),
    _baudrate(baudrate)
{
    receiver = this;
}

void ReceiverSerial::init()
{
    _packetCount = 0;
#if defined(FRAMEWORK_RPI_PICO)
    // see https://github.com/victorhook/asac-fc/blob/main/src/receiver.c
    _uart = uart_get_instance(_uartIndex);

    uart_init(uart1, _baudrate);
    gpio_set_function(_pins.rx.pin, GPIO_FUNC_UART);
    gpio_set_function(_pins.tx.pin, GPIO_FUNC_UART);

    enum { NO_CTS = false, NO_RTS = false };
    uart_set_hw_flow(_uart, NO_CTS, NO_RTS);

    const uart_parity_t parity =  // NOLINT(cppcoreguidelines-init-variables) false positive
        (_parity == PARITY_NONE) ? UART_PARITY_NONE :
        (_parity == PARITY_EVEN) ? UART_PARITY_EVEN : UART_PARITY_ODD;
    uart_set_format(_uart, _dataBits, _stopBits, parity);

    uart_set_fifo_enabled(_uart, true);

    // Enable UART interrupt
    const irq_num_t irqNum = _uartIndex == 0 ? UART0_IRQ : UART1_IRQ; // NOLINT*cppcoreguidelines-init-variables) false positive
    irq_set_exclusive_handler(irqNum, dataReadyISR);
    irq_set_enabled(irqNum, true);
    enum { RX_NEEDS_DATA = true, RX_DOES_NOT_NEED_DATA = false };
    enum { TX_NEEDS_DATA = true, TX_DOES_NOT_NEED_DATA = false };
    uart_set_irq_enables(_uart, RX_NEEDS_DATA, TX_DOES_NOT_NEED_DATA);

#elif defined(FRAMEWORK_ESPIDF)

#elif defined(FRAMEWORK_TEST)

#else // defaults to FRAMEWORK_ARDUINO

#endif
}

/*!
This waits for data from the serial UART
*/
int32_t ReceiverSerial::WAIT_FOR_DATA_RECEIVED(uint32_t ticksToWait)
{
    return WAIT_DATA_READY(ticksToWait);
}

/*!
If a packet was received then unpack it and return true.

Returns false if an empty or invalid packet was received.
*/
bool ReceiverSerial::update(uint32_t tickCountDelta)
{
    if (isPacketEmpty()) {
        return false;
    }

    if (!unpackPacket()) {
        return false;
    }

    _packetReceived = true;
    ++_packetCount;

    // record tickoutDelta for instrumentation
    _tickCountDelta = tickCountDelta;

    // track dropped packets
    _droppedPacketCountDelta = _droppedPacketCount - _droppedPacketCountPrevious;
    _droppedPacketCountPrevious = _droppedPacketCount;

    // NOTE: there is no mutex around this flag
    _newPacketAvailable = true;
    return true;
}
