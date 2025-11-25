#ifndef PATCHER_SETTINGS_HPP
#define PATCHER_SETTINGS_HPP

#include <string>

struct patcher_settings 
{
	std::string cache_folder;
	std::string output_prefix;
	
	bool force;
	bool remove_cache;
	bool debug;

	static patcher_settings& empty();
};

inline patcher_settings& patcher_settings::empty()
{
	static patcher_settings _{"cache", ".patcher.jar", true, true, false};
	return _;
}

#endif