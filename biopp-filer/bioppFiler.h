#ifndef BIOPP_FILER_H
#define BIOPP_FILER_H

#include <mili/mili.h>

namespace bioppFiler
{
struct BioppFilerExceptionHierarchy {};
typedef GenericException<BioppFilerExceptionHierarchy> BioppFilerException;

DEFINE_SPECIFIC_EXCEPTION(FileNotFound, BioppFilerException);
DEFINE_SPECIFIC_EXCEPTION(FileError, BioppFilerException);
}

#include "formatFasta/fastaSaver.h"
#include "formatFasta/fastaParser.h"

#endif
