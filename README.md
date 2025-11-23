Card printer
==========
A small application to edit and print cards in pdf.

Dependencies
-----

* [SFML](https://github.com/SFML/SFML)
* [nlohmann-json](https://github.nlohmann/json.git)
* [spdlog](https://github.com/gabime/spdlog)
* [SmolGui](https://github.com/stoleks/smolgui)
* [entt](https://github.com/skypjack/entt.git)
* [PDFHummus](https://github.com/galkahana/PDF-Writer)

Building it
-----

You simply need to copy this git repository and then to call cmake
```console
mkdir build
cd build
cmake ..
cmake --build .
cd ..
```

How to use it
-----

Right now this app has basic functionnalities, so you need to specify a folder containing a init.json in contents/filepath.json, in the relativePathToExternal entry.

```json
{
  "relativePathToExternal": "your_path_starting_from_this_repo_dir",
}
```

You can find a project using it [here](https://forge.apps.education.fr/jedrecyalexandre/jeu_cartes_molecules_meth) (it's in french), its init.json is as follow:

```json
{
  "cardsTexts": "textes_cartes.json",
  "cardsDataCsv": "cartes.csv",
  "cardModelJson": "modele.json",
  "cardsDataJson": "cartes.json",
  "outputFile": "paquet_cartes_meth",
  "outputDirectory": "impression/",
  "texturesDirectory": "images/textures/"
}
```
