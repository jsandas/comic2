#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "comic2/resource_loader.hpp"

int main(int argc, char **argv) {
  try {
    std::filesystem::path root = std::filesystem::current_path();
    if (argc > 1) {
      root = argv[1];
    }

    const std::vector<std::string> frpak_files = {
        "FRPAK.001", "FRPAK.002", "FRPAK.003", "FRPAK.004",
        "FRPAK.005", "FRPAK.006", "FRPAK.007",
    };

    std::cout << "Validating FRPAK streams under: " << root.string() << "\n";
    for (const auto &name : frpak_files) {
      const auto file = root / name;
      const auto image = comic2::load_frpak_fullscreen_image(file);
      std::cout << name << ": row_span=0x" << std::hex << image.row_span_bytes
                << std::dec << " plane_bytes=" << image.planes[0].size()
                << "\n";
    }

    return 0;
  } catch (const std::exception &ex) {
    std::cerr << "error: " << ex.what() << "\n";
    return 1;
  }
}
