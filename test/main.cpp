#include <extApi.h>
#include <extApiPlatform.h>
#include <v_repConst.h>

#include <iostream>
#include <unistd.h>
#include <time.h>
#include <chrono>  // for high_resolution_clock

#define VREP_DT 0.01
#define TEST_DURATION 10.

void startStreaming(int client_id) {
	simxUChar* signal;
	simxInt sLength;
	for (size_t i = 0; i < 10; ++i) {
		std::string name = "toClient"+std::to_string(i);
		simxReadStringStream(client_id, name.c_str(), &signal, &sLength, simx_opmode_streaming);
	}
}

double run(int client_id) {
	simxUChar* signal;
	simxInt sLength;
	auto start = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < size_t(TEST_DURATION/VREP_DT); i++) {
		simxSynchronousTrigger(client_id);
		for (size_t i = 0; i < 10; ++i) {
			std::string name = "toClient"+std::to_string(i);
			if(simxReadStringStream(client_id, name.c_str(), &signal, &sLength, simx_opmode_buffer) != simx_return_ok) {
				std::cerr << "Unable to read signal " << name << std::endl;
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	return elapsed.count();
}

int main(int argc, char const *argv[]) {

	double time_shm = -1;
	double time_tcp = -1;
	int client_id_shm = -1;
	int client_id_tcp = -1;

	std::cout << "Connecting to V-REP through shared memory" << std::endl;
	client_id_shm = simxStart("", -1000, true, true, 2000, 5);
	if(client_id_shm < 0) {
		std::cerr << "Failed to connect to V-REP using the shared memory ID -1000" << std::endl;
	}
	std::cout << "Connecting to V-REP through TCP" << std::endl;
	client_id_tcp = simxStart("127.0.0.1", 19997, true, true, 2000, 5);
	if(client_id_tcp < 0) {
		std::cerr << "Failed to connect to V-REP using the TCP port 19997" << std::endl;
	}

	simxStartSimulation(client_id_shm, simx_opmode_oneshot_wait);

	/***            Shared memory             ***/
	usleep(1000000);
	startStreaming(client_id_shm);

	std::cout << "Starting the shared memory test" << std::endl;
	simxSynchronous(client_id_shm, true);
	usleep(1000000);
	time_shm = run(client_id_shm);
	usleep(1000000);
	simxSynchronous(client_id_shm, false);

	/***            TCP             ***/
	usleep(1000000);
	startStreaming(client_id_tcp);

	std::cout << "Starting the TCP test" << std::endl;
	simxSynchronous(client_id_tcp, true);
	time_tcp = run(client_id_tcp);
	simxSynchronous(client_id_tcp, false);

	std::cout << "time_shm: " << time_shm << " (real time factor: " << TEST_DURATION/time_shm << ")" << std::endl;
	std::cout << "time_tcp: " << time_tcp << " (real time factor: " << TEST_DURATION/time_tcp << ")" << std::endl;

	simxStopSimulation(client_id_tcp, simx_opmode_oneshot_wait);

	return 0;
}
