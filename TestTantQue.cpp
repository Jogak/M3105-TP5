/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   TestTantQue.cpp
 * Author: kaluznyg
 *
 * Created on Nov 6, 2019, 10:06:39 AM
 */

#include "TestTantQue.h"
#include "Interpreteur.h"
#include<fstream>


CPPUNIT_TEST_SUITE_REGISTRATION(TestTantQue);

TestTantQue::TestTantQue() {
}

TestTantQue::~TestTantQue() {
}

void TestTantQue::setUp() {
}

void TestTantQue::tearDown() {
}

void TestTantQue::testAnalyse() {
    ifstream f1("TestUTantQue.txt");
    Interpreteur interpreteur1(f1);
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Analyse correcte", interpreteur1.analyse());
    ifstream f2("TestUTantQueErreur.txt");
    Interpreteur interpreteur2(f2);
    CPPUNIT_ASSERT_THROW_MESSAGE("Analyse incorrecte, exception levée", interpreteur2.analyse(), SyntaxeException);
}

