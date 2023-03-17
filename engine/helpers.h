#pragma once

#include <fstream>
#include <stdexcept>
#include <vector>
#include "path_config.h"

namespace help {

static std::vector<char> readResource(const std::string& filename) {
    std::string absolute = RESOURCE_DIR + filename;
    std::ifstream file(absolute, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file: " + absolute);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

}