# Beebout Assignment 2

It is a 32-bit Windows console application that opens an SDL window.

## How to Use

Build and run. I set it up (hopefully) to copy necessary .dlls to the output folder, so it should work if you simply open the .exe - but it'll definitely work if ran through visual studio.


## How to Play

To start, in the windows console, type either /host or /connect x.x.x.x x as directed.

When a player connects, they will spawn in the top left corner.

Players can move with WASD, and spawn coins by left-clicking.
If a player touches a coin, they "collect" (destroy) it.

In the middle of the game, there is вжух. If a player touches вжух, he adds them to his list of magicked fellows, who's colors are changed by вжух approximately every second.
