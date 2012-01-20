#ifndef BIOPP_FILER_EXCEPTION
#define BIOPP_FILER_EXCEPTION

#include <mili/mili.h>

struct BioppFilerExceptionHierarchy {};
typedef GenericException<BioppFilerExceptionHierarchy> BioppFilerException;

DEFINE_SPECIFIC_EXCEPTION(FileNotFound, BioppFilerException);
DEFINE_SPECIFIC_EXCEPTION(FileError, BioppFilerException);


#endif
