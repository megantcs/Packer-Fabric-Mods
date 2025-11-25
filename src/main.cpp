#include "core/patcher_core.hpp"
#include "utils/string_utils.h"
#include <iostream>

int execute(int argc, char** argv) {
	patcher_settings settings;
	settings.cache_folder = NULL_OR(get_splits_argument(SET_PROGRAM_ARGUMENTS, "cache_folder", '='), "cache");
	settings.output_prefix = NULL_OR(get_splits_argument(SET_PROGRAM_ARGUMENTS, "output_prefix", '='), ".patch.jar");
	settings.remove_cache = has_argument(SET_PROGRAM_ARGUMENTS, "--cleanup");
	settings.debug = has_argument(SET_PROGRAM_ARGUMENTS, "--debug");
	settings.force = has_argument(SET_PROGRAM_ARGUMENTS, "--force");

	auto files = get_array_splits_args(SET_PROGRAM_ARGUMENTS, "jar", '=');
	auto input = get_splits_argument(SET_PROGRAM_ARGUMENTS, "input", '=');

	if (files.empty() || input.empty()) {
		printf("use [files=file] [input=file] [option]\n");
		return 2;
	}

	if (!io::exist_path(input.c_str())) {
		printf("input not found: %s\n", input.c_str());
		return 3;
	}

	patcher_core core(settings);

	for (auto file : files) {
		if (io::exist_path(file.c_str())) {
			core.add_file(file);
		}
		else {
			printf("missing file: %s\n", file.c_str());
		}
	}
	core.get_logger().output_log.add_event([](const std::string& msg) {	printf("[log] %s\n", msg.c_str()); });
	core.patch(input);
}

int main(int argc, char** argv)
{
	return execute(argc, argv);
}