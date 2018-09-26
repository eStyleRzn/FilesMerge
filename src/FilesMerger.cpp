#include "FilesMerger.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace FM {

//======================================================================================================================
FilesMerger::FilesMerger(FileNames&& in)
 : fileNames_(std::move(in))
{
  resultFile_.open ("merged.log", std::ios_base::trunc);
}

bool FilesMerger::exec()
{
  if (!resultFile_.is_open())
  {
    std::cerr << "Unable to create the output file" << std::endl;
    return false;
  }

  for (auto&& f : fileNames_)
  {
    if (!readFile(f))
      std::cerr << "Unable to read: " << f << std::endl;
  }

  if (!doMerge())
  {
    std::cerr << "Files merge failed" << std::endl;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool FilesMerger::readFile(const std::string& fileName)
{
  std::ifstream f(fileName);

  if (f.is_open())
  {
    std::string line;
    while (std::getline (f,line))
    {
      if (!addRecord(std::move(line)))
        std::cerr << "Could insert a string from file: " << fileName << std::endl;
    }
    f.close();

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool FilesMerger::addRecord(StringType&& record)
{
  // Insert and check for duplicate
  auto result = mergeResults_.emplace(std::move(record));

  // dubplicate
  if (!result.second)
    return true;

  // Store items reference in the sort container - ite key and the reference
  TimeNumeric key = calcTimeKey(*result.first);
  if (-1 == key)
  {
    std::cerr << "Unable to parse datetime field" << std::endl;
    return false;
  }

  sortItems_.emplace_back(SortItem(key, &(*result.first)));

  std::cout << "Sort item " << sortItems_.back().first << " " << sortItems_.back().second << std::endl;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool FilesMerger::doMerge()
{
  auto lessOp = [](SortBox::value_type itemBefore, SortBox::value_type itemAfter) -> bool
  {
    return (itemBefore.first < itemAfter.first);
  };

  // Sort items for the output result
  std::sort(sortItems_.begin(), sortItems_.end(), lessOp);

  // Pass through the sorted container and write data to the file
  for (auto&& item : sortItems_)
  {
    resultFile_ << *(item.second) << std::endl;
  }

  resultFile_.flush();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
FilesMerger::TimeNumeric FilesMerger::calcTimeKey(const StringType& src) const
{
  return parseTime(src.begin(), src.begin() + timeFieldLength);
}

//----------------------------------------------------------------------------------------------------------------------
FilesMerger::TimeNumeric FilesMerger::parseTime(StringType::const_iterator first, StringType::const_iterator last) const
{
   static const StringType dateTimeFormat{ "%d.%m.%Y %H:%M:%S" };

   // Create a stream which we will use to parse the string,
   // which we provide to constructor of stream to fill the buffer.
   std::basic_stringstream<StringType::value_type> ss( StringType(first, last));

   // Create a tm object to store the parsed date and time.
   std::tm dt;

   // Now we read from buffer using get_time manipulator
   // and formatting the input appropriately.
   ss >> std::get_time(&dt, dateTimeFormat.c_str());

   // Convert the tm structure to time_t value and return.
   return std::mktime(&dt);
}

}

