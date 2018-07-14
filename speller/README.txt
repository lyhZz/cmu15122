15-122 Principles of Imperative Computation
Doslingos

Files you won't modify:
   lib/arr_lib.c0       - Helper functions for esaily creating test arrays
   lib/arrayutil.c0     - A version of arrayutil.c0 for strings
   lib/readfile.c0      - File reading library
   speller-awful.c0     - Badly broken implementation
   echo.c0              - Example of using the C0 args library

Files that don't exist yet:
   speller.c0           - Code for tasks 1 to 25
   speller-test.c0      - Testing speller.c0
   analysis.c0          - Using the speller code to answer questions

Data: (described on page 3 of the handout)
   texts/dict.txt
   texts/small-dict.txt
   texts/scott-tweet.txt
   texts/sloth.txt
   texts/shakespeare.txt

   You can create more data files if you would like to use them in
   your tests: just hand these in along with your other files.

==========================================================

Loading all the libraries in coin to play with them:
   % coin lib/*.c0

Compiling speller.c0 and tests:
   % cc0 -d -w -o speller lib/*.c0 speller.c0 speller-test.c0
   % ./speller

Compiling speller-awful.c0 and tests:
   % cc0 -d -w -o speller-bad lib/*.c0 speller-awful.c0 speller-test.c0
   % ./speller-bad

Compiling analysis.c0:
   % cc0 -w -o analysis lib/*.c0 speller.c0 analysis.c0
   % ./analysis texts/dict.txt texts/shakespeare.txt

==========================================================

Submitting with Andrew handin script:
   % handin speller speller.c0 speller-test.c0 analysis.c0

Creating a tarball to submit with autolab.andrew.cmu.edu web interface:
   % tar -czvf handin.tgz speller.c0 speller-test.c0 analysis.c0
