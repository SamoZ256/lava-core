#include "lvcore/filesystem/filesystem.hpp"

namespace lv {

std::string readFile(const char* filename) {
  std::string content;
  std::ifstream file;
  // ensure ifstream objects can throw exceptions:
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // open files
    file.open(filename);
    std::stringstream stream;
    // read file's buffer contents into streams
    stream << file.rdbuf();
    // close file handlers
    file.close();
    // convert stream into string
    content = stream.str();
  }
  catch(std::ifstream::failure e) {
    std::cout << "Error: could not open file '" << filename << "'" << std::endl;
  }

  return content;
}

} //namespace lv
