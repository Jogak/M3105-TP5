/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   TestTantQue.h
 * Author: kaluznyg
 *
 * Created on Nov 6, 2019, 10:06:38 AM
 */

#ifndef TESTTANTQUE_H
#define TESTTANTQUE_H

#include <cppunit/extensions/HelperMacros.h>

class TestTantQue : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(TestTantQue);

    CPPUNIT_TEST(testAnalyse);

    CPPUNIT_TEST_SUITE_END();

public:
    TestTantQue();
    virtual ~TestTantQue();
    void setUp();
    void tearDown();

private:
    void testAnalyse();

};

#endif /* TESTTANTQUE_H */

