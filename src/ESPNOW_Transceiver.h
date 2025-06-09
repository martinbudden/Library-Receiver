
# pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#if defined(USE_ESPNOW)

#include <esp_now.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#else

#define IRAM_ATTR
typedef int esp_err_t;
enum {ESP_OK=0, ESP_FAIL=-1};
enum esp_now_send_status_t {ESP_NOW_SEND_SUCCESS = 0, ESP_NOW_SEND_FAIL};
enum  wifi_interface_t {WIFI_IF_STA = 1, WIFI_IF_AP  = 2};
enum {ESP_NOW_ETH_ALEN=6, ESP_NOW_KEY_LEN = 16};
struct esp_now_peer_info_t {
    uint8_t peer_addr[ESP_NOW_ETH_ALEN];
    uint8_t lmk[ESP_NOW_KEY_LEN];
    uint8_t channel;
    wifi_interface_t ifidx;
    bool encrypt;
    void *priv;
};

#endif // USE_ESP_NOW

class ESPNOW_Transceiver {
public:
    enum { BROADCAST_PEER=0, PRIMARY_PEER=1, SECONDARY_PEER=2, PEER_2=2, PEER_3=4, MAX_PEER_COUNT=4 };
    struct received_data_t {
        inline received_data_t(uint8_t* aBufferPtr, size_t aBufferSize) : bufferPtr(aBufferPtr), bufferSize(aBufferSize), len(0) {}
        uint8_t* bufferPtr;
        size_t bufferSize;
        size_t len;
    };
    struct peer_data_t {
        esp_now_peer_info_t peer_info { .peer_addr={0,0,0,0,0,0}, .lmk={0}, .channel=0, .ifidx=WIFI_IF_STA, .encrypt=false, .priv=nullptr };
        received_data_t* receivedDataPtr {nullptr};
    };
public:
    IRAM_ATTR explicit ESPNOW_Transceiver(const uint8_t* myMacAddress);
    // !!NOTE: all references passed to init() and addSecondaryPeer() must be static or allocated, ie they must not be local variables on the stack
    IRAM_ATTR esp_err_t init(received_data_t& received_data, uint8_t channel, const uint8_t* transmitMacAddress);
    IRAM_ATTR esp_err_t addSecondaryPeer(received_data_t& received_data, const uint8_t* macAddress);
    IRAM_ATTR inline const uint8_t* myMacAddress() const { return &_myMacAddress[0]; }
    // sends data to the primary peer,
    IRAM_ATTR esp_err_t sendData(const uint8_t* data, size_t len) const;
    // sends data to the secondary peer,
    IRAM_ATTR esp_err_t sendDataSecondary(const uint8_t* data, size_t len) const;
    IRAM_ATTR bool isPrimaryPeerMacAddressSet() const;
    IRAM_ATTR const uint8_t* getPrimaryPeerMacAddress() const { return _peerData[PRIMARY_PEER].peer_info.peer_addr; }
    IRAM_ATTR inline uint8_t getBroadcastChannel() const { return _peerData[BROADCAST_PEER].peer_info.channel; }
    IRAM_ATTR esp_err_t broadcastData(const uint8_t* data, size_t len) const {
#if defined(USE_ESPNOW)
         return esp_now_send(_peerData[BROADCAST_PEER].peer_info.peer_addr, data, len); 
#else
        (void)data; (void)len; return -1;
#endif
    }
    IRAM_ATTR inline uint32_t getReceivedPacketCount() const { return _receivedPacketCount; }
    IRAM_ATTR inline uint32_t getTickCountDelta() const { return _tickCountDelta; }
    IRAM_ATTR inline uint32_t getTickCountDeltaAndReset() { const uint32_t tickCountDelta = _tickCountDelta; _tickCountDelta = 0; return tickCountDelta; }
private:
    IRAM_ATTR esp_err_t init(uint8_t channel);
    IRAM_ATTR esp_err_t addBroadcastPeer(uint8_t channel);
    // when data is received the copy function is called to copy the received data into the client's buffer
    IRAM_ATTR bool copyReceivedDataToBuffer(const uint8_t* macAddress, const uint8_t* data, size_t len);
    IRAM_ATTR bool macAddressIsBroadCastMacAddress(const uint8_t* macAddress) const;
    IRAM_ATTR bool macAddressIsSecondaryPeerMacAddress(const uint8_t* macAddress) const;
    IRAM_ATTR esp_err_t setPrimaryPeerMacAddress(const uint8_t* macAddress);
    IRAM_ATTR inline void setSendStatus(esp_now_send_status_t status) { _sendStatus = status; }
private:
    IRAM_ATTR static void onDataSent(const uint8_t* macAddress, esp_now_send_status_t status);
    IRAM_ATTR static void onDataReceived(const uint8_t* macAddress, const uint8_t* data, int len); // len is int rather than size_t to match esp_now_recv_cb_t callback signature
private:
    static const std::array<uint8_t, ESP_NOW_ETH_ALEN> broadcastMacAddress;
    static ESPNOW_Transceiver* transceiver; // alias of `this` to be used in onDataSent and onDataReceived callback functions
    // tick counts are used for instrumentation
    uint32_t _tickCountPrevious {0};
    uint32_t _tickCountDelta {0};
    uint32_t _receivedPacketCount {0}; //!< used to check for dropped packets
    // by default the transceiver has two peers, the broadcast peer and the primary peer
    int _isPrimaryPeerMacAddressSet {false}; // this is int rather than bool because using bool caused strange bugs
    int _peerCount {0};
    std::array<peer_data_t, MAX_PEER_COUNT> _peerData;
    esp_now_send_status_t _sendStatus {ESP_NOW_SEND_SUCCESS};
    std::array<uint8_t, ESP_NOW_ETH_ALEN + 2> _myMacAddress {0, 0, 0, 0, 0, 0, 0, 0};
#if defined(USE_ESPNOW) && defined(USE_FREERTOS)
    enum { DATA_READY_QUEUE_LENGTH = 2};
    mutable uint32_t _primaryDataReceivedQueueItem {}; // this is just a dummy item whose value is not used
    std::array<uint8_t, DATA_READY_QUEUE_LENGTH * sizeof(_primaryDataReceivedQueueItem)> _primaryDataReceivedQueueStorageArea {};
    StaticQueue_t _primaryDataReceivedQueueStatic {};
    QueueHandle_t _primaryDataReceivedQueue {};
    mutable uint32_t _secondaryDataReceivedQueueItem {}; // this is just a dummy item whose value is not used
    std::array<uint8_t, DATA_READY_QUEUE_LENGTH * sizeof(_secondaryDataReceivedQueueItem)> _secondaryDataReceivedQueueStorageArea {};
    StaticQueue_t _secondaryDataReceivedQueueStatic {};
    QueueHandle_t _secondaryDataReceivedQueue {};
public:
    IRAM_ATTR inline void WAIT_FOR_PRIMARY_DATA_RECEIVED() const { xQueueReceive(_primaryDataReceivedQueue, &_primaryDataReceivedQueueItem, portMAX_DELAY); }
    IRAM_ATTR inline void SIGNAL_PRIMARY_DATA_RECEIVED_FROM_ISR() const { xQueueSendFromISR(_primaryDataReceivedQueue, &_primaryDataReceivedQueueItem, nullptr); }
    IRAM_ATTR inline void WAIT_FOR_SECONDARY_DATA_RECEIVED() const { xQueueReceive(_secondaryDataReceivedQueue, &_secondaryDataReceivedQueueItem, portMAX_DELAY); }
    IRAM_ATTR inline void SIGNAL_SECONDARY_DATA_RECEIVED_FROM_ISR() const { xQueueSendFromISR(_secondaryDataReceivedQueue, &_secondaryDataReceivedQueueItem, nullptr); }
#else
public:
    inline void WAIT_FOR_PRIMARY_DATA_RECEIVED() const {}
    inline void SIGNAL_PRIMARY_DATA_RECEIVED_FROM_ISR() const {}
    inline void WAIT_FOR_SECONDARY_DATA_RECEIVED() const {}
    inline void SIGNAL_SECONDARY_DATA_RECEIVED_FROM_ISR() const {}
#endif
};
