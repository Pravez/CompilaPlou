#ifndef PROJET_COMPILATION_TOOLS_H
#define PROJET_COMPILATION_TOOLS_H

enum color{
    RED,
    BLUE,
    GREEN
};
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

#endif //PROJET_COMPILATION_TOOLS_H
