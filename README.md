# vrep_remote_api_shm
This version of the V-REP remote API enables the shared memory based communication for Windows and POSIX platforms. Tests have been conducted on Windows, macOS and Linux.

# Why using shared memory
In one word: speed. The original method uses a TCP connection between the client (your program) and the server (V-REP). 
When transfering a significant amount of data between the client and the server the latter will become sluggish mainly because of all the data being copied multiple times. 
The result is a low real time factor when using the synchronous mode or data not being updated at every time step in the other case.
Shared memory allows the client and the server to share a common memory segment and thus avoiding all unnecessary copies. This connection mechanism is only viable if the client and the server run on the same machine, otherwise the TCP communication must be used.

# Installation
Both the server (V-REP) and client (your application) parts need to be updated with this version. You can try to directly use the precompiled versions:
* lib/libv_repExtRemoteApi.so and lib/libremoteApi.so for Linux
* lib/libv_repExtRemoteApi.dylib and lib/libremoteApi.dylib for macOS
* lib/Release/libv_repExtRemoteApi.dll and lib/Release/remoteApi.dll for Windows

but if it doesn't work for you, try to recompile them (see next section) before following the installation instructions.

The first step is to update the V-REP's remote api plugin. To do so, simply replace the libv_repExtRemoteApi.[so|dll] file into your V-REP installation directory with this one.

For the second step, it will depend on how you use the remote API library:
* You link your application to a shared library? replace that shared library with (lib)remoteApi.[so|dll]
* You link your application to a static library? change the following line `add_library(remoteApi SHARED ${client_side_src})` in `CMakeLists.txt` to `add_library(remoteApi STATIC ${client_side_src})`, recompile the library then link your application with the newly generated one.
* You include the remote API files into your application? you need to replace your files with the content of the `remoteApi` folder and to add `include/shared_memory.h` and `common/shared_memory.c` to your build.


# Compilation
To compile the libraries, you will need a C++ compiler (e.g. GCC, Clang, VC++), CMake, Git and Make (non-Windows systems).
## Linux & macOS
From a terminal run:
1. `git clone git@github.com:BenjaminNavarro/vrep_remote_api_shm.git`
2. `cd vrep_remote_api_shm`
3. `mkdir build`
4. `cd build`
5. `cmake ..`
6. `make`

## Windows
Visual Studio needs to be installed and the compiler (cl) must be in the PATH. 
Open the Git Bash application then run:
1. `git clone git@github.com:BenjaminNavarro/vrep_remote_api_shm.git`
2. `cd vrep_remote_api_shm`
3. `mkdir build`
4. `cd build`
5. `cmake -G "Visual Studio 15 2017 Win64" ..`
6. `cmake --build . --config Release`

# Usage
To start a remote API server using shared memory you need to use negative port numbers. More info on that at http://www.coppeliarobotics.com/helpFiles/en/remoteApiServerSide.htm.

Once the server is started, you can connect to it from your client using: `simxStart("", port, waitUntilConnected, doNotReconnectOnceDisconnected, timeOutInMs, commThreadCycleInMs)` whith the same negative port as given to the server.

# Performance gain
It depends on both your scene and your OS. You can use the `test_shm` application to check is using the shared memory can be beneficial on your machine (compare the real time factors printed at the end). On the same laptop, with V-REP 3.4.0, I get the following real time factors by running the `test_shm` benchmark:
* Linux (4.11): 0.98 (shared memory) / 0.21 (TCP)
* Windows 10: 0.86 (shared memory) / 0.84 (TCP)
* macOS Sierra: 0.99 (shared memory) / 0.99 (TCP), with increased data size: 0.87 (shared memory), 0.52 (TCP)

It can seen that the best performance is acheived using the shared memory on Linux but also that the difference on Windows is not significant. More tests are needed to determine the specific cases where it is trully usefull.  

# Acknowledgements
Even if it has not been release by Coppelia, the implementation for Windows was already done but deactivated (both a compile flag and commented sections). I just made an abstraction layer to make it work with Windows and POSIX systems and improved the system a bit.
