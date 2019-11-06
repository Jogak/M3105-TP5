/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TestTantQue.cpp
 * Author: kaluznyg
 * 
 * Created on Nov 4, 2019, 10:52:51 AM
 */

// CppUnit site http://sourceforge.net/projects/cppunit/files

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <cppunit/portability/Stream.h>
#include <iostream>
using namespace std;
#include "../Interpreteur.h"
#include "../Exceptions.h"

using namespace std;

class ProgressListener : public CPPUNIT_NS::TestListener {
public:

    ProgressListener()
    : m_lastTestFailed(false) {
    }

    ~ProgressListener() {
    }

    void startTest(CPPUNIT_NS::Test *test) {
        CPPUNIT_NS::stdCOut() << test->getName();
        CPPUNIT_NS::stdCOut() << "\n";
        CPPUNIT_NS::stdCOut().flush();

        m_lastTestFailed = false;
    }

    void addFailure(const CPPUNIT_NS::TestFailure &failure) {
        CPPUNIT_NS::stdCOut() << " : " << (failure.isError() ? "error" : "assertion");
        m_lastTestFailed = true;
    }

    void endTest(CPPUNIT_NS::Test *test) {
        if (!m_lastTestFailed)
            CPPUNIT_NS::stdCOut() << " : OK";
        CPPUNIT_NS::stdCOut() << "\n";
    }

private:
    /// Prevents the use of the copy constructor.
    ProgressListener(const ProgressListener &copy);

    /// Prevents the use of the copy operator.
    void operator=(const ProgressListener &copy);

private:
    bool m_lastTestFailed;
};

int main() {
    // Create the event manager and test controller
    ifstream fichier("../testInstTantQue.txt");
    try {
        if (fichier.fail()) throw FichierException();
        Interpreteur interpreteur(fichier);
        interpreteur.analyse();
        // Si pas d'exception levée, l'analyse syntaxique a réussi
        
        interpreteur.getArbre()->executer();
        interpreteur.getTable().chercheAjoute((Symbole)"i");
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Variable i",3,intepreteur.getTable().chercheAjoute("i"));
    } catch (InterpreteurException & e) {
        cout << e.what() << endl;
    }
    return 0;
}
