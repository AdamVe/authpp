#include "resources.h"

namespace authppgtk {

namespace fs = std::filesystem;

namespace {
    fs::path ui_path = fs::current_path() / "data" / "ui";
}

Resources::Resources() = default;

Resources::~Resources() = default;

fs::path& Resources::get_ui_path()
{
    return ui_path;
}

} // namespace authppgtk