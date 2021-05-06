#include "gtest/gtest.h"
#include "rinex2rtcm3.hpp"

namespace {
	auto GetParameters(const std::vector<std::string>& arguments_string) {
		std::vector<char*> arguments;
		for (auto& el : arguments_string)
			arguments.push_back(const_cast<char*>(el.c_str()));
		return rinex2rtcm3::Parameters(static_cast<int>(arguments.size()), arguments.data());
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
	std::vector<std::string> arguments_string{
		"",
		"--input",
		TEST_PATH + std::string("07590920.*"),
		"--output",
		"output_file.rtcm3",
		"--type",
		"legacy"
	};
	auto parameters = GetParameters(arguments_string);;

	ASSERT_EQ(arguments_string[2], parameters.input_filenames[0]);
	ASSERT_EQ(arguments_string[4], parameters.output_filename);
	ASSERT_EQ(parameters.message_type, rinex2rtcm3::Parameters::OutputMessageType::Legacy);
	
	auto converter = rinex2rtcm3::Converter(parameters);
	converter.Process();
}