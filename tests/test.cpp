#include <biopp-filer/fasta-parser.h>
#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "biopp/bio_molecular/bio_molecular.h"

TEST(FastaParserTest, title)
{
	std::string file("seq.txt");
    std::string title;
    NucSequence seq;
    FastaParser<NucSequence> fp(file);
	
	fp.load_sequence(title, seq);
	ASSERT_EQ("SEQUENCE_1",title);
	
}

TEST(FastaParserTest, seq)
{
	std::string file("seq.txt");
    std::string title;
    NucSequence seq;
    FastaParser<NucSequence> fp(file);
	
	fp.load_sequence(title, seq);

	ASSERT_EQ("ATCGATCGATCGTCGATCGATCG",seq.getString());
}
