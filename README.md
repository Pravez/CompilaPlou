# Projet de Compilation
Groupe Paul BRETON - Nicolas VIDAL

Note : Ce projet a été réalisé avec la version `1.36` d'emscripten et la version `3.9` de LLVM.


### Instructions et indications

Vous trouverez dans ce répertoire deux sous-répertoires. Le premier `p5-libraries`
contient toute la bibliothèque p5 et sera exploité par les sorties d'Emscripten.

Dans le répertoire `src/sources` vous pourrez trouver toutes les sources de notre compilateur,
ainsi que les fichiers de tests (dans `src/final-tests`). un Makefile est mis à votre disposition pour pouvoir
utiliser le compilateur.

L'éxecutable produit lors de la compilation, `parse`, vous permettra de parser les fichier C. Si vous donnez l'option
`-o` suivie d'un nom de fichier, il sortira le résultat dans ce fichier plutôt que dans le fichier par défaut `output.ll`.

### Utilisation du Makefile

- `make` ou `make all` : permet de compiler le compilateur. Produit l'exécutable `parse`, à la racine du répertoire `src`.
- `make run [test name].c` : en remplaçant `test name` par le nom du test que vous souhaitez lancer,
vous produirez un code llvm dans output.ll, correspondant au résultat de la compilation du test par notre compilateur.
- `make compll [file].ll` : permet de lancer la compilation du llvm produit via l'outil `lli`, en passant en paramètre `file` le nom du fichier à prendre.
- `make emcc [file].ll` : permet de compiler à l'aide d'`emcc` un fichier `.ll` donné en paramètre. Ceci produit un fichier `foo.js` résultant de la compilation.
 Par ailleurs il faut noter que nous avons par défaut mis les fonctions `my_draw` et `my_setup` à exporter. Si vous souhaitez
 exporter d'autres fonctions il sera nécessaire de ne pas utiliser le raccourci crée par ce makefile.
- `make clean` : permet de "nettoyer" le répertoire des fichiers compilés et intermédiaires de compilation.

##### Liste des tests possibles

Voici la liste (exhaustive) des tests que vous pouvez lancer, et que vous trouverez dans `src/final-tests`

- boucles
- errors
- fibonacci
- ifAndFunction
- simple
- warnings

Tests impliquant l'utilisation de p5 : (et donc d'emcc)
- get_started_p5
- get_started_advanced
- example_code_ex
- example_code_mandelbrot

###### Comment réaliser les tests ?

- Tout d'abord, assurez vous d'avoir bien compilé le compilateur : placez vous dans le répertoire `src`.
Puis faites : `make all`.
- Ensuite, compilez le fichier de tests que vous souhaitez : `make run [votrefichier].c`.
- Vous pourrez alors voir dans `output.ll` le résultat de la compilation. Pour le tester via le
makefile avec le programme `lli`, faites : `make compll [votrefichier].ll`. Ici votre fichier sera donc 
`output`. Vous aurez alors d'affiché le code de retour du programme. 

==> Cas particulier : compilation et test avec la bibliothèque p5.js

Plutôt que d'utiliser `make compll` utilisez `make emcc [votrefichier].ll` où votre fichier sera
vraisemblablement `output`. Firefox s'ouvrira alors avec le résultat de la compilation.