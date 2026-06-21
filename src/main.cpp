#include <exception>
#include <filesystem>
#include <iostream>

#include "comic2/bootstrap.hpp"

int main(int argc, char **argv) {
  try {
    const std::filesystem::path root = (argc > 1)
                                           ? std::filesystem::path(argv[1])
                                           : std::filesystem::current_path();
    return comic2::run_bootstrap_entry(root);
  } catch (const std::exception &ex) {
    std::cerr << "error: " << ex.what() << "\n";
    return 1;
  }
}
