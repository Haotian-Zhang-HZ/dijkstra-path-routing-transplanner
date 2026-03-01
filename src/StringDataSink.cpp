#include "StringDataSink.h"

const std::string &CStringDataSink::String() const{
    return DString;
}

bool CStringDataSink::Put(const char &ch) noexcept{
    DString += std::string(1,ch);
    // we cannot add a char to a string so we do the casting here
    // constructor: string(size_t count, char ch);
    // it means we are consrtucting a string with the ch and appending it
    return true; //this is a flag to indicate successful write
}

bool CStringDataSink::Write(const std::vector<char> &buf) noexcept{
    DString += std::string(buf.data(),buf.size());
    // constructor: string(const char* s, size_t n);
    // Can't use casting here because buf is a vector not a char*
    return true;
}
