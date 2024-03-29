#ifndef INTERPRETEUR_H
#define INTERPRETEUR_H

#include "Symbole.h"
#include "Lecteur.h"
#include "Exceptions.h"
#include "TableSymboles.h"
#include "ArbreAbstrait.h"

class Interpreteur {
public:
	Interpreteur(ifstream & fichier);   // Construit un interpréteur pour interpreter
	                                    //  le programme dans  fichier 
                                      
	void analyse();                     // Si le contenu du fichier est conforme à la grammaire,
	                                    //   cette méthode se termine normalement et affiche un message "Syntaxe correcte".
                                      //   la table des symboles (ts) et l'arbre abstrait (arbre) auront été construits
	                                    // Sinon, une exception sera levée

	inline const TableSymboles & getTable () const  { return m_table;    } // accesseur	
	inline Noeud* getArbre () const { return m_arbre; }                    // accesseur
        void traduitEnCPP(ostream & cout, unsigned int indentation) const;
	inline int getcptSyntaxeErreur() const {return cptSyntaxeErreur;}
        
private:
    Lecteur        m_lecteur;  // Le lecteur de symboles utilisé pour analyser le fichier
    TableSymboles  m_table;    // La table des symboles valués
    Noeud*         m_arbre;    // L'arbre abstrait
    std::vector<Noeud*> vectNoeuds;
    std::vector<SymboleValue*> vectSymbValues;
    const Symbole  chaine; 
    int cptSyntaxeErreur;

    // Implémentation de la grammaire
    Noeud*  programme();   //   <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    Noeud*  seqInst();	   //     <seqInst> ::= <inst> { <inst>  }
    Noeud*  inst();	   //        <inst> ::= <affectation> ; | <instSi> ; | <instTantQue> ; | <instRepeter> ; | <instPour> ; |
                           //       <instLire> ; | <instEcrire> ; | <instPermut>
    Noeud*  affectation(); // <affectation> ::= <variable> = <expression> 
    Noeud*  expression();  //  <expression> ::= <facteur> { <opBinaire> <facteur> }
    Noeud*  expEt();
    Noeud*  expComp();
    Noeud*  expAdd();
    Noeud*  expMult();
    Noeud*  expparent();
    Noeud*  facteur();     //     <facteur> ::= <entier>  |  <variable>  |  - <facteur>  | non <facteur> | ( <expression> )
    Noeud*  instTantQue(); //   <instTantQue> ::= tantque ( <expression> ) <seqInst> fintantque       
                           //   <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
    Noeud*  instSi();      //      <instSi> ::= si ( <expression> ) <seqInst> {sinonsi(<expression) <seqInst>}[sinon <seqInst>]finsi
    Noeud* instRepeter();  //   <instRepeter> ::=repeter <seqInst> jusqua( <expression> )
    Noeud* instPour();     //   <instPour>    ::=pour( [ <affectation> ] ; <expression> ;[ <affectation> ]) <seqInst> finpour

    Noeud* instLire();     //  <instLire> ::= lire ( <variable> { , <variable> } )
    Noeud* instEcrire();    //<instEcrire> ::= ecrire ( <expression> | <chaine> { , <expression> | <chaine> } )
    Noeud* instPermut();    // <instPermut> ::= permut (<variable>,<variable>)

                             // outils pour simplifier l'analyse syntaxique
    void tester (const string & symboleAttendu) const;   // Si symbole courant != symboleAttendu, on lève une exception
    void testerEtAvancer(const string & symboleAttendu); // Si symbole courant != symboleAttendu, on lève une exception, sinon on avance
    void erreur (const string & mess) const;             // Lève une exception "contenant" le message mess
    
};

#endif /* INTERPRETEUR_H */
