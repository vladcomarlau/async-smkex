# Libwebsocket Compilation Tutorial


```sh
# Clone libwebsocket project 
git clone https://github.com/warmcat/libwebsockets.git

# Perform compilation  
cd libwebsockets
mkdir build
cd build
cmake ..
make 

# Install the libwebsocket library on Linux
sudo make install 
```



# Simple app clients 

## Sender:
Performs HTTP POST request to http://server_addr:port/send-message/{sender_serial}/{receiver_serial} 
Source code: smkex-http-client-post/smkex-http-client-post.c
Demo: smkex-http-client-post/build/build.sh


## Receiver:
Create a web-socket connection to http://server_addr:port/smkex-websocket/{receiver_serial}
Source code: smkex-ws-client-rx/smkex-ws-client.c
Demo: smkex-ws-client-rx/build/build.sh


