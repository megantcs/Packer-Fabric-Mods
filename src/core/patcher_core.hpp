#ifndef PATCHER_CORE_HPP
#define PATCHER_CORE_HPP

#include <functional>
#include "patcher_settings.hpp"
#include "patcher_file.hpp"

namespace io {
	bool exist_path(const char* path);
	bool exist_or_create_directory(const char* path);
	bool copy_file(const std::string& source, const std::string& destination, bool overwrite = true);
	bool create_parent_dirs(const std::string& filepath);
	bool create_zip_from_folder(const std::string& folderPath, const std::string& zipPath);
	bool extract_zip(const std::string& zipPath, const std::string& outputDir);
	bool remove_directory(const std::string& path);

	const char* extract_name(const char* path);
}

namespace utils 
{
	bool add_jars_to_configs(const std::string& configPath, const std::vector<std::string>& jarPaths);
}

template <class T>
class Logger {
    template <class Ty>
    struct OutputThread {
        template <class TyEvent>
        using handler = std::function<void(const TyEvent& msg)>;

        void add_event(const handler<Ty>& msg);
        void remove_event(const handler<Ty>& msg);

        void call(const Ty& msg) const;
        void call(Ty&& msg) const;

        OutputThread<Ty>& operator<<(const Ty& msg);
        OutputThread<Ty>& operator<<(Ty&& msg);

    private:
        std::vector<handler<Ty>> handlers;
    };

public:
    OutputThread<T> output_log;
    OutputThread<T> output_error;
    OutputThread<T> output_warning;
};

class patcher_core
{
    patcher_settings settings;
    patcher_array_files files;
    typedef Logger<std::string> patcher_logger;

    patcher_logger logger;
public:
    template<class TSettings>
    patcher_core(TSettings&& settings);

    patcher_file    add_file(const std::string& path);
    patcher_logger& get_logger();

    bool remove_file(const std::string& path);
    bool patch(const std::string& input);

    void create_patched_archive(const std::string& cache_folder,
        const std::string& name_input) const;

    void cleanup_cache(const std::string& cache_folder) const;

    void update_mod_configuration(const std::string& cache_folder, 
        const std::vector<std::string>& jar_paths) const;

    std::vector<std::string> copy_files_to_meta_inf(const std::string& cache_folder) const;
    void prepare_meta_inf_structure(const std::string& cache_folder) const;

    std::string prepare_cache_directory(const std::string& input, 
        const std::string& name_input) const;

    void debug(const std::string& msg) const; 
};

template<class TSettings>
inline patcher_core::patcher_core(TSettings&& settings) : settings(std::forward<TSettings>(settings)) {}

template <class T>
template <class Ty>
void Logger<T>::OutputThread<Ty>::add_event(const handler<Ty>& msg_handler) {
    handlers.push_back(msg_handler);
}

template <class T>
template <class Ty>
void Logger<T>::OutputThread<Ty>::remove_event(const handler<Ty>& msg_handler) {
    auto it = std::remove_if(handlers.begin(), handlers.end(),
        [&msg_handler](const handler<Ty>& h) {
            return h.target_type() == msg_handler.target_type();
        });
    handlers.erase(it, handlers.end());
}

template <class T>
template <class Ty>
void Logger<T>::OutputThread<Ty>::call(const Ty& msg) const{
    for (const auto& handler : handlers) {
        if (handler) handler(msg);
    }
}

template <class T>
template <class Ty>
void Logger<T>::OutputThread<Ty>::call(Ty&& msg) const {
    for (const auto& handler : handlers) {
        if (handler) handler(msg);
    }
}

template <class T>
template <class Ty>
Logger<T>::OutputThread<Ty>& Logger<T>::OutputThread<Ty>::operator<<(const Ty& msg) {
    call(msg);
    return *this;
}

template <class T>
template <class Ty>
Logger<T>::OutputThread<Ty>& Logger<T>::OutputThread<Ty>::operator<<(Ty&& msg) {
    call(std::move(msg));
    return *this;
}


#endif


