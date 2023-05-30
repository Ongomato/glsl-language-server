#include "includer.hpp"

#include <filesystem>
#include "utils.hpp"

namespace fs = std::filesystem;

using IncludeResult = FileIncluder::IncludeResult;

void FileIncluder::releaseInclude(IncludeResult* result) {
    delete result;
}

IncludeResult* FileIncluder::includeLocal(
        const char* header_name,
        const char* includer_name,
        size_t depth)
{
    auto suffix = strip_prefix("file://", includer_name);
    if (!suffix) return nullptr;

    fs::path path = suffix;
    path.replace_filename(header_name);
    path = fs::absolute(path);

    std::string uri = "file://";
    uri += path.string();

    auto& documents = this->workspace->documents();

    auto existing = documents.find(uri);
    if (existing == documents.end()) {
        // load the file
        if (auto contents = read_file_to_string(&path.string()[0])) {
            documents[uri] = *contents;
            existing = documents.find(uri);
        } else {
            return nullptr;
        }
    }

    const std::string& contents = existing->second;
    return new IncludeResult{uri, contents.c_str(), contents.size(), nullptr};
}
