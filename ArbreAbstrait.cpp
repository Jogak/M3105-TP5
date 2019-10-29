#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <vector>
#include <iostream>
#include <typeinfo>
////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(vector<Noeud*> condition, vector<Noeud*> sequence)
    : m_condition(condition), m_sequence(sequence) {
    }

int NoeudInstSi::executer() {
    for(int i = 0; i<m_condition.size();i++){
        if( m_condition[i] != nullptr){
            if(m_condition[i]->executer() == true){
                m_sequence[i]->executer();
                return 0;
            }
        } else{
            m_sequence[i]->executer();
        }
    }
  return 0; // La valeur renvoyée ne représente rien !
}

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
  while(m_condition->executer()) {m_sequence->executer();}
  return 0; // La valeur renvoyée ne représente rien !
}

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer(){
    do {m_sequence->executer();}
    while(!m_condition->executer());
    return 0;
}

NoeudInstPour::NoeudInstPour(Noeud* affectation1, Noeud* expression, Noeud* affectation2, Noeud* sequence)
:m_affectation1(affectation1), m_affectation2(affectation2), m_expression(expression), m_sequence(sequence){
}

int NoeudInstPour::executer(){
   if(m_affectation1 !=NULL) {
       m_affectation1->executer();
   }
    while(m_expression->executer()){
        m_sequence->executer();
        if(m_affectation2 != NULL){
            m_affectation2->executer();
        }
    }
    return 0;
}


NoeudInstLire::NoeudInstLire(vector<Noeud*> var)
: m_var(var){}

int NoeudInstLire::executer(){
    for(auto i : m_var){
        cout << ((SymboleValue*) i)->getChaine() << ":";
        int valeur;
        cin >> valeur;
        ((SymboleValue*)i)->setValeur(valeur);
    }
    return 0;
}

NoeudInstEcrire::NoeudInstEcrire(Noeud* noeudPremierElement, vector<Noeud*> noeudsSupp)
: m_noeud(noeudPremierElement), m_noeudsSupp(noeudsSupp) {
}

int NoeudInstEcrire::executer() {
    Noeud* p;
    p = m_noeud; // on pointe sur le noeud du premier element

    // on regarde si l’objet pointé par p est de type SymboleValue et si c’est une chaîne
    if ((typeid (*p) == typeid (SymboleValue)) && (*((SymboleValue*) p) == "<CHAINE>" )) {
        cout << ((SymboleValue*) p)->getChaine() ; //on affiche la chaine de caractere du symbole value de p
    } else {
        cout << p->executer() ; // on affiche le résultat
    }

    for (int i = 0; i < m_noeudsSupp.size(); i++) {
        cout << " , ";
        p = m_noeudsSupp[i]; // on fait pointer p sur l'élément courant du vecteur
        if ((typeid (*p) == typeid (SymboleValue)) && (*((SymboleValue*) p) == "<CHAINE>" )) {
            cout << ((SymboleValue*) p)->getChaine(); //on affiche la chaîne de caractères
        } else {
            cout << p->executer(); // on affiche le résultat
        }
    }
    return 0;
}
