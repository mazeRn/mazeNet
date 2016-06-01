# About

This project contains the server and the GUI component of the student project
for 'Rechnernetze'. It opens a match for the "Das verrückte Labyrinth" boardgame
and waits for players. After each turn it checks, if the move is correct, and
request for annother one on fail. If one player wins, the match ends.
It comunicates through sockets and uses XML to serialize objects.
The XML-syntax is defined by `./src/XSD/mazeCom.xsd`.

# How to...

...clone the project.

## commandline

change to the folder you store your projects and type:

```
git clone https://git.noc.fh-aachen.de/rechnernetze/mazenet-server.git
```

this will crete a `mazenet-server` folder and copies current files of the master branch. You can open/import the project with your favorite IDE.

## Eclipse

TODO