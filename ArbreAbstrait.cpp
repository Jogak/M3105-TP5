#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <vector>

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

NoeudInstEcrire::NoeudInstEcrire(Noeud* expression1, Noeud* chaine1, Noeud* expression2, Noeud* chaine2)
:m_expression1(expression1), m_chaine1(chaine1), m_expression2(expression2), m_chaine2(chaine2){    
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