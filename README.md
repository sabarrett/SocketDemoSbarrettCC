# Socket Demo
# Running and playing instructions
These are the step-by-step instructions for running and playing Assignment 2.

1. Build the solution and launch a command window from SocketDemo\RoboCat\Bin\Debug
2. Run the .exe in the command window with the following arguments. For the first instance of the game, enter a port number and the username of the player.
Exe: SocketDemo.exe 8000 Harris
3. For all other instances once the master peer is open, give it the full address of the master peer, followed by the username.
Exe: SocketDemo.exe 127.0.0.1:8000 Scott
4. Once all game instances are open, press the S KEY on the master peer instance to start the game and wait 1 second.
5. The game should now be started and you can take the following actions.

LEFT CLICK: Will draw the base game object on both instances. Can be held down to draw more.

RIGHT CLICK: Will delete all game objects at the clicked location

ONE KEY: Will create a different game object at a random location. The locations will be synced

TWO KEY: Will create a 3rd game object at your mouse location. Will then move to a new location every few seconds. (Note: I couldn't get the random working right so while the movement is synced, it's not exactly random.)

SPACE KEY: Will pause all animations. (It's updated every frame, so if the button is held it will toggle quickly.)

ESCAPE: Will quit and end the program. (Can be done even if not connected to other players)
