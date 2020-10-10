# Forests (male zadanie)

### Introduction

There grow many forests in the world. In every forest there are many trees. On every tree many animals can live.
Every forest has a unique name and every tree in a particular forest also. Every animal living on a particular tree
has a unique name. Write a programme imitating that world.

### Description

At the start of the programme there should be no forest, nor any tree or animal in the world. The programme reads
characters from standard input and interprets undermentioned commands (forest, tree, animal are the unique names):

- `ADD forest [tree] [animal]`   – adds `animal` to `tree` in `forest`, or adds `tree` to `forest`, or creates `forest`.
- `DEL [forest] [tree] [animal]` – deletes `animal` from `tree` in `forest`, or `tree` from `forest`, or `forest`
								 from the world, or deletes all beings.
- `PRINT [forest] [tree]`		   – prints lexicographically all forests, or all trees in `forest`, 
								 or all animals on `tree` in `forest`.
-`CHECK forest [tree] [animal]` – checks if there exists `animal` on `tree` in `forest`, `tree` in `forest`, or `forest`.

Adding an existing being or deleting a non-existent one is ignored, same as trying to print from an empty being
or lines consiting only white characters or began with an `#`.

### Output

* Stdout
	* For `ADD` and `DEL` prints `OK\n`
	* For `CHECK` prints `YES\n` or `NO\n` depending on the result
* Stderr
	* `ERROR\n` for every line not matching the description
	
### Additional requirements

* A `makefile` file
* A script for automated testing `test.sh`
	* Run this way: `./test.sh prog dir` 

### Disclaimer

I do not own the idea for that project. I am not the author. This is merely a rough translation and a summary.
All credit goes to University of Warsaw, Faculty of Mathematics, Informatics and Mechanics.
