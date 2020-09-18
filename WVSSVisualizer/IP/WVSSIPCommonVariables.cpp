#include "WVSSIPCommonVariables.h"

const char *g_pSocketStateText[AVP_SOCKET_STATES] = {
 "The socket is not connected", 
 "The socket is performing a host name lookup", 
 "The socket has started establishing a connection", 
 "A connection is established", 
 "The socket is bound to an address and port", 
 "Listening state (for internal use only)",
 "The socket is about to close (data may still be waiting to be written)" };
