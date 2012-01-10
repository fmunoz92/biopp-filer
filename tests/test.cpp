#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include "biopp/bio_molecular/bio_molecular.h"
#include <biopp-filer/fasta-parser.h>

TEST(FastaParserTest, LoadSave)
{
    const std::string file("SaveTest.txt");
    const std::string seqString("ATCGAATCGATCGTCG");

    const NucSequence sequenceSave = "ATCGAATCGATCGTCG";
    const std::string titleSave = "sequence 1";

    NucSequence sequenceLoad;
    std::string titleLoad;

    FastaParser<NucSequence> fp(file);
    fp.save_sequence(titleSave, sequenceSave);
    fp.load_sequence(titleLoad, sequenceLoad);

    ASSERT_EQ(titleSave, titleLoad);
    ASSERT_EQ(sequenceSave, sequenceLoad);
}

void makeFileTest(std::string file_name)
{
    std::ofstream of(file_name.c_str());
    of << " ;LineaVacia1 \n >SEQUENCE_1;comentario1 \nATCGA; comentario2 \n ;lineaVacia2\nATCGATCG    ;comentario3\nTCG";
    of.close();
}

TEST(FastaParserTest, Load)
{
    const std::string file("LoadTest.txt");
    NucSequence seq;
    std::string title;
    makeFileTest(file);
    FastaParser<NucSequence> fp(file);

    fp.load_sequence(title, seq);

    ASSERT_EQ("SEQUENCE_1", title);
    ASSERT_EQ("ATCGAATCGATCGTCG", seq.getString());
}
