# ***This*** is Autocomplete - in C !
## Video Demo:  <https://youtu.be/1fAsGMYd9BE>
## Description:
This was my final project for Harvard University' s CS50x 2023 course and my first attempt to create an efficient data structure from scratch: a ***command line program in C*** that autocompletes the user' s input.
## Included files:
- ***autocomp.c :*** Source code file.
- ***dict.txt :*** Text dictionary of English words - required([source](https://github.com/dwyl/english-words/blob/master/words_alpha.txt)).
- ***README.md :*** What you' re reading right now.
## Overview:
Running with '-s' command line argument loads the dictionary into memory using a **trie** data structure. This mode is optimized for speed with an O(l) word retrieval time, where 'l' is the size of the word. The downside is that this method can only complete user input **starting** with the given word syllable(s).

Running with '-i' command line argument uses simple iteration with fscanf to check every word of the dictionary for a matching fragment. This has the advantage of returning words that also **include** the given word syllable(s) anywhere in their respective string length. The disadvantage is that it is about 8000 times slower than the trie.

## Technical stuff:
- I made use of linked lists to create the nodes of the trie, with 2 header pointers during initialization to ensure a queue type functionality of the structure.
- The original source code for the '-s' command line argument accepts ASCII characters and the apostrophe symbol **only**. Attempting to read a dictionary file with non-alphabetical entries will result in a segmentation fault and crash. It should be relatively simple to modify the code for that purpose though. Using the program with the '-i' command line argument can bypass this limitation, since it works with fscanf, although you lose the efficiency and speed of the trie data structure.
### Epilogue
I hope you find my project interesting and eventually useful. Thanks for reading!
