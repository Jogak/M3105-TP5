#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr), cptSyntaxeErreur(0) {
}

void Interpreteur::analyse() {
  m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) const {
  // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
  static char messageWhat[256];
  if (m_lecteur.getSymbole() != symboleAttendu) {
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(),
            symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
  }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) {
  // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
  tester(symboleAttendu);
  m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const {
  // Lève une exception contenant le message et le symbole courant trouvé
  // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
  static char messageWhat[256];
  sprintf(messageWhat,
          "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
          m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
  throw SyntaxeException(messageWhat);
}

Noeud* Interpreteur::programme() {
  // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
  try{testerEtAvancer("procedure");
  testerEtAvancer("principale");
  testerEtAvancer("(");
  testerEtAvancer(")");
  Noeud* sequence = seqInst();
  testerEtAvancer("finproc");
  tester("<FINDEFICHIER>");
  return sequence;
  } catch(SyntaxeException &e){
      throw;
  }
}

Noeud* Interpreteur::seqInst() {
  // <seqInst> ::= <inst> { <inst> }
  try { NoeudSeqInst* sequence = new NoeudSeqInst();
  do {
    sequence->ajoute(inst());
  } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si"
          || m_lecteur.getSymbole() == "tantque" 
          || m_lecteur.getSymbole() == "repeter"  || m_lecteur.getSymbole() == "pour"
          || m_lecteur.getSymbole() == "ecrire"   || m_lecteur.getSymbole() == "lire" 
          || m_lecteur.getSymbole() == "permut"
          );
  // Tant que le symbole courant est un début possible d'instruction...
  // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
  return sequence;
  } catch(SyntaxeException &e){
      throw;
  }
}

Noeud* Interpreteur::inst() {
  // <inst> ::= <affectation>  ; | <instSi>
    try{
      if (m_lecteur.getSymbole() == "<VARIABLE>") {
        Noeud *affect = affectation();
        testerEtAvancer(";");
        return affect;
      }
      else if (m_lecteur.getSymbole() == "si")
        return instSi();
      // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
      else if (m_lecteur.getSymbole() == "tantque")
          return instTantQue();
      else if (m_lecteur.getSymbole() == "repeter")
          return instRepeter();
      else if (m_lecteur.getSymbole() == "pour")
          return instPour();
      else if (m_lecteur.getSymbole() == "lire")
          return instLire();
      else if (m_lecteur.getSymbole() == "ecrire")
                return instEcrire();
      else if (m_lecteur.getSymbole() == "permut")
          return instPermut();
      else{
          erreur("Instruction incorrecte");
          return nullptr;
      }
    }catch(SyntaxeException const& e){ // on récupère l'exception qui a été levée
        cout << e.what() << endl;
        if (m_lecteur.getSymbole() != "finproc"){
        while(m_lecteur.getSymbole() != ";" || m_lecteur.getSymbole() != "finproc" || m_lecteur.getSymbole() != "<FINDEFICHIER>"){
            m_lecteur.avancer();
        }
        }
        while((m_lecteur.getSymbole()!="si"&& m_lecteur.getSymbole()!="tantque" && m_lecteur.getSymbole()!="pour" &&
               m_lecteur.getSymbole()!="ecrire" && m_lecteur.getSymbole()!="lire") && m_lecteur.getSymbole()!="<FINDEFICHIER>"
                && m_lecteur.getSymbole() !="<VARIABLE>"  && m_lecteur.getSymbole() !="finproc" && m_lecteur.getSymbole() != "permut" ){
            m_lecteur.avancer(); // on fait avancer le lecteur tant qu'il ne lit pas un des symbole du while
            
        }
        m_arbre = nullptr;
        cptSyntaxeErreur = cptSyntaxeErreur +1;
        return nullptr;
      }
    }


Noeud* Interpreteur::affectation() {
  // <affectation> ::= <variable> = <expression> 
  try{tester("<VARIABLE>");
  Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
  m_lecteur.avancer();
  testerEtAvancer("=");
  Noeud* exp = expression();             // On mémorise l'expression trouvée
  return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
  } catch(SyntaxeException &e){
      throw;
  }
}

Noeud* Interpreteur::expression() {
  // <expression> ::= <facteur> { <opBinaire> <facteur> }
  //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
    Noeud* et = expEt();
    while(m_lecteur.getSymbole()=="ou"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* etdroite = expEt();
        et = new NoeudOperateurBinaire(operateur, et, etdroite);
    }
    return et;
}

Noeud* Interpreteur::facteur() {
  // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
  try{Noeud* fact = nullptr;
  if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
    fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
  } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
  } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
  } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
    m_lecteur.avancer();
    fact = expression();
    testerEtAvancer(")");
  } else
    erreur("Facteur incorrect");
  return fact;
  } catch (SyntaxeException &e){
      throw;
  }
}

Noeud* Interpreteur::expEt(){
    Noeud* comp = expComp();
    while(m_lecteur.getSymbole() == "et"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* compdroite = expComp();
        comp = new NoeudOperateurBinaire(operateur, comp, compdroite);
    }
    return comp;
}

Noeud* Interpreteur::expComp(){
    Noeud* add = expAdd();
    while(m_lecteur.getSymbole()=="<"||m_lecteur.getSymbole()==">"||
          m_lecteur.getSymbole()=="!="||m_lecteur.getSymbole()=="=="||
          m_lecteur.getSymbole()=="<="||m_lecteur.getSymbole()==">="){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* adddroite = expAdd();
        add = new NoeudOperateurBinaire(operateur, add, adddroite);
    }
    return add;
}

Noeud* Interpreteur::expAdd(){
    Noeud* mult = expMult();
    while(m_lecteur.getSymbole()=="+"||m_lecteur.getSymbole()=="-"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* multdroite = expMult();
        mult = new NoeudOperateurBinaire(operateur, mult, multdroite);
    }
    return mult;
}

Noeud* Interpreteur::expMult(){
    Noeud*  parent = expparent();
    while(m_lecteur.getSymbole()=="*"||m_lecteur.getSymbole()=="/"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* parentdroite = expparent();
        parent = new NoeudOperateurBinaire(operateur, parent, parentdroite);
    }
    return parent;
}

Noeud* Interpreteur::expparent(){
    Noeud* fact = facteur();
    if(m_lecteur.getSymbole()=="("){
    while(m_lecteur.getSymbole()!=")"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* facteurdroite = facteur();
        fact = new NoeudOperateurBinaire(operateur, fact, facteurdroite);
    }
    }
    return fact;
}

Noeud* Interpreteur::instSi() {
  // <instSiRiche> ::= si (<expression>) <seqInst> { sinonsi (<expression>) <seqInst> } [sinon <seqInst>] finsi
    try{vector<Noeud*> vectorSeq;
    vector<Noeud*> vectorCond;
    testerEtAvancer("si");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la première condition
    vectorCond.push_back(condition); 
    testerEtAvancer(")");
    Noeud* sequence = seqInst(); // On mémorise la première séquence d'instruction
    vectorSeq.push_back(sequence);
    Symbole var = m_lecteur.getSymbole();
    while (var == "sinonsi"){  // Tant qu'on trouve un sinonsi, on analyse
        testerEtAvancer("sinonsi");
        testerEtAvancer("(");
        Noeud* condition = expression(); // On mémorise la condition du sinonsi actuel
        vectorCond.push_back(condition); 
        testerEtAvancer(")");
        Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
        vectorSeq.push_back(sequence);
        var = m_lecteur.getSymbole();
  }if(var == "sinon"){ // Si on tombe sur un sinon,
      testerEtAvancer("sinon");
      Noeud* sequence = seqInst(); // On mémorise sa séquence d'instruction en dernier
      vectorSeq.push_back(sequence);
  }
    testerEtAvancer("finsi");
    testerEtAvancer(";");
  return new NoeudInstSi( vectorCond,vectorSeq); // Et on renvoie un noeud Instruction Si
  } catch(SyntaxeException &e){
      throw;
  }
}

Noeud* Interpreteur::instTantQue() {
  // <instTantQue> ::= tantque ( <expression> ) <seqInst> fintantque
  try{testerEtAvancer("tantque");
  testerEtAvancer("(");
  Noeud* condition = expression(); // On mémorise la condition
  testerEtAvancer(")");
  testerEtAvancer(";");
  Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
  testerEtAvancer("fintantque");
  testerEtAvancer(";");
  return new NoeudInstTantQue(condition, sequence); // Et on renvoie un noeud Instruction Tant Que}
  } catch(SyntaxeException &e){
      throw;
  }
}

Noeud* Interpreteur::instRepeter(){
    // <instRepeter> ::=repeter <seqInst> jusqua( <expression> )
    try{testerEtAvancer("repeter");
    Noeud* sequence = seqInst(); // On mémorise la séquence d'instruction
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expression(); // on mémorise la condition
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstRepeter(condition, sequence); // on retourne un noeud Instruction Repeter
    } catch(SyntaxeException &e){
      throw;
  }
}

Noeud* Interpreteur::instPour(){
    // <instPour> ::= pour ( [ <affectation> ] ; <expression> ; [ <affectation> ] ) <seqInst> finpour
    try{testerEtAvancer("pour");
    testerEtAvancer("(");
    Noeud* affectation1 = nullptr; 
    if( m_lecteur.getSymbole() != ";"){ // On cherche à savoir si il y a une première affectation
        affectation1 = affectation();
    }
    testerEtAvancer(";");
    Noeud* expr = expression();
    testerEtAvancer(";");
    Noeud* affectation2 = nullptr;
    if(m_lecteur.getSymbole() != ")"){ // On cherche à savoir si il y a une deuxième affectation
        affectation2 = affectation();
    }else{
        m_lecteur.avancer();
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    Noeud* sequence = seqInst(); // On mémorise la séquence d'instruction
    testerEtAvancer("finpour");
    testerEtAvancer(";");
    return new NoeudInstPour(affectation1, expr, affectation2, sequence); // on retourne un noeud Instruction Pour
    } catch(SyntaxeException &e){
      throw;
  }
}


Noeud* Interpreteur::instLire(){
    // <instLire> ::= lire ( <variable> { , <variable> } )
    try{vector<Noeud*> vectorVar;
    testerEtAvancer("lire");
    testerEtAvancer("(");
    SymboleValue* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // On mémorise la première variable
    while(Symbole(var->getChaine()) == "<VARIABLE>"){ // On regarde si il y a d'autres variable à lire
       
        vectorVar.push_back(var); // On mémorise la nouvelle variable
        m_lecteur.avancer();
        if(m_lecteur.getSymbole()==","){
            testerEtAvancer(",");
        }else{
            testerEtAvancer(")");
            testerEtAvancer(";");
        }
        var = m_table.chercheAjoute(m_lecteur.getSymbole()); // On ajoute la varibale trouvée à la table des symboles
    }    
    return new NoeudInstLire(vectorVar);    // on retourne un noeud instruction Lire
    } catch(SyntaxeException &e){
      throw;
  }
}

Noeud* Interpreteur::instEcrire() {
    // <instEcrire>  ::= ecrire( <expression> | <chaine> {, <expression> | <chaine> })
    try{Noeud* noeud = nullptr;
    Noeud* noeud2 = nullptr;
    testerEtAvancer("ecrire");
    testerEtAvancer("(");

    if (m_lecteur.getSymbole() == "<CHAINE>") {
        noeud = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
        m_lecteur.avancer();
        
    }else{ // sinon c'est une expression
        noeud = expression();
    }
    
    vector<Noeud*> noeudsSupp;
    
    while(m_lecteur.getSymbole()==","){ // on regarde si il y a d'autres choses à écrire
        testerEtAvancer(",");
        if (m_lecteur.getSymbole() == "<CHAINE>") {
            noeud2 = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
            m_lecteur.avancer();
            noeudsSupp.push_back(noeud2);
        }else { // si le symbole lu est un entier , ça veut dire que c'est une expression
            noeud2 = expression();
            noeudsSupp.push_back(noeud2);
        }
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    
    return new NoeudInstEcrire(noeud,noeudsSupp); // on retourne un noeud inst Ecrire
    } catch(SyntaxeException &e){
      throw;
  }
}

Noeud* Interpreteur::instPermut(){
    try{Noeud* var1 = nullptr;
    Noeud* var2 = nullptr;
    m_table.chercheAjoute( Symbole ("s")); // On ajoute une variable s à la table, pour la traduction en C++
    testerEtAvancer("permut");
    testerEtAvancer("(");
    if(m_lecteur.getSymbole() == "<VARIABLE>"){ 
        var1 = m_table.chercheAjoute(m_lecteur.getSymbole()); // On récupère la première variable
        m_lecteur.avancer();
    }
    testerEtAvancer(",");
    if(m_lecteur.getSymbole() == "<VARIABLE>"){
        var2 = m_table.chercheAjoute(m_lecteur.getSymbole()); // On récupère la seconde variable
        m_lecteur.avancer();
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudPermut(var1,var2); // on retourne un noeud instPermut
    } catch(SyntaxeException &e){
      throw;
  }
}

void Interpreteur::traduitEnCPP(ostream& cout, unsigned int indentation) const{
    cout << "#include <iostream>" << endl << endl;
    cout << setw(indentation) << "" << "int main() {" << endl; // Début d’un programme C++
    for(int i = 0; i<m_table.getTaille(); i++){
        if(m_table[i] == "<VARIABLE>"){
        cout << setw(indentation+4) << "" << "int "<<m_table[i].getChaine();
            cout << ";" << endl;
        }
    }
 if(getArbre() != nullptr ) getArbre()->traduitEnCPP(cout,indentation+4); // lance l'opération traduitEnCPP sur la racine
 cout << setw(4*(indentation+1)) << "" << "return 0;" << endl ;
 cout << setw(4*indentation) << "}" << endl ; // Fin d’un programme C++
}



