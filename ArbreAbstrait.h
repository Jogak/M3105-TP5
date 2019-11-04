#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
  public:
    virtual int  executer() =0 ; // Méthode pure (non implémentée) qui rend la classe abstraite
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual ~Noeud() {} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
    virtual void traduitEnCPP(ostream & cout, unsigned int indentation) const = 0; // Traduit l'instruction en langage C++ sur un flux de sortie
};

////////////////////////////////////////////////////////////////////////////////
class NoeudSeqInst : public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
  public:
     NoeudSeqInst();         // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {}       // A cause du destructeur virtuel de la classe Noeud
    int executer() override; // Exécute chaque instruction de la séquence
    void ajoute(Noeud* instruction) override;  // Ajoute une instruction à la séquence
    void traduitEnCPP(ostream & cout, unsigned int indentation) const override; // Traduit l'instruction en langage C++ sur un flux de sortie
  private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////
class NoeudAffectation : public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    ~NoeudAffectation() {}   // A cause du destructeur virtuel de la classe Noeud
    int executer() override; // Exécute (évalue) l'expression et affecte sa valeur à la variable
    void traduitEnCPP(ostream & cout, unsigned int indentation) const override; // Traduit l'instruction en langage C++ sur un flux de sortie
  private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudOperateurBinaire : public Noeud {
// Classe pour représenter un noeud "opération binaire" composé d'un opérateur
//  et de 2 fils : l'opérande gauche et l'opérande droit
  public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
   ~NoeudOperateurBinaire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer() override;   // Exécute (évalue) l'opération binaire)
    void traduitEnCPP(ostream & cout, unsigned int indentation) const override; // Traduit l'instruction en langage C++ sur un flux de sortie
  private:
    Symbole m_operateur;
    Noeud*  m_operandeGauche;
    Noeud*  m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSi : public Noeud {
// Classe pour représenter un noeud "instruction si riche"
//  et ses conditions et instructions associés
  public:
    NoeudInstSi(vector<Noeud*> condition, vector<Noeud*> sequence);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSi() {}         // A cause du destructeur virtuel de la classe Noeud
    int executer() override; // Exécute l'instruction si : si condition vraie on exécute la séquence
    void traduitEnCPP(ostream & cout, unsigned int indentation) const override; // Traduit l'instruction en langage C++ sur un flux de sortie
  private:
    vector<Noeud*>  m_condition;
    vector<Noeud*>  m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstTantQue : public Noeud {
// Classe pour représenter un noeud "instruction tant que"
//  et ses 2 fils : la condition du tant que et la séquence d'instruction associée
  public:
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
     // Construit une "instruction tant que" avec sa condition et sa séquence d'instruction
   ~NoeudInstTantQue() {}         // A cause du destructeur virtuel de la classe Noeud
    int executer() override; // Exécute l'instruction tant que : tant que la condition est vraie, on execute la sequence d'instruction
    void traduitEnCPP(ostream & cout, unsigned int indentation) const override; // Traduit l'instruction en langage C++ sur un flux de sortie
  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstRepeter : public Noeud {
// Classe pour représenter un noeud "repeter"
// et ses 2 fils : la condition du repeter et la séquence d'instruction associée
public:
    NoeudInstRepeter(Noeud* condition, Noeud* sequence);
        // Construit une "instruction repeter" avec sa condition et sa séquence d'instructions
    ~NoeudInstRepeter() {}  // A cause du destructeur virtuel de la classe Noeud
    int executer() override;    // Execute l'instruction tant que la condition est vrai
    void traduitEnCPP(ostream & cout, unsigned int indentation) const override; // Traduit l'instruction en langage C++ sur un flux de sortie
private:
    Noeud* m_condition;
    Noeud* m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstPour : public Noeud {
// Classe pour représenter un noeud "pour"
// et ses fils : 2 affectation (non obligatoire), la condition et la séquence d'instruction associé
public:
    NoeudInstPour(Noeud* affectation1, Noeud* expression, Noeud* affectation2, Noeud* sequence);
        // Construit une "instruction pour" avec ses éventuels affectation, sa condition et sa séquence d'instruction
    ~NoeudInstPour() {} // A cause du destructeur virtuel de la classe Noeud
    int executer() override;    // Execute l'instruction tant que la condition est vrai
    void traduitEnCPP(ostream & cout, unsigned int indentation) const override; // Traduit l'instruction en langage C++ sur un flux de sortie
private:
    Noeud* m_affectation1;
    Noeud* m_affectation2;
    Noeud* m_expression;
    Noeud* m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstLire : public Noeud {
    //Classe pour représenter une "instruction lire"
    // et ses fils, le nombre de variable à lire
public:
    NoeudInstLire(vector<Noeud*> var);
        // Construit une "instruction lire" avec ses différentes variables
    ~NoeudInstLire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer() override;    // Lis une nouvelle variable tant qu'elles ne sont pas toutes lues
    void traduitEnCPP(ostream & cout, unsigned int indentation) const override; // Traduit l'instruction en langage C++ sur un flux de sortie
private:
    vector<Noeud*> m_var;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstEcrire : public Noeud {
// Classe pour représenter un noeud "instruction ecrire"
//  et ses 2 fils : un noeud , et vecteur de noeud supplémentaires
  public:
    NoeudInstEcrire(Noeud* noeudPremierElement, vector<Noeud*> noeudsSupp);
     // Construit une instruction ecrire avec les vector de noeuds mis en paramètre.
   ~NoeudInstEcrire() {}; // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction ecrire : ecrire ( <expression> | <chaine> puis potentiellement d'autres
    void traduitEnCPP(ostream & cout, unsigned int indentation) const override; // Traduit l'instruction en langage C++ sur un flux de sortie
  private:
      Noeud* m_noeud;
      vector<Noeud*> m_noeudsSupp;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudPermut : public Noeud {
    // Classe pour représenter un noeud "permut"
    // et ses 2 fils : 2 variables qu'il faut permuter
public:
    NoeudPermut(Noeud* var1, Noeud* var2);
        // Construit une "instruction permut" avec ses deux variables
    ~NoeudPermut() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Permute la valeur des deux variables
    void traduitEnCPP(ostream & cout, unsigned int indentation) const override; // Traduit l'instruction en langage C++ sur un flux de sortie
private:
    Noeud* m_var1;
    Noeud* m_var2;
};

#endif /* ARBREABSTRAIT_H */
