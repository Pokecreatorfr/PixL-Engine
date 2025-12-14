# Retro Renderer

Retro Renderer est un moteur de rendu dont l’objectif est d’être simple à utiliser et d’offrir un rendu « rétro ». Pas de techniques modernes : des triangles, des textures et quelques lumières, pour une jolie bouillie de pixels.

L’API exposée sera découpée en deux parties.

La première sera le rendu officiel de **Retro Renderer** ; la seconde sera une partie « cheat », permettant des choses qu’on n’aurait jamais pu voir sur N64 ou PS1.

## Limites

### Pipeline

Le pipeline graphique du moteur de rendu est fixe, afin d’imiter les premières consoles 3D.

### Rendu des triangles

Les triangles envoyés à l’API n’ont que **deux modes de remplissage** et **deux modes de transparence**.

- Soit des UV de texture sont fournis
- Soit des couleurs par sommet.

Un triangle transparent ou semi-transparent doit être signalé au moteur afin d’être rendu avec le pipeline approprié.

### Lumières

1 ambiant
1 Directional
16 Point Light
16 Spot Light
