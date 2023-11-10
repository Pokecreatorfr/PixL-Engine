# PixL-Engine

Je développe de moteur de jeu sur mon temps libre.

Ayant étudié du moteur de pokémon émeraude (et réalisé plusieurs améliorations de celui-ci) j'ai décidé de m'inspirer de ce dernier pour la réalisation de mon propre moteur.

Le système d'affichage des cartes est un système de tilemap sur plusieurs couches, auquel viendront s'interposer d'autres couches (personnages, météo, interface utilisateur ... ) .

Le moteur est écrit en C/C++ et utilise la librairie SDL pour l'affichage. Le langage de script n'est pas encore crée, j'ai cependant plusieurs idées en tête, soit du python (plus facile pour les débutants, mais cela nécessite une interface de communication entre l'interpréteur python et le moteur), soit du C++(moins facile pour les débutants, mais sera compilé dans le moteur, cela permettra de s'affranchir de dépendance a python, mais obligera a recompiler le moteur pour que les changements prennent effet) ou alors un langage que je créerais qui sera ensuite converti en c++ (pourrait être plus facile que le c++, en gardant les mêmes avantages et inconvénients, mais pourra allonger le temps de compilation dans le cas ou un grand nombre de script est présent)
