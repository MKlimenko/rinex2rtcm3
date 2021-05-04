#include "lyra/lyra.hpp"

#include "rinex2rtcm3.hpp"
#include <iostream>
#include <stdexcept>



int main(int argc, char** argv) {
	try {
		rinex2rtcm3::Parameters p;

		auto converter = rinex2rtcm3::Converter(p);
		
		
		
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
