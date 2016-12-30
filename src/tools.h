#ifndef PROJET_COMPILATION_TOOLS_H
#define PROJET_COMPILATION_TOOLS_H

extern int CURRENT_REGI;
extern int CURRENT_LBLI;
extern char CURRENT_REG[10];
extern char CURRENT_LBL[10];

enum color{
    RED,
    BLUE,
    GREEN
};
#define COLOR_FG_RED    "\033[31m"
#define COLOR_FG_BLUE   "\033[34m"
#define COLOR_FG_GREEN  "\033[32m"
#define COLOR_RESET     "\033[31m"

/**
 * Affiche du texte dans le terminal avec la couleur donnée
 * @param s String Message à afficher
 * @param c color Couleur à utiliser
 */
void debug(char* s, enum color c);

/**
 * Affiche du texte dans le terminal avec la couleur donnée
 * @param s String Message à afficher
 * @param i int Entier supplémentaire à afficher à la fin du message
 * @param c color Couleur à utiliser
 */
void debugi(char* s, int i, enum color c);

/**
 * Affiche du texte dans le terminal avec la couleur donnée
 * @param s String Message à afficher
 * @param m char* String supplémentaire à afficher à la fin du message
 * @param c color Couleur à utiliser
 */
void debugs(char* s, char* m, enum color c);

/**
 * Retourne un nouveau numéro de registre et met à jour les variables globales
 * CURRENT_REGI et CURRENT_REG.
 * @return la nouvelle valeur de CURRENT_REGI
 */
int new_register();

/**
 * Retourne un nouveau numéro de label et met à jour les variables globales
 * CURRENT_LBLI et CURRENT_LBL.
 * @return la nouvelle valeur de CURRENT_LBLI
 */
int new_label();

/**
 * Utility function taking a list of char* to create one unique string
 * @param qty the number of params
 * @param ... the params
 * @return an allocated string
 */
char* concatenate_strings(int qty, ...);


#endif //PROJET_COMPILATION_TOOLS_H
