#include <extApi.h>
#include <extApiPlatform.h>
#include <v_repConst.h>

#include <iostream>


int main(int argc, char const *argv[]) {

  int port = -1000;
  if(argc > 1) {
    port = atoi(argv[1]);
  }

  int client_id = simxStart("127.0.0.1", port, true, true, 2000, 5);

  simxStartSimulation(client_id, simx_opmode_oneshot_wait);
  simxSynchronous(client_id, true);

  simxUChar* signal;
  simxInt sLength;
  simxReadStringStream(client_id, "toClient", &signal, &sLength, simx_opmode_streaming);

  if(client_id >= 0) {
    for (size_t i = 0; i < 1000; i++) {
      simxSynchronousTrigger(client_id);
      simxReadStringStream(client_id, "toClient", &signal, &sLength, simx_opmode_buffer);
      std::cout << "Length: " << sLength << std::endl;
    }
  }

  simxStopSimulation(client_id, simx_opmode_oneshot_wait);

  return 0;
}
