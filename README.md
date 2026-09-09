# wav_channels_splitter

`wav_channels_splitter` est un petit utilitaire en C++20 permettant de **séparer les canaux audio d'un fichier WAV multicanal**.

Pour chaque canal du fichier source, le programme crée un nouveau fichier WAV mono contenant uniquement les échantillons de ce canal.

Le projet possède deux implémentations distinctes afin de faciliter la compilation sur **Linux** et **Windows**.

## Démonstration

### Avant

![Fichier WAV original](images/input.jpg)

### Après

![Fichiers WAV séparés par canal](images/output.jpg)

## Fonctionnement

À partir d'un fichier comme :

```text
music.wav
```

si celui-ci contient 4 canaux, le programme génère :

```text
music_ch1.wav
music_ch2.wav
music_ch3.wav
music_ch4.wav
```

Les fichiers générés sont placés dans le **même dossier que le fichier source**.

Le programme conserve les principales caractéristiques audio du fichier d'origine :

- fréquence d'échantillonnage ;
- résolution (`bits per sample`) ;
- format audio PCM ou IEEE float supporté ;
- nombre d'échantillons correspondant à chaque canal.

Chaque fichier de sortie est écrit comme un fichier WAV **mono**.

---

## Arborescence du projet

Une organisation du projet est la suivante :

```text
wav_channels_splitter/
├── linux_srcs/
│   ├── main.cpp
│   ├── Wav.cpp
│   └── Wav.hpp
├── windows_srcs/
│   ├── main.cpp
│   ├── Wav.cpp
│   └── Wav.hpp
├── bin/
│   ├── wav_channels_splitter
│   └── wav_channels_splitter.exe
├── Makefile
├── install_wav_channels_splitter.reg
└── README.md
```

Les versions Linux et Windows utilisent des entrées différentes :

- Linux utilise `main(int argc, char **argv)` et `std::string`.
- Windows utilise `wmain(int argc, wchar_t **argv)` et `std::filesystem::path`, notamment pour mieux gérer les chemins contenant des caractères Unicode.

---

## Prérequis

Le projet possède `deux binaires autonomes déjà compilés` dans le fichier bin qui pourront être utilisés directement

Linux :

```text
wav_channels_splitter
```

Windows :

```text
wav_channels_splitter.exe
```

Néanmoins pour une nouvelle compilation certains prérequis seront nécessaires

### Linux

Le projet nécessite :

- un compilateur C++ compatible C++20 ;
- `g++` ;
- `make`.

Par exemple sur Debian/Ubuntu :

```bash
sudo apt install g++ make
```

### Windows

Pour compiler la version Windows depuis Linux, le projet utilise le compilateur MinGW-w64 :

```text
x86_64-w64-mingw32-g++
```

Le Makefile est configuré pour produire un exécutable Windows statique.

---

## Compilation

### Linux

Depuis la racine du projet :

```bash
make
```

L'exécutable produit est :

```text
wav_channels_splitter
```

Pour supprimer les fichiers objets :

```bash
make clean
```

Pour supprimer les fichiers objets et les exécutables :

```bash
make fclean
```

Pour nettoyer puis reconstruire :

```bash
make re
```

### Windows depuis Linux

Le Makefile fournit une cible dédiée :

```bash
make windows
```

Elle produit :

```text
wav_channels_splitter.exe
```

La compilation utilise notamment :

```text
-static
-static-libgcc
-static-libstdc++
```

afin de limiter les dépendances nécessaires à l'exécution du programme sur Windows.

---

## Utilisation sous Linux

Le programme attend exactement **un argument** : le chemin du fichier WAV à traiter.

Exemple :

```bash
./wav_channels_splitter /home/user/audio/music.wav
```

Ou depuis le dossier courant :

```bash
./wav_channels_splitter music.wav
```

Le programme vérifie que :

1. le chemin existe ;
2. le chemin correspond à un fichier ;
3. l'extension est `.wav`.

Si le fichier est valide, le programme lit son en-tête WAV, récupère les données audio, puis crée un fichier mono par canal.

---

## Utilisation sous Windows

L'exécutable peut être lancé depuis un terminal :

```text
wav_channels_splitter.exe "C:\Users\User\Music\music.wav"
```

Les guillemets sont recommandés lorsque le chemin contient des espaces.

Exemple :

```text
wav_channels_splitter.exe "C:\Users\User\Music\My Audio\music.wav"
```

La version Windows utilise `wmain` et `std::filesystem::path` pour accepter correctement les chemins Windows, y compris les chemins contenant des caractères non ASCII.

---

## Intégration au clic droit sous Windows

Le projet fournit également un fichier `.reg` permettant d'ajouter une entrée au menu contextuel des fichiers `.wav`.

Le fichier est basé sur la clé :

```text
HKEY_CURRENT_USER\Software\Classes\SystemFileAssociations\.wav\shell\WavChannelsSplitter
```

L'entrée apparaît alors dans le menu contextuel d'un fichier WAV sous le nom :

```text
Wav channels Splitter
```

Lorsqu'elle est sélectionnée, Windows exécute le programme avec le fichier choisi comme argument.

### Exemple de configuration `.reg`

```reg
Windows Registry Editor Version 5.00

[HKEY_CURRENT_USER\Software\Classes\SystemFileAssociations\.wav\shell\WavChannelsSplitter]
@="Wav channels Splitter"

[HKEY_CURRENT_USER\Software\Classes\SystemFileAssociations\.wav\shell\WavChannelsSplitter\command]
@="\"C:\\Program Files\\WavChannelsSplitter\\wav_channels_splitter.exe\" \"%1\""
```

### Important

Le chemin indiqué dans le `.reg` doit correspondre à l'emplacement réel de l'exécutable.

Dans l'exemple ci-dessus, Windows cherchera :

```text
C:\Program Files\WavChannelsSplitter\wav_channels_splitter.exe
```

Le `.reg` **n'installe pas automatiquement le programme** et ne copie pas l'exécutable. Il ajoute uniquement l'entrée correspondante dans le registre Windows.

Après avoir placé l'exécutable à l'emplacement souhaité, le fichier `.reg` peut être exécuté pour enregistrer l'entrée du menu contextuel.

---

## Format WAV pris en charge

Le programme analyse les chunks standards d'un fichier RIFF/WAVE et recherche notamment :

- `fmt ` : informations sur le format audio ;
- `data` : données audio.

Les formats identifiés par le programme sont :

| Format WAV | Bits par échantillon |
|---|---:|
| PCM | 8 bits |
| PCM | 16 bits |
| PCM | 24 bits |
| PCM | 32 bits |
| IEEE float | 32 bits |
| IEEE float | 64 bits |

Le programme vérifie notamment la présence des signatures :

```text
RIFF
WAVE
```

Les chunks inconnus sont ignorés lors de la lecture afin de permettre le traitement de fichiers WAV contenant d'autres chunks RIFF.

---

## Principe du découpage des canaux

Dans un fichier WAV PCM ou float interleaved, les échantillons sont stockés successivement par frame.

Par exemple, pour un fichier stéréo :

```text
L R L R L R L R ...
```

Le programme parcourt chaque frame et extrait l'échantillon correspondant au canal demandé.

Pour un fichier stéréo :

```text
source.wav
   │
   ├── canal 1 ──> source_ch1.wav
   │
   └── canal 2 ──> source_ch2.wav
```

Pour un fichier multicanal, le principe est identique avec un fichier de sortie supplémentaire pour chaque canal.

---

## Nom des fichiers générés

Le nom du fichier d'entrée est utilisé comme préfixe.

Par exemple :

```text
D:\Audio\test_surround.wav
```

produira :

```text
D:\Audio\test_surround_ch1.wav
D:\Audio\test_surround_ch2.wav
D:\Audio\test_surround_ch3.wav
D:\Audio\test_surround_ch4.wav
D:\Audio\test_surround_ch5.wav
D:\Audio\test_surround_ch6.wav
```

---

## Cas d'un fichier mono

Si le fichier contient un seul canal, aucune séparation n'est effectuée.

```text
channels <= 1
```

Le programme quitte alors la fonction de séparation sans créer de nouveau fichier.

---

## Gestion des erreurs

Le programme signale notamment les erreurs suivantes :

- fichier inexistant ;
- chemin ne correspondant pas à un fichier ;
- fichier dont l'extension n'est pas `.wav` ;
- fichier inaccessible ;
- signature RIFF/WAVE invalide ;
- chunk `fmt ` invalide ;
- erreur de lecture des données audio ;
- erreur lors de la création ou de l'écriture d'un fichier de sortie.

Les erreurs rencontrées pendant le traitement sont interceptées dans `main` et affichées dans la console.

---

## Limitations actuelles

Le programme est volontairement simple et se concentre sur la séparation des canaux audio.

Quelques limitations actuelles :

- les fichiers non-`.wav` sont refusés ;
- le programme attend un seul fichier en entrée ;
- le fichier de sortie est toujours créé dans le même dossier que le fichier source ;
- les métadonnées ou chunks additionnels du fichier source ne sont pas recopiés dans les fichiers de sortie ;
- la sortie est reconstruite avec un chunk `fmt ` standard de 16 octets ;
- un fichier mono ne produit aucun fichier supplémentaire ;
- les formats WAV non reconnus ne sont pas explicitement bloqués avant le traitement du fichier, même si leur format reste marqué comme `UNKNOWN`.

---
