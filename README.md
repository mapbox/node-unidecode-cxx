# unidecode-cxx

This is a weekend experiment at porting [node-unidecode](https://github.com/FGRibreau/node-unidecode) to C++. All of node-unidecode's data files are compiled statically into the binary, and more is done with lookup tables rather than branches.

So far it passes all of unidecode's tests except one (it doesn't handle null characters in the same way), and is about twice as fast. It definitely hasn't been battle-tested, though, and in particular, I have no idea how it will handle invalid unicode.