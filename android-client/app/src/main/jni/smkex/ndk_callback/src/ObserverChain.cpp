#include "ObserverChain.h"

ObserverChain::ObserverChain(jobject pJobject, jmethodID pID) {
    store_Wlistener=pJobject;
    store_method = pID;
}

