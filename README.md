# SO2IC SERVER

server for the so2ic implementation

## Purpose

This is the server for the Shards Of Inifinity In C implementation. Shards of Infinty is a deckbuilding game that you could see [here](https://iello.fr/jeux/shards-of-infinity/). \
The purpose of this reimplementation in C is to challenge me a bit by making a client server game in pure C. Actually, the goal is to learn as much as I can in C :). \
If this project ever come to an end (100% impossible) I would like to make an algorithm/AI to play against me so I can play everywhere at any time. \
Finally, if I can use those 'bots' to make some probability about the game like a card tierlist it could be cool.

## Requirement

- *gcc*

## Usage

I've recently implement the [nob building tool](https://github.com/tsoding/nob.h) \
Since this add, to build this project you just have too :

```sh
gcc -o nob nob.c
./build/soi-server
```

This also works with clang, just compile nob.c using clang and change the following line in nob.c

```c
nob_cmd_append(&cmd, "gcc", "-Wall", "-Wextra", "-g", "-o", BUILD_DIR"soi-server");

// use the c compiler you want
nob_cmd_append(&cmd, "clang", "-Wall", "-Wextra", "-g", "-o", BUILD_DIR"soi-server");
nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-g", "-o", BUILD_DIR"soi-server");
```

## Author 

This project is entirely made by me (ASTOLFI Vincent). I suggest you to check on my [github profile](https://www.github.com/viastolfi) if you want to see the other project I've done for my studies or the ones I do in my free time.
