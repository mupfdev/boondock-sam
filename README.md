# Boondock Sam
## About

Boondock Sam is a classic 2D platformer game in the spirit of the 1990s
written in C using SDL2 and the
[TMX Map Format](http://doc.mapeditor.org/en/stable/reference/tmx-map-format/)
(Tile Map XML).

Last but not least: follow-on project of

[Rainbow Joe](https://github.com/mupfelofen-de/rainbow-joe) which I do
not wanna develop any further.

## Why C?

A question I hear alot is why I'm using C for this project instead of a
modern language like Rust, Haskell or `[insert your preferred language
here]`, because writing games in C is neither a easy thing to do nor
very common (these days).

The main reason is simple: to get exercise in C programming.  And
writing a game in C seemed like a fun way of getting some.  Even though
I am currently trying to get on with Haskell, I still consider C a fun
and challenging language.

Anyway: I hope you find the code of this project useful.  If so, feel
free to use it in any way you want. Just consider buying me a beer in
case we meet someday.

## Dependencies and how to compile

The program has been successfully compiled with the following libraries:
```
libxml2    2.9.8
sdl2       2.0.8
sdl2_image 2.0.3
sdl2_mixer 2.0.2
zlib       1.2.11
```

To compile _Boondock Sam_ under Linux simply enter:
```
make
```

If you're on NixOS enter:
```
nix-shell --command make
```

To generate the documentation using doxygen enter:
```
doxygen
```

## License and Credits

This project is licenced under the "THE BEER-WARE LICENCE".  See the
file [LICENCE.md](LICENCE.md) for details.

[TMX C Loader](https://github.com/baylej/tmx/) by Bayle Jonathan is
licenced under a BSD 2-Clause "Simplified" Licence.  See the file
[COPYING](src/tmx/COPYING) for details.

[inih](https://github.com/benhoyt/inih) by Ben Hoyt is licensed the New
BSD licence.  See the file [LICENSE.txt](src/inih/LICENSE.txt) for
details.

[Jungle Asset Pack](https://jesse-m.itch.io/jungle-pack) by Jesse M.
