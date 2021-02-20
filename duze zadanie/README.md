# Gamma (duze zadanie)

### Introduction

This project's main focus will be an implementation of a simple game called 'gamma'.
The game is played on a rectangular board made from equally sized square fields. We say fields are "neighbouring"
if they are adjoined on their sides (not only corners). Fields create an "area" if one can travel through all of them
only moving through neighbouring ones. Number of areas per player is a top-down set parameter of the game. 
At the beggining the board is empty. Players consecutively place their pieces. A palyer can place their piece on any
free field if this move does not violate the area limit. Once per game, during their turn, a player can make 
a "golden move" – replace another player's piece with thier own. Turns of players that cannot make any moves are skipped.
The game is won by the palyer with the most fields taken.

This projects consists of 3 parts:
1. Implementation of the game engine
2. Implementation of batch and interactive modes
3. Moficataion of cmake, interactive mode and `gamma_golden_possible`


### Description

Description of the engine is in the file `src\gamma.h.`

###### Batch mode

The programme reads data from the command line.
At the starts it awaits one from two commands:
* `B width height players areas`
* `I width height players areas`

After chosing the batch mode the programme acccepts commands:
* `m player x y` – calls `gamma_move`
* `g player x y` – calls `gamma_golden_move`
* `b player` 	 – calls `gamma_busy_fields`
* `f player`	 – calls `gamma_free_fields`
* `q player` 	 – calls `gamma_golden_possible`
* `p` 			 – calls `gamma_board`

Every incorrect line should be acknowledged by printing `ERROR line\n` to stderr.
Every correct line shoould be acknowledged by printing `OK line\n` to stdout.
Where `line` is the number of a particular line.

###### Interacive mode

In the interactive mode the board is pictured.
- Movement 	   – `←` `↑` `↓` `→`
- Placing pieces   – `SPACE`
- Golden move	   – `G`
- Skip turn	   – `C`
- Exit		   – `^D`

	
### Additional requirements

* CMake creating both debug and release versions
* Doxygen documentation
* Submit to the University's repository

### Comments

- One may wonder why are the `ASCII escape codes` used in this projects instead of exploting fun libraries.
  Unfortunately it occured that using them could potenially result in valgrind errors :crying_cat_face:

- The game might not be compatibile with OS other than Linux :crying_cat_face:

### Preview

![alt text](https://github.com/kozakusek/pictures/blob/main/gamma/start.png?raw=true "Interactive Mode")  
![alt text](https://github.com/kozakusek/pictures/blob/main/gamma/game.png?raw=true "2 player game")  
![alt text](https://github.com/kozakusek/pictures/blob/main/gamma/end.png?raw=true "Scoreboard")  

### Disclaimer

I do not own the idea for that project. I am not the author of the above text. This is merely a rough translation and a summary.
All credit goes to University of Warsaw, Faculty of Mathematics, Informatics and Mechanics.
