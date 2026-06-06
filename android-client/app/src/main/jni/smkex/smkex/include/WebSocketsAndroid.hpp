#include "ISmkexTransportCallback.h"
#include "ISmkexTransport.h"
#include "WebSockets.h"

class WebSocketsAndroid : public ISmkexTransport {
    // de initializat constructor gol privat (pentru singleton)
    // de intializat constructor privat de copiere

    public:

    static WebSocketsAndroid &getInstance();

    int sendMessageToBuddy( const std::string &buddy,
                            const uint8_t *data,
                            uint32_t dataLen,
                            int channel) override;

    int updateMessagesFromServer() override;

    void init(  const std::string clientID[],
                const std::string buddyId[],
                int nrChannels);
};