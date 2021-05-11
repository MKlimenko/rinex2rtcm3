#include "gtest/gtest.h"
#include "rinex2rtcm3.hpp"

#include <fstream>
#include <numeric>

namespace {
	auto GetParameters(const std::vector<std::string>& arguments_string) {
		std::vector<char*> arguments;
		for (auto& el : arguments_string)
			arguments.push_back(const_cast<char*>(el.c_str()));
		return rinex2rtcm3::Parameters(static_cast<int>(arguments.size()), arguments.data());
	}

	auto ProcessRinex(rinex2rtcm3::Parameters::OutputMessageType type) {
		std::vector<std::string> arguments_string{
			"",
			"--input",
			absolute(std::filesystem::path(TEST_PATH + std::string("07590920.*"))).string(),
			"--output",
			"output_file.rtcm3",
			"--type",
		};

		switch (type) {
		case rinex2rtcm3::Parameters::OutputMessageType::CompactMsm:
			arguments_string.emplace_back("compact_msm");
			break;
		case rinex2rtcm3::Parameters::OutputMessageType::FullMsm:
			arguments_string.emplace_back("full_msm");
			break;
		case rinex2rtcm3::Parameters::OutputMessageType::Legacy:
			arguments_string.emplace_back("legacy");
			break;
		case rinex2rtcm3::Parameters::OutputMessageType::CustomSet:
			break;
		default:
			break;
		}

		const auto parameters = GetParameters(arguments_string);

		auto converter = rinex2rtcm3::Converter(parameters);
		return converter.Process();
	}

	auto GetRtcmInfo() {
		auto rtcm = std::unique_ptr<rtcm_t, decltype(&free_rtcm)>(new rtcm_t, free_rtcm);
		init_rtcm(rtcm.get());
		std::string keep_ephemeris = "-EPHALL";
		std::copy(keep_ephemeris.begin(), keep_ephemeris.end(), rtcm->opt);
		auto file_pointer = std::unique_ptr<FILE, decltype(&fclose)>(fopen("output_file.rtcm3", "rb"), fclose);

		auto number_of_messages = 0;
		while (input_rtcm3f(rtcm.get(), file_pointer.get()) != -2)
			++number_of_messages;

		return std::make_pair(std::move(rtcm), number_of_messages);
	}

	void MessageSetVerificationHelper(rinex2rtcm3::Parameters::OutputMessageType current_type) {
		const auto messages_written = ProcessRinex(current_type);

		auto [rtcm, number_of_messages] = GetRtcmInfo();

		ASSERT_EQ(messages_written, number_of_messages);

		std::set<std::size_t> message_types_in_output;
		for (std::size_t i = 0; i < std::size(rtcm->nmsg3); ++i)
			if (rtcm->nmsg3[i])
				message_types_in_output.emplace(1000 + i);		
	}
}

namespace argument_parsing {
	TEST(argument_parsing, input_legacy) {
		std::vector<std::string> arguments_string{
			"",
			"--input",
			"input_file.obs",
			"--input",
			"input_file.nav",
			"--output",
			"output_file.rtcm3",
			"--type",
			"legacy"
		};
		auto parameters = GetParameters(arguments_string);;

		ASSERT_EQ(arguments_string[2], parameters.input_filenames[0]);
		ASSERT_EQ(arguments_string[4], parameters.input_filenames[1]);
		ASSERT_EQ(arguments_string[6], parameters.output_filename);
		ASSERT_EQ(parameters.message_type, rinex2rtcm3::Parameters::OutputMessageType::Legacy);
		ASSERT_FALSE(parameters.interleaved);
	}

	TEST(argument_parsing, compact_msm) {
		std::vector<std::string> arguments_string{
			"",
			"--input",
			"input_file.obs",
			"--input",
			"input_file.nav",
			"--output",
			"output_file.rtcm3",
			"--type",
			"compact_msm"
		};
		auto parameters = GetParameters(arguments_string);;

		ASSERT_EQ(arguments_string[2], parameters.input_filenames[0]);
		ASSERT_EQ(arguments_string[4], parameters.input_filenames[1]);
		ASSERT_EQ(arguments_string[6], parameters.output_filename);
		ASSERT_EQ(parameters.message_type, rinex2rtcm3::Parameters::OutputMessageType::CompactMsm);
		ASSERT_FALSE(parameters.interleaved);
	}

	TEST(argument_parsing, full_msm) {
		std::vector<std::string> arguments_string{
			"",
			"--input",
			"input_file.obs",
			"--input",
			"input_file.nav",
			"--output",
			"output_file.rtcm3",
			"--type",
			"full_msm"
		};
		auto parameters = GetParameters(arguments_string);;

		ASSERT_EQ(arguments_string[2], parameters.input_filenames[0]);
		ASSERT_EQ(arguments_string[4], parameters.input_filenames[1]);
		ASSERT_EQ(arguments_string[6], parameters.output_filename);
		ASSERT_EQ(parameters.message_type, rinex2rtcm3::Parameters::OutputMessageType::FullMsm);
		ASSERT_FALSE(parameters.interleaved);
	}

	TEST(argument_parsing, custom_set) {
		std::vector<std::string> arguments_string{
			"",
			"--input",
			"input_file.obs",
			"--input",
			"input_file.nav",
			"--output",
			"output_file.rtcm3",
			"--type",
			"custom_set",
			"--custom",
			"1",
			"--custom",
			"2",
			"--custom",
			"3",
		};
		auto parameters = GetParameters(arguments_string);

		ASSERT_EQ(arguments_string[2], parameters.input_filenames[0]);
		ASSERT_EQ(arguments_string[4], parameters.input_filenames[1]);
		ASSERT_EQ(arguments_string[6], parameters.output_filename);
		ASSERT_EQ(parameters.message_type, rinex2rtcm3::Parameters::OutputMessageType::CustomSet);
		ASSERT_EQ(parameters.message_set, (std::vector<std::size_t>{ 1, 2, 3 }));
		ASSERT_FALSE(parameters.interleaved);
	}

	TEST(argument_parsing, input_interleaved) {
		std::vector<std::string> arguments_string{
			"",
			"--input",
			"input_file.obs",
			"--input",
			"input_file.nav",
			"--output",
			"output_file.rtcm3",
			"--type",
			"legacy",
			"--interleave",
		};
		auto parameters = GetParameters(arguments_string);;

		ASSERT_EQ(arguments_string[2], parameters.input_filenames[0]);
		ASSERT_EQ(arguments_string[4], parameters.input_filenames[1]);
		ASSERT_EQ(arguments_string[6], parameters.output_filename);
		ASSERT_EQ(parameters.message_type, rinex2rtcm3::Parameters::OutputMessageType::Legacy);
		ASSERT_TRUE(parameters.interleaved);
	}


	TEST(argument_parsing, no_input) {
		std::vector<std::string> arguments_string{
			"",
			"--output",
			"output_file.rtcm3",
			"--type",
			"custom_set",
			"--custom",
			"1",
		};

		try {
			auto parameters = GetParameters(arguments_string);
		}
		catch (...) {
			SUCCEED();
			return;
		}
		FAIL();
	}
	TEST(argument_parsing, no_output) {
		std::vector<std::string> arguments_string{
			"",
			"--input",
			"input_file.obs",
			"--type",
			"custom_set",
			"--custom",
			"1",
		};

		try {
			auto parameters = GetParameters(arguments_string);
		}
		catch (...) {
			SUCCEED();
			return;
		}
		FAIL();
	}
	TEST(argument_parsing, no_custom_data) {
		std::vector<std::string> arguments_string{
			"",
			"--input",
			"input_file.obs",
			"--output",
			"input_file.rtcm3",
			"--type",
			"custom_set",
		};

		try {
			auto parameters = GetParameters(arguments_string);
		}
		catch (...) {
			SUCCEED();
			return;
		}
		FAIL();
	}
}
TEST(message_set_verification, legacy) {
	MessageSetVerificationHelper(rinex2rtcm3::Parameters::OutputMessageType::Legacy);
}
TEST(message_set_verification, compact_msm) {
	MessageSetVerificationHelper(rinex2rtcm3::Parameters::OutputMessageType::CompactMsm);
}
TEST(message_set_verification, full_msm) {
	MessageSetVerificationHelper(rinex2rtcm3::Parameters::OutputMessageType::FullMsm);
}