# Assignment 2 Submission

## How to run

- Run ```\RoboCat\Bin\Debug\SocketDemo.exe```
- Type ```s``` to start a server
- Run the .exe again, this time typing ```c``` to start a client and ```1``` for player 1.
- Run the .exe a third time, typing ```c``` for client and ```2``` for player 2.

OR

- Just run the .exe with ```c``` and ```1``` to play without connected to the server

## How to use

- Click on the screen to shoot a bullet
- The player, and crosshair, and bullets are synced directly between clients through the server

## Video demo

https://user-images.githubusercontent.com/90067642/166136357-906316c9-d787-425c-9480-21986cc15a55.mp4

## Bugs

- The server will panic until both clients are open
- A client will crash very rarely and randomly without an error
  - This is because client A receives a bullet array from client B midway through rendering client Bs bullet array.
  - Client A then tries to render the next bullet in the array, and cannot because it has been reset from the server in a seperate thread
