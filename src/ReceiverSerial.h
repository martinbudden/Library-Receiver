#pragma once

#include "SerialPort.h"
#include "ReceiverBase.h"


class ReceiverSerialPortWatcher : public SerialPortWatcherBase {
public:
    explicit ReceiverSerialPortWatcher(ReceiverBase& receiver);
    bool onDataReceivedFromISR(uint8_t data) override;
private:
    ReceiverBase& _receiver;
};


class ReceiverSerial : public ReceiverBase {
public:
    ReceiverSerial(const SerialPort::serial_pins_t& pins, uint8_t uartIndex, uint32_t baudrate, uint8_t dataBits, uint8_t stopBits, uint8_t parity);
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
    virtual uint8_t readByte() override;
    virtual bool update(uint32_t tickCountDelta) override;
    bool isPacketEmpty() const { return _packetIsEmpty; }
    void setPacketEmpty() { _packetIsEmpty = true; }
    size_t getPacketIndex() const { return _packetIndex; } // for testing
protected:
    SerialPort _serialPort;
    ReceiverSerialPortWatcher _serialPortWatcher;
    bool _packetIsEmpty {true};
    uint32_t _receivedPacketCount {};
    int32_t _errorPacketCount {};
    size_t _packetIndex {};
    timeUs32_t _startTime {};
};
