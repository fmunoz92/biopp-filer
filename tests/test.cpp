#include <string>
#include <list>
#include <iostream>
#include <gtest/gtest.h>
#include <biopp/biopp.h>
#include "biopp-filer/bioppFiler.h"

using namespace bioppFiler;

TEST(FastaFormatTest, LoadSave)
{
    const std::string file("LoadSave.txt");

    std::list<biopp::NucSequence> lSeqSaver;
    std::list<biopp::NucSequence> lSeqLoader;

    std::list<std::string> lDesSaver;
    std::list<std::string> lDesLoader;

    /******************Save****************************/

    FastaSaver<biopp::NucSequence> fs(file);

    lSeqSaver.push_back(biopp::NucSequence("ATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCG"));
    lSeqSaver.push_back(biopp::NucSequence("CTCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCG"));
    lSeqSaver.push_back(biopp::NucSequence("TATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCG"));

    lDesSaver.push_back(std::string("sequence 1"));
    lDesSaver.push_back(std::string("sequence 2"));
    lDesSaver.push_back(std::string("sequence 3"));

    std::list<biopp::NucSequence>::iterator itSeq = lSeqSaver.begin();
    std::list<std::string>::iterator itDes = lDesSaver.begin();
    for (; itSeq != lSeqSaver.end(); itSeq++, itDes++)
    {
        fs.saveNextSequence(*itDes, *itSeq);
    }

    /*******************Load**************************/

    FastaParser<biopp::NucSequence> fp(file);

    biopp::NucSequence sequenceLoad;
    std::string titleLoad;
    while (fp.getNextSequence(titleLoad, sequenceLoad))
    {
        lSeqLoader.push_back(sequenceLoad);
        lDesLoader.push_back(titleLoad);
    }

    /*******************Compare************************/
    std::list<biopp::NucSequence>::iterator itSeqSaver  = lSeqSaver.begin();
    std::list<biopp::NucSequence>::iterator itSeqLoader = lSeqLoader.begin();

    std::list<std::string>::iterator        itDesSaver  = lDesSaver.begin();
    std::list<std::string>::iterator        itDesLoader = lDesLoader.begin();

    for (; itSeqSaver != lSeqSaver.end(); itSeqSaver++, itDesSaver++, itSeqLoader++, itDesLoader++)
    {
        ASSERT_EQ(*itSeqSaver, *itSeqLoader);
        ASSERT_EQ(*itDesSaver, *itDesLoader);
    }
}

TEST(FastaFormatTest, LoadSaveWithoutDescription)
{
    const std::string file("LoadSaveWithoutDescription.txt");

    /******************Save****************************/
    const biopp::NucSequence sequenceSave1("ATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCG");
    const biopp::NucSequence sequenceSave2("ATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCG");
    const biopp::NucSequence sequenceSave3("ATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCG");

    const std::string titleSave1("sequence 1");
    const std::string titleSave2("sequence 2");

    FastaSaver<biopp::NucSequence>  fs(file);

    fs.saveNextSequence(titleSave1, sequenceSave1);
    fs.saveNextSequence(titleSave2, sequenceSave2);
    fs.saveNextSequence(sequenceSave3);//without description

    /*******************Load**************************/
    FastaParser<biopp::NucSequence> fp(file);

    biopp::NucSequence sequenceLoad1;
    biopp::NucSequence sequenceLoad2;
    biopp::NucSequence sequenceLoad3;

    std::string titleLoad1;
    std::string titleLoad2;
    std::string titleLoad3;

    fp.getNextSequence(titleLoad1, sequenceLoad1);
    fp.getNextSequence(titleLoad2, sequenceLoad2);
    fp.getNextSequence(titleLoad3, sequenceLoad3);

    /*******************Compare************************/
    ASSERT_EQ(sequenceSave1, sequenceLoad1);
    ASSERT_EQ(titleSave1, titleLoad1);

    ASSERT_EQ(sequenceSave2, sequenceLoad2);
    ASSERT_EQ(titleSave2, titleLoad2);

    ASSERT_EQ(sequenceSave3, sequenceLoad3);
    ASSERT_EQ(std::string(""), titleLoad3);
}

TEST(FastaFormatTest, Reset)
{
    const std::string file("reset.txt");

    /******************Save****************************/
    const biopp::NucSequence sequenceSave1("ATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCG");
    const biopp::NucSequence sequenceSave2("GAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCGTCGATCGAATCGATCG");

    const std::string titleSave1("sequence 1");
    const std::string titleSave2("sequence 2");

    FastaSaver<biopp::NucSequence>  fs(file);

    fs.saveNextSequence(titleSave1, sequenceSave1);
    fs.saveNextSequence(titleSave2, sequenceSave2);
    fs.saveNextSequence(titleSave2, sequenceSave2);

    /*******************Load**************************/
    FastaParser<biopp::NucSequence> fp(file);

    biopp::NucSequence sequenceLoad1;
    biopp::NucSequence sequenceLoad2;
    biopp::NucSequence sequenceLoadAux;

    std::string titleLoad1;
    std::string titleLoad2;
    std::string titleLoadAux;

    while (fp.getNextSequence(titleLoadAux, sequenceLoadAux))
    {}
    fp.reset();

    fp.getNextSequence(titleLoad1, sequenceLoad1);
    fp.getNextSequence(titleLoad2, sequenceLoad2);

    /*******************Compare************************/

    ASSERT_EQ(titleLoad1, titleSave1);
    ASSERT_EQ(sequenceLoad1, sequenceSave1);

    ASSERT_EQ(titleLoad2, titleSave2);
    ASSERT_EQ(sequenceLoad2, sequenceSave2);
}



TEST(FastaFormatTest, Load)
{
    const std::string file("Load.txt");

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

    fp.getNextSequence(title1, seq1);
    fp.getNextSequence(title2, seq2);
    fp.getNextSequence(title3, seq3);

    ASSERT_EQ("AUCGAAUCGAUCGUCG", seq1.getString());
    ASSERT_EQ("SEQUENCE_1", title1);

    ASSERT_EQ("sequence_2", title2);
    ASSERT_EQ("AGGUGAGGUGAGGUG", seq2.getString());

    ASSERT_EQ("AUUG", seq3.getString());
    ASSERT_EQ("", title3);
}
