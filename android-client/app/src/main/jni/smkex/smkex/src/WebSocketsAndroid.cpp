#include "WebSocketsAndroid.hpp"
#include "SmkexJNI.hpp"

WebSocketsAndroid &WebSocketsAndroid::getInstance() {
    static WebSocketsAndroid instance;
    return instance;
}

int WebSocketsAndroid::sendMessageToBuddy(
                    const std::string &buddy,
                    const uint8_t *data,
                    uint32_t dataLen,
                    int channel) {
    sendDataViaChannel(buddy.c_str(), data, dataLen, channel);
    return 0;
}

int WebSocketsAndroid::updateMessagesFromServer() {
    return 0;
}

void WebSocketsAndroid::init(
                    const std::string clientID[],
                    const std::string buddyId[],
                    int nrChannels) {

    for (int i = 0; i < nrChannels; i++)
        initSmkexWebSockets(clientID[i].c_str(), SMKEX_CHANNELS[i]);
}

