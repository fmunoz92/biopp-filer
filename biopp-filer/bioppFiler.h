/*
bioppFiler.h: load and save sequences(NucSequence, PseudonucSequence, and AminoSequence)
    Copyright (C) 2012 Facundo Muñoz FuDePAN

    This file is part of Biopp-filer.

    Biopp-filer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Biopp-filer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Biopp-filer.  If not, see <http://www.gnu.org/licenses/>.

    NOTE: This file is in prototype stage, and is under active development.
*/

#ifndef BIOPP_FILER_H
#define BIOPP_FILER_H

#include <mili/mili.h>

namespace bioppFiler
{
struct BioppFilerExceptionHierarchy {};
typedef mili::GenericException<BioppFilerExceptionHierarchy> BioppFilerException;

DEFINE_SPECIFIC_EXCEPTION(FileNotFound, BioppFilerException);
DEFINE_SPECIFIC_EXCEPTION(FileError, BioppFilerException);
DEFINE_SPECIFIC_EXCEPTION(InvalidSequenceError, BioppFilerException);
}

#include "formatFasta/fastaSaver.h"
#include "formatFasta/fastaParser.h"

#endif
