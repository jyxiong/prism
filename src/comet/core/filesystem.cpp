#include "comet/core/filesystem.h"

namespace comet
{
    std::string read_file(const std::string &filepath, bool binary)
    {
        std::string result;
        std::ifstream file(filepath, binary ? std::ios::binary : std::ios_base::openmode());

        if (!file.is_open())
        {
            return result;
        }

        result.resize(file.tellg());
        file.seekg(0, std::ios::beg);

        result.assign((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());

        return result;
    }
} // namespace comet