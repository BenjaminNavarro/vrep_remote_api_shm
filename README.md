# vrep_remote_api_shm
This version of the V-REP remote API enables the shared memory based communication for Windows and POSIX platforms. For now, only Linux have been tested. Tests on other platforms will come soon.

# Why using shared memory
In one word: speed. The original method uses a TCP connection between the client (your program) and the server (V-REP). 
When transfering a significant amount of data between the client and the server the latter will become sluggish mainly because of all the data being copied multiple times. 
The result is a low real time factor when using the synchronous mode or data not being updated at every time step in the other case.
Shared memory allows the client and the server to share a common memory segment and thus avoiding all unnecessary copies. This connection mechanism is only viable if the client and the server run on the same machine, otherwise the TCP communication must be used.

# How to use it?
You first need to recompile both the server and the client libraries: 
 1. run `make` in the `v_repExtRemoteApi` folder then copy `v_repExtRemoteApilib/libv_repExtRemoteApi.so` to your V-REP installation folder.
 2. run `make` in the `remoteApiBindings/lib` folder then link your application with `remoteApiBindings/lib/lib/remoteApi.so`.

Then, to start a remote API server using shared memory do it as usual but with a negative port number:
 * Edit your `remoteApiConnections.txt` to add a new server or edit the existing one
 * Start one from inside a script: `simExtRemoteApiStart(id, maxPacketSize, debug, preEnableTrigger)` with id<0 and maxPacketSize<=3200000.

And finally connect to it from your client: `simxStart("--unused--", id, waitUntilConnected, doNotReconnectOnceDisconnected, timeOutInMs, commThreadCycleInMs)` whith the same negative id as given to the server.

# Performance gain
On a scenario where ~1800 bytes are sent from the server to the client using the synchronous mode, I got a real time factor increasing from 0.2 to 0.5 with dt=10ms.

# TODO
 * Check with Windows and macOS.
 * Memory is constantly map/unmapped, try to leave it mapped and see if there is any speed improvement.

# Known issues
 * V-REP exits with a Seg Fault when using shared memory. No real issue from the user point of view.

# Acknowledgements
Even if it has not been release by Coppelia, the implementation for Windows was already done but deactivated (both a compile flag and commented sections). I just made an abstraction layer to make it work with Windows and POSIX systems.



CMake on Windows: 
```
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config Release
```
