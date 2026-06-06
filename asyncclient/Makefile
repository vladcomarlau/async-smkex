# Async-SMKEX standalone client (Milestone 2).
# Reuses crypto.cpp from the existing SMKEX module; adds X25519/Ed25519,
# a local SQLite store, and libcurl networking.
#
# Build:  make
# Needs:  libssl-dev libsqlite3-dev libcurl4-openssl-dev

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wno-deprecated-declarations -pthread -I../MultiPathVoIPLite/smkex/include
LDLIBS   = -lssl -lcrypto -lsqlite3 -lcurl

SMKEX    = ../MultiPathVoIPLite/smkex/src/crypto.cpp
SRCS     = main.cpp aclient_crypto.cpp store.cpp http.cpp $(SMKEX)

aclient: $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o aclient $(LDLIBS)

clean:
	rm -f aclient

.PHONY: clean
