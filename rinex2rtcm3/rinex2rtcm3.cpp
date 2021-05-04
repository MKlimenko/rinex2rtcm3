#include "rinex2rtcm3.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) {
	try {
		auto parameters = rinex2rtcm3::Parameters(argc, argv);

		auto converter = rinex2rtcm3::Converter(parameters);
		converter.Process();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
