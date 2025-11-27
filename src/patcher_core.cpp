#define _CRT_SECURE_NO_WARNINGS

#include "core/patcher_core.hpp"
#include <filesystem>
#include <json.hpp>
#include <fstream>
#include <miniz.h>
#include "utils/utils.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

bool io::exist_path(const char* path)
{
	return std::filesystem::exists(path);
}

bool io::exist_or_create_directory(const char* path)
{
	if (exist_path(path)) return false;
    DEBUG_ASSERT(path);
    DEBUG_ASSERT(path[0] != '\0');

    try {
        std::filesystem::create_directory(path);
    }
    catch (std::exception &ex) {
        printf("[exception-error] %s\n", ex.what());
        abort();
    }
	return true;
}

bool io::copy_file(const std::string& source, const std::string& destination, bool overwrite)
{
    try {
        if (!fs::exists(source)) return false;
        if (!fs::is_regular_file(source)) return false;
        if (fs::exists(destination) && !overwrite) return false;
        

        fs::path dest_path(destination);
        fs::create_directories(dest_path.parent_path());

        fs::copy_file(source, destination, overwrite ? fs::copy_options::overwrite_existing : fs::copy_options::none);

        return true;
    }
    catch (const fs::filesystem_error& e) {
        return false;
    }
    catch (const std::exception& e) {
        return false;
    }
}

bool io::create_parent_dirs(const std::string& filepath)
{
    std::filesystem::path path(filepath);
    auto parent_path = path.parent_path();

    if (!parent_path.empty() && !std::filesystem::exists(parent_path)) {
        return std::filesystem::create_directories(parent_path);
    }
    return true;
}

bool io::create_zip_from_folder(const std::string& folderPath, const std::string& zipPath)
{
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));

    if (!mz_zip_writer_init_file(&zip_archive, zipPath.c_str(), 0)) return false;
    

    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        mz_zip_writer_end(&zip_archive);
        return false;
    }

    bool success = true;

    for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();
            std::string relativePath = fs::relative(entry.path(), folderPath).string();

            std::replace(relativePath.begin(), relativePath.end(), '\\', '/');


            FILE* file = fopen(filePath.c_str(), "rb");
            if (!file) {
                success = false;
                continue;
            }

            fseek(file, 0, SEEK_END);
            size_t file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            std::vector<char> file_data(file_size);
            if (fread(file_data.data(), 1, file_size, file) != file_size) {
                fclose(file);
                success = false;
                continue;
            }
            fclose(file);

            if (!mz_zip_writer_add_mem(&zip_archive, relativePath.c_str(),
                file_data.data(), file_data.size(),
                MZ_DEFAULT_COMPRESSION)) {
                success = false;
            }
        }
    }

    if (!mz_zip_writer_finalize_archive(&zip_archive)) {
        success = false;
    }

    mz_zip_writer_end(&zip_archive);
    return success;
}

bool patcher_core::remove_file(const std::string& path)
{
    auto initial_size = files.size();
    std::erase_if(files, [&path](const patcher_file& file) {
        return file.input == path;
        });
    return files.size() < initial_size;
}

patcher_core::patcher_logger&
patcher_core::get_logger()
{
    return logger;
}

patcher_file patcher_core::add_file(const std::string& path)
{
    if (!io::exist_path(path.c_str()))
        return patcher_file::none();

    files.emplace_back(path);
    return files.back();
}

bool io::extract_zip(const std::string& zipPath, const std::string& outputDir)
{
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }

    if (!mz_zip_reader_init_file(&zip_archive, zipPath.c_str(), 0)) return false;
    

    mz_uint num_files = mz_zip_reader_get_num_files(&zip_archive);

    bool success = true;

    for (mz_uint i = 0; i < num_files; i++) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            success = false;
            continue;
        }

        std::string filename = file_stat.m_filename;
        std::string outputPath = outputDir + "/" + filename;

        if (!io::create_parent_dirs(outputPath)) {
            success = false;
            continue;
        }

        if (file_stat.m_is_directory) continue;
        if (!mz_zip_reader_extract_to_file(&zip_archive, i, outputPath.c_str(), 0)) success = false;
    }

    mz_zip_reader_end(&zip_archive);

    return success;
}

bool io::remove_directory(const std::string& path)
{
    if (!fs::exists(path)) return true; 
    if (!fs::is_directory(path)) return false;
   
    std::uintmax_t removed_count = fs::remove_all(path);

    return true;
}

const char* io::extract_name(const char* path)
{
    if (!path || *path == '\0') {
        return "";
    }
    const char* last_slash = strrchr(path, '/');
    const char* last_backslash = strrchr(path, '\\');

    const char* last_separator = (last_slash > last_backslash) ? last_slash : last_backslash;

    if (last_separator) {
        return last_separator + 1;
    }

    return path;
}

bool utils::add_jars_to_configs(const std::string& configPath, const std::vector<std::string>& jarPaths)
{
    try {
        std::ifstream configFile(configPath);
        if (!configFile.is_open()) {
            return false;
        }

        json config;
        configFile >> config;
        configFile.close();

        if (!config.contains("jars")) {
            config["jars"] = json::array();
        }
        else if (!config["jars"].is_array()) {
            config["jars"] = json::array();
        }

        int addedCount = 0;
        auto& jarsArray = config["jars"];

        for (const auto& jarPath : jarPaths) {
            bool jarExists = false;
            for (const auto& existingJar : jarsArray) {
                if (existingJar.contains("file") && existingJar["file"] == jarPath) {
                    jarExists = true;
                    break;
                }
            }

            if (!jarExists) {
                json jarObject;
                jarObject["file"] = jarPath;
                jarsArray.push_back(jarObject);
                addedCount++;
            }
        }

        if (addedCount == 0) {
            return true;
        }

        std::ofstream outputFile(configPath);
        if (!outputFile.is_open()) {
            return false;
        }

        outputFile << config.dump(2);
        outputFile.close();

        return true;

    }
    catch (const std::exception& e) {
        return false;
    }
}

bool patcher_core::patch(const std::string& input)
{
    using namespace io;

    debug("Starting patching process for: " + input);

    const std::string name_input = extract_name(input.c_str());
    debug("Extracted name: " + name_input);

    const std::string cache_folder = prepare_cache_directory(input, name_input);
    debug("Cache folder: " + cache_folder);

    debug("Extracting ZIP archive...");
    extract_zip(input, cache_folder);

    debug("Preparing META-INF structure...");
    prepare_meta_inf_structure(cache_folder);

    debug("Copying files to META-INF...");
    const std::vector<std::string> jar_paths = copy_files_to_meta_inf(cache_folder);
    debug("Copied " + std::to_string(jar_paths.size()) + " files to META-INF");

    debug("Updating mod configuration...");
    update_mod_configuration(cache_folder, jar_paths);

    debug("Creating patched archive...");
    create_patched_archive(cache_folder, name_input);

    debug("Cleaning up cache...");
    cleanup_cache(cache_folder);

    debug("Patching completed successfully for: " + input);
    return true;
}

std::string patcher_core::prepare_cache_directory(const std::string& input, const std::string& name_input) const
{
    debug("Preparing cache directory...");
    io::exist_or_create_directory(settings.cache_folder.c_str());
    std::string cache_path = settings.cache_folder + "\\" + name_input;
    debug("Cache path created: " + cache_path);
    return cache_path;
}

void patcher_core::prepare_meta_inf_structure(const std::string& cache_folder) const
{
    using namespace io;
    debug("Creating META-INF structure in: " + cache_folder);
    const std::string meta_inf_jars = cache_folder + "\\META-INF\\jars";
    exist_or_create_directory(meta_inf_jars.c_str());
    debug("META-INF/jars directory created: " + meta_inf_jars);
}

std::vector<std::string> patcher_core::copy_files_to_meta_inf(const std::string& cache_folder) const
{
    using namespace io;
    debug("Starting to copy " + std::to_string(files.size()) + " files to META-INF");
    std::vector<std::string> jar_paths;
    jar_paths.reserve(files.size());

    for (const auto& file : files) {
        const std::string jar_name = extract_name(file.input.c_str());
        const std::string dest_path = cache_folder + "\\META-INF\\jars\\" + jar_name;

        debug("Copying file: " + file.input + " to " + dest_path);
        copy_file(file.input, dest_path, settings.force);

        std::string relative_path = "META-INF/jars/" + jar_name;
        jar_paths.push_back(relative_path);
        debug("Added to jar paths: " + relative_path);
    }

    debug("Finished copying files. Total jar paths: " + std::to_string(jar_paths.size()));
    return jar_paths;
}

void patcher_core::update_mod_configuration(const std::string& cache_folder,
    const std::vector<std::string>& jar_paths) const
{
    const std::string config_path = cache_folder + "\\fabric.mod.json";
    debug("Updating mod configuration at: " + config_path);
    debug("Adding " + std::to_string(jar_paths.size()) + " jars to configuration");

    utils::add_jars_to_configs(config_path, jar_paths);
    debug("Mod configuration updated successfully");
}

void patcher_core::create_patched_archive(const std::string& cache_folder,
    const std::string& name_input) const
{
    using namespace io;
    const std::string output_name = settings.output + settings.output_prefix;
    debug("Creating patched archive: " + output_name + " from folder: " + cache_folder);

    create_zip_from_folder(cache_folder, output_name);
    debug("Patched archive created successfully: " + output_name);
}

void patcher_core::cleanup_cache(const std::string& cache_folder) const
{
    using namespace io;
    if (settings.remove_cache) {
        remove_directory(cache_folder);
        debug("Cache directory removed: " + cache_folder);
    }
    else {
        debug("Cache preservation enabled. Cache directory kept: " + cache_folder);
    }
}

void patcher_core::debug(const std::string& msg) const
{
    if (!settings.debug) return;

    logger.output_log.call(msg);
}
