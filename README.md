# Schönemann
- [About](#about)
- [How it works](#how-it-works)
- [Build](#build)
- [Rating](#rating)
- [Thanks](#thanks)

# About
This is a simple chess engine written in C++. The name was inspired by this chess line in the Ruy Lopez:
```
1. e4 e5 2. Nf3 Nc6 3. Bb5 f5 4. d4 fxe4 5. Nxe5 Nxe5 6. dxe5 c6 7. Nc3 *
```
in this it seems that white is hanging a bishop but is not worse.

# How it works
-  NNUE with 512 Hidden Layers
-  Trained with Leela Data

-  PVS-Search
-  SEE Prunning + MoveOrdering
-  Check Extension
-  MVA - LVV in MoveOrdering

# Build
Type 
  ```
  make all
  ```
This will produce an executable with the name of ```null.exe```

# Rating
[![lichess-bullet](https://lichess-shield.vercel.app/api?username=Schoenemann&format=bullet)](https://lichess.org/@/Schoenemann/perf/bullet)
[![lichess-blitz](https://lichess-shield.vercel.app/api?username=Schoenemann&format=blitz)](https://lichess.org/@/Schoenemann/perf/blitz)
[![lichess-rapid](https://lichess-shield.vercel.app/api?username=Schoenemann&format=rapid)](https://lichess.org/@/Schoenemann/perf/rapid)

It hase currently an Elo estimate in CCRL of about ~2680

#  Thanks
Special thanks to Disservin's [chess libary](https://github.com/Disservin/chess-library/) witch made the wohle development process simpler.
Many great people from the Stockfish discord server and EP Server.
