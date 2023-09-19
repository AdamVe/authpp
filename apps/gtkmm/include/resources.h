#pragma once

#include <filesystem>

namespace authppgtk {

class Resources {
public:
    Resources();

    virtual ~Resources();

    [[nodiscard]] static std::filesystem::path& get_ui_path();
};

} // namespace authppgtk