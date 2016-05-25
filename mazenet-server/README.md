# About

This Project contains the server and the GUI component of the student project
for 'Rechnernetze'. It opens a match for the "Das verrückte Labyrinth" boardgame
and waits for Players. After each Turn it checks, if the move is correct, and
request for annother one on fail. If one Player wins the match ends.
It comunicates through sockets and uses XML to serialize Objects.
The XML-Syntax is defined by  `./src/XSD/mazeCom.xsd` File.  

# How to...

clone the Project.

## commandline

change to the Folder you store your projects and type:

```
git clone https://git.noc.fh-aachen.de/rechnernetze/mazenet-server.git
```

this will crete a `mazenet-server` folder and copies current Files of the Master branch. You can open/import the Project with your favorite IDE.

## Eclipse

TODO