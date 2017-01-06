# Projet de Compilation
Groupe Paul BRETON - Nicolas VIDAL

Note : Ce projet a été réalisé avec la version `1.36` d'emscripten et la version `3.9` de LLVM.


### Instructions et indications

Vous trouverez dans ce répertoire deux sous-répertoires. Le premier `p5-libraries`
contient toute la bibliothèque p5 et sera exploité par les sorties d'Emscripten.

Dans le répertoire `src` vous pourrez trouver toutes les sources de notre compilateur,
ainsi que les fichiers de tests. un Makefile est mis à votre disposition pour pouvoir
utiliser le compilateur.

L'éxecutable produit lors de la compilation, `parse`, vous permettra de parser les fichier C. Si vous donnez l'option
`-o` suivie d'un nom de fichier, il sortira le résultat dans ce fichier plutôt que dans le fichier par défaut `output.ll`.

### Utilisation du Makefile

- `make` ou `make all` : permet de compiler le compilateur. Produit l'exécutable `parse`
- `make run [test name].c` : en remplaçant `test name` par le nom du test que vous souhaitez lancer,
vous produirez un code llvm dans output.ll, correspondant au résultat de la compilation du test par notre compilateur.
- `make emcc [file].ll` : permet de compiler à l'aide d'`emcc` un fichier `.ll` donné en paramètre. Ceci produit un fichier `foo.js` résultant de la compilation.
 Par ailleurs il faut noter que nous avons par défaut mis les fonctions `my_draw` et `my_setup` à exporter. Si vous souhaitez
 exporter d'autres fonctions il sera nécessaire de ne pas utiliser le raccourci crée par ce makefile.
- `make clean` : permet de "nettoyer" le répertoire des fichiers compilés et intermédiaires de compilation.