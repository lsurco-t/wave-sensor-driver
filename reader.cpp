#include <iostream>
#include <fstream>
#include <string>

int main(){
	std::cout << "Attempting to read from kernel device..." << std::endl;

	std::ifstream sensor("/dev/wave_sensor");

	if (!sensor.is_open()){
		std::cerr << "Error: Could not open /dev/wave_sensor. Are you running with sudo?" << std::endl; 
		return 1;
	}

	std::string data;
	std::getline(sensor, data);

	std::cout << "Data received from Kernel Space: " << data << std::endl;

	sensor.close();
	return 0;
}