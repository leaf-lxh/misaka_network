#include <string>

class FileIO
{
    public:
        FileIO() = default;

        size_t ReadFile(const std::string &path, std::string &buffer);
        size_t WriteFile(const std::string &path, std::string &content);
};