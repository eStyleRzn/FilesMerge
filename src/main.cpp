#include <iostream>
#include <vector>
#include <string>

#include "FilesMerger.h"

using namespace FM;

int main(int argc, char *argv[])
{
  // Parse command line args into srings vector
  std::vector<std::string> args(argv+1, argv+argc);

  if (!args.size())
  {
    std::cerr << "No merge files were set in command line" << std::endl;
    return EXIT_FAILURE;
  }

  FilesMerger merger(std::forward<std::vector<std::string>>(args));

  if (merger.exec())
    return EXIT_SUCCESS;
}
