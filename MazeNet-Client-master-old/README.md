# MazeNet-Client

Branches:

master : default branch

harmless : branch utilising ARP-Spoofing

nuke : branch utilising IPv6 Router Advertisement Flooding

## Build
Needed Libraries:
- Debian: xsdcxx,           Arch Linux: xsd
- Debian: libboost-all-dev, Arch Linux: boost
- Debian: libxerces-c-dev,  Arch Linux: xerces-c
- Debian: libpcap-dev,      Arch Linux: libpcap

either run in the MazeNet-Client directory:
```
$ export MAZENET=`pwd`
$ scripts/mazenet.all.sh
```

or:
```
$ mkdir Build && cd Build
$ cmake ..
$ make
```

## Running the applications:
If the above libraries are installed, the precompiled version might also work. Otherwise a recompile is recommended.
If the w3p0nz.active config option is set to true, you need root permissions!

The Host and Port can be configured in the config file.
Specifing host and port manually will override these settings.
### Examples:
Display help: 
```
$ MazeNet-Client --help
```

Change Host / Port:
```
$ MazeNet-Client --host <hostname> --port <port>
```

## Heuristic configuration

### base

- reachableFieldValue : Value of a field that is reachable by the player

- reachableTreasureValue : Value of a not-yet-found treasure that is reachable by the player

- reachableHomeValue : Value of a reachable Home , will be divided by the number of remaining treasures to increase value over time.

- reachableHomeCutoff : number of remaining treasures, that trigger home value calculation

### position

- onTargetValue : Value for standing on the current treasure, this should be higher than almost everything else (except for enemy reachable home)

- transitionMoveValue : Value of a possible transition move to target from this position. if two transitions are needed value will be divided by 2.

- transitionPossibleTreasureValue : Value of a potential treasures after a transition move if we already know our treasure.

- distanceBaseValue : Base value for distance to target. will be divided by distance*distanceFactor

- distanceFactor : scaling for distance. The above.

- moveOutOfMapPenalty : obsolete. Just here for security reasons.

### enemy

Counter measures against enemy players.

#### next
These values affect the next player

#### best
These values affect the best player

- reachableTreasureValue : value of a treasure the enemy can reach.

- reachableHomeValue : value of a reachable home. This can be used for aggressive late game behaviour. divided by number of remaining treasures

- reachablehomeCutoff : above value will be used as soon as the remaining treasures are below or equal to the cutoff

- transitionMoveTreasureValue : value of a treasure the enemy can reach with a shift operation. divided by number of remaining treasures to increase late game aggressive behaviour.