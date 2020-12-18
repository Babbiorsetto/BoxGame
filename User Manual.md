## User Manual
BoxGame is a multiplayer, turn-based, client/server game for Unix platforms.  
The server holds all information about the current game, as well as about player's profiles.  
The clients allow users to connect to the server to register or login and play a game.

## Game Rules
- Players are freely able to join or leave a game at any time.
- The game is played on a bi-dimensional map. Several boxes are placed on the game map. The objective is to pick up boxes and turn them in at delivery zones spread throughout.
- On their turn, each player can make a single move toward a direction, pick up or deposit a box. The borders of the map wrap around to the opposite side.
- The game map contains invisible obstacles. Trying to move to the spot where an obstacle is, or to a spot occupied by another player, causes the player to miss their chance to move.
- Boxes have a timer that starts counting down once they're picked up. If a box expires it disappears from the player and cannot be turned in.
- A game ends when all boxes have either been turned in or disappeared. The player who turned in the most is declared the winner.

## Interface Specifics
Each game object is represented by a character on the map.
- "Y" is the player character
- "P" is a character controlled by another player
- "1" is a box
- "A" is a dropoff point
- "X" is a known obstacle
- "-" is a navigable tile or obstacle not yet known by the player

The game client shows the map, as well as the names, ordering and score of the players in the current game.

## Compilation
Compile using gnu make and gcc.  
Inside the `BoxGame` working directory run
```
make bin/server
make bin/client
```
if you're missing make you'll get some hint as to how to install it on your system

## Execution
Launch the server with
```
./bin/server PORT_N
```
where `PORT_N` is an integer between 1024 and 65535.

---

Launch the clients with 
```
./bin/client IP_ADDRESS PORT_N
```
where `IP_ADDRESS` is a valid IPv4 address for the server machine and `PORT_N` is the number selected for the server
