#ifndef PATCHER_FILE_HPP
#define PATCHER_FILE_HPP

#include <string>
#include <vector>

struct patcher_file 
{
	std::string input;
	patcher_file(const std::string &input);

	operator std::string() const {
		return input;
	}

	static patcher_file& none();
};

typedef std::vector<patcher_file> patcher_array_files;

inline patcher_file::patcher_file(const std::string& _input) : input(_input) { }

inline patcher_file& patcher_file::none()
{
	static patcher_file _("(null)");
	return _;
}

#endif

