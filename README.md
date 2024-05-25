# PixL-Engine

Je développe un moteur de jeu pendant mon temps libre. Ayant étudié le moteur de Pokémon Émeraude (et effectué plusieurs améliorations), j'ai décidé de m'inspirer de celui-ci pour créer mon propre moteur.

Le système d'affichage des cartes repose sur un système de tilemap à plusieurs couches, auxquelles viendront s'ajouter d'autres couches (personnages, météo, interface utilisateur, etc.).

Le moteur est programmé en C/C++ et utilise la bibliothèque SDL pour l'affichage. Le langage de script n'est pas encore créé, mais j'ai plusieurs idées en tête. Il pourrait être en Python (plus accessible pour les débutants, mais nécessitant une interface de communication entre l'interpréteur Python et le moteur), en C++ (moins accessible pour les débutants, mais compilé directement dans le moteur, éliminant la dépendance à Python, bien que la recompilation du moteur soit nécessaire pour appliquer les changements), ou bien un langage que je développerais, ensuite converti en C++ (peut être plus simple que le C++, tout en conservant les mêmes avantages et inconvénients, mais cela pourrait rallonger le temps de compilation en présence d'un grand nombre de scripts).


EDIT : L'actuel code n'est qu'un POC , le but maintenant est de faire le rendu à l'aide d'OpenGL pour pouvoir faire des effet avancés grâce aux shaders. 
