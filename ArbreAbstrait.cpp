#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <vector>
#include <iostream>
#include <typeinfo>
#include <fstream>
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

void NoeudSeqInst::traduitEnCPP(ostream& cout, unsigned int indentation) const{
    for(int i =0; i<m_instructions.size();i++){
        m_instructions[i]->traduitEnCPP(cout,indentation);
    }
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

void NoeudAffectation::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(indentation) << "";
    m_variable->traduitEnCPP(cout,0); 
    cout << " = " ;
    m_expression->traduitEnCPP(cout, 0);
    cout << ";" << endl;
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

void NoeudOperateurBinaire::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(indentation) << "" ;
    m_operandeGauche->traduitEnCPP(cout,0);
    cout << m_operateur.getChaine(); 
    m_operandeDroit->traduitEnCPP(cout,0);
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

void NoeudInstSi::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(indentation) << "" << "if (" ;
    m_condition[0]->traduitEnCPP(cout, 0);
    cout << ") {" << endl;
    m_sequence[0]->traduitEnCPP(cout, indentation+1);
    cout << setw(indentation) << "" << "}" ;
    for(int i = 1; i<m_sequence.size(); i++){
            cout << " else ";
            if( m_sequence.size() == m_condition.size()  || i < m_condition.size()){
                cout << "if (";
                m_condition[i]->traduitEnCPP(cout,0);
                cout << ") ";
            }
            cout << "{" << endl << setw(indentation+1);
            m_sequence[i]->traduitEnCPP(cout, indentation +1);
            cout << setw(indentation) << "" << "}";
    }
    cout << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
  while(m_condition->executer()) {m_sequence->executer();}
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstTantQue::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(indentation) << "" <<"while (" ;
    m_condition->traduitEnCPP(cout,0);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout, indentation + 2);
    cout << setw(indentation)<< "" << "}"<< endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer(){
    do {m_sequence->executer();}
    while(!m_condition->executer());
    return 0;
}

void NoeudInstRepeter::traduitEnCPP(ostream& cout, unsigned int indentation) const{
    cout << setw(indentation)<< "" <<"do {" << endl;
    m_sequence->traduitEnCPP(cout,indentation +2);
    cout << setw(indentation) << "" << "}";
    cout << " while (";
    m_condition->traduitEnCPP(cout,0);
    cout << ");" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

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

void NoeudInstPour::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(indentation) << "" << "for (";
    m_affectation1->traduitEnCPP(cout,0);
    m_expression->traduitEnCPP(cout,indentation+5);
    cout << ";" << endl;
    m_affectation2->traduitEnCPP(cout,indentation + 5);
    cout.seekp(cout.tellp()-(indentation-1),ios::beg);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout, indentation+2); 
    cout << setw(indentation) << "" << "}" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////

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

void NoeudInstLire::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(indentation) << "" << "std::cin >> ";
    m_var[0]->traduitEnCPP(cout,1);
    for(int i = 1; i <m_var.size() ; i++){
        cout << " >> ";
        m_var[i]->traduitEnCPP(cout,1);
    }
    cout << ";" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

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

void NoeudInstEcrire::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(indentation) << "" << "std::cout << ";
    m_noeud->traduitEnCPP(cout,0);
    for(int i = 0; i < m_noeudsSupp.size(); i++){
        cout << " << ";
        m_noeudsSupp[i]->traduitEnCPP(cout, 0);
    }
    cout << ";" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPermut
////////////////////////////////////////////////////////////////////////////////

NoeudPermut::NoeudPermut(Noeud* var1, Noeud* var2):m_var1(var1),m_var2(var2){}


int NoeudPermut::executer(){
    
    int s = ((SymboleValue*)m_var1)->executer();
    ((SymboleValue *)m_var1)->setValeur(((SymboleValue*)m_var2)->executer());
    ((SymboleValue*)m_var2)->setValeur(s);
    
    return 0;
}

void NoeudPermut::traduitEnCPP(ostream& cout, unsigned int indentation) const{
    cout << setw(indentation) << ""  ;
    cout << "s = ";
    m_var1->traduitEnCPP(cout, 0);
    cout << ";" <<endl ;
    m_var1->traduitEnCPP(cout, indentation);
    cout << " = " ;
    m_var2->traduitEnCPP(cout,0);
    cout << ";"<< endl;
    m_var2->traduitEnCPP(cout, indentation);
    cout << " = " << "s ; " << endl;
}
