#ifndef FILESMERGER_H
#define FILESMERGER_H

#include <string>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <memory>
#include <ctime>

namespace FM
{
//======================================================================================================================
// The class is intended to merge serveral input log files into the signle resultFile_ with the records sorted by datetime
class FilesMerger
{
  using FileNames = std::vector<std::string>;
  using StringType = std::string;
  using MergeItemsBox = std::unordered_set<StringType>;

  using TimeNumeric = time_t;
  using SortItem = std::pair<TimeNumeric, MergeItemsBox::const_pointer>;
  using SortBox = std::vector<SortItem>;

public:
  FilesMerger(FileNames&&);
  virtual ~FilesMerger() {}

  bool exec();

private:
  // Disable copying
  FilesMerger(const FilesMerger&) = delete;
  FilesMerger& operator=(const FilesMerger&) = delete;

  bool readFile(const std::string&);
  bool addRecord(StringType&&);
  bool doMerge();

  TimeNumeric calcTimeKey(const StringType&) const;
  TimeNumeric parseTime(StringType::const_iterator first, StringType::const_iterator last) const;

  static const uint8_t timeFieldLength = 19;

  MergeItemsBox mergeResults_;
  SortBox sortItems_;
  std::ofstream resultFile_;

  FileNames fileNames_;
};

}

#endif // FILESMERGER_H
