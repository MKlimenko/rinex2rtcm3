#pragma once

#include "rtklib.h"

#include <filesystem>
#include <vector>

namespace rinex2rtcm3 {
	struct Parameters {
		enum class OutputMessageType {
			CompactMsm,
			FullMsm,
			Legacy,
			CustomSet
		};

		std::vector<std::filesystem::path> input_filenames;
		std::filesystem::path output_filename;
		OutputMessageType message_type = OutputMessageType::CompactMsm;
		std::vector<std::size_t> message_set;

	private:
		void GetOutputMessageSet() {
			switch (message_type) {
			case OutputMessageType::CompactMsm:
				message_set = {1074, 1084, 1094, 1104, 1114, 1124, 1134, 1019, 1020, 1041, 1042, 1044, 1045, 1046, };
				break;
			case OutputMessageType::FullMsm:
				message_set = { 1077, 1087, 1097, 1107, 1117, 1127, 1137, 1019, 1020, 1041, 1042, 1044, 1045, 1046, };
				break;
			case OutputMessageType::Legacy:
				message_set = { 1004, 1012, 1019, 1020, };
				break;
			case OutputMessageType::CustomSet: 
			default:
				break;
			}
		}
	};

	class Converter {
		prcopt_t prcopt = prcopt_default;
		std::unique_ptr<strconv_t> conversion_stream;
	public:
		Converter(const Parameters& p) : conversion_stream(strconvnew(STRFMT_RINEX, STRFMT_RTCM3, "1077", 0, 0, prcopt.rnxopt[0])) {
			prcopt.navsys = SYS_ALL;

		}
	};
}