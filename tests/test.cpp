#include <string>
#include <iostream>
#include <gtest/gtest.h>
#include <biopp/biopp.h>
#include <biopp-filer/bioppFiler.h>

using namespace bioppFiler;

TEST(FastaParserTest, LoadSave)
{
    const std::string file("SaveTest.txt");
    const std::string seqString1("ATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCG");
    const std::string seqString2("ATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCG");
    const std::string seqString3("ATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCG");

    const biopp::NucSequence sequenceSave1 = seqString1;
    const biopp::NucSequence sequenceSave2 = seqString2;
    const biopp::NucSequence sequenceSave3 = seqString2;

    const std::string titleSave1 = "sequence 1";
    const std::string titleSave2 = "sequence 2";

    biopp::NucSequence sequenceLoad1;
    biopp::NucSequence sequenceLoad2;
    biopp::NucSequence sequenceLoad3;

    std::string titleLoad1;
    std::string titleLoad2;
    std::string titleLoad3;

    FastaSaver<biopp::NucSequence>  fs(file);
    FastaParser<biopp::NucSequence> fp(file);

    fs.save_next_sequence(titleSave1, sequenceSave1);
    fs.save_next_sequence(titleSave2, sequenceSave2);
    fs.save_next_sequence(sequenceSave3);

    fp.get_next_sequence(titleLoad1, sequenceLoad1);
    fp.get_next_sequence(titleLoad2, sequenceLoad2);
    fp.get_next_sequence(titleLoad3, sequenceLoad3);

    ASSERT_EQ(sequenceSave1, sequenceLoad1);
    ASSERT_EQ(titleSave1, titleLoad1);

    ASSERT_EQ(sequenceSave2, sequenceLoad2);
    ASSERT_EQ(titleSave2, titleLoad2);

    ASSERT_EQ(sequenceSave3, sequenceLoad3);
    ASSERT_EQ(std::string(""), titleLoad3);
}



TEST(FastaParserTest, Load)
{
    const std::string file("LoadTest.txt");

    std::ofstream of(file.c_str());
    of << ">SEQUENCE_1\nATCGA; comentario2 \n ATCGATCG;comentario3\nTCG\n>sequence_2\nAGGTG\nAGGTG\nAGGTG\n\nATTG\n";
    of.close();
    /* file LoadTest.txt:
     * >SEQUENCE_1
     * ATCGA; comentario2
     *  ATCGATCG;comentario3
     * TCG
     * >sequence_2
     * AGGTG
     * AGGTG
     * AGGTG
     *
     * ATTG";
     */

    biopp::NucSequence seq1;
    std::string title1;
    biopp::NucSequence seq2;
    std::string title2;
    biopp::NucSequence seq3;
    std::string title3;

    FastaParser<biopp::NucSequence> fp(file);

    fp.get_next_sequence(title1, seq1);
    fp.get_next_sequence(title2, seq2);
    fp.get_next_sequence(title3, seq3);

    ASSERT_EQ("ATCGAATCGATCGTCG", seq1.getString());
    ASSERT_EQ("SEQUENCE_1", title1);

    ASSERT_EQ("sequence_2", title2);
    ASSERT_EQ("AGGTGAGGTGAGGTG", seq2.getString());

    ASSERT_EQ("ATTG", seq3.getString());
    ASSERT_EQ("", title3);
}
