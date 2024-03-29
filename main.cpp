#include <iostream>
using namespace std;
#include "Interpreteur.h"
#include "Exceptions.h"
#include <fstream>

int main(int argc, char* argv[]) {
    string nomFich;
    if (argc != 2) {
        cout << "Usage : " << argv[0] << " nom_fichier_source" << endl << endl;
        cout << "Entrez le nom du fichier que voulez-vous interpréter : ";
        getline(cin, nomFich);
    } else
        nomFich = argv[1];
    ifstream fichier(nomFich.c_str());
    try {
        if (fichier.fail()) throw FichierException();
        Interpreteur interpreteur(fichier);
        interpreteur.analyse();
        // Si pas d'exception levée, l'analyse syntaxique a réussi
        if(interpreteur.getcptSyntaxeErreur() != 0){
            cout << endl << "================ Syntaxe Incorrecte" ;
            cout << endl << interpreteur.getcptSyntaxeErreur() << " erreur de syntaxe trouvé dans le programme, impossible d'executer le programme" << endl;
        } else { 
        cout << endl << "================ Syntaxe Correcte" << endl;
        // On affiche le contenu de la table des symboles avant d'exécuter le programme
        cout << endl << "================ Table des symboles avant exécution : " << interpreteur.getTable();
        cout << endl << "================ Execution de l'arbre" << endl;
        // On exécute le programme si l'arbre n'est pas vide
        if (interpreteur.getArbre() != nullptr) {
        interpreteur.getArbre()->executer();
        ofstream cout;
        cout.open("traduction.cpp", ios::out);
        interpreteur.traduitEnCPP(cout, 0);
        }
        // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
        cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable(); }
    } catch (InterpreteurException & e) {
        cout << e.what() << endl;
    }
    return 0;
}
