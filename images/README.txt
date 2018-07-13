15-122 Principles of Imperative Computation
Images

==========================================================

Files you will modify:
   imageutil.c0       - Skeleton image helper functions

Files you won't modify:
   remove-red.c0      - You can modify this to respect the pixel/imageutil
                        interfaces if you'd like
   remove-red-main.c0 - Runs remove_red()
   remove-red-test.c0 - Some unit tests for remove_red()
   reflect-main.c0    - Runs reflect()
   blur-main.c0       - Runs blur()

Files that don't exist yet:
   pixel.c0           - Copy this over from Programming 1 (Pixels)
   reflect.c0         - Task 3
   blur.c0            - Task 4
   images-test.c0     - test your manipulations (optional)

Data:
   blur-slightly-mask.txt - 3x3 blur mask
   blur-mask.txt - 5x5 blur mask
   blur-more-mask.txt - 5x5 blur mask
   images/g5.png
   images/carnegie.png
   images/scs.png
   images/cmu.png
   images/tinytestpattern.png - 3x2 image, may be good for testing
   images/g5-remove-red.png - result of remove_red()
   images/g5-remove-red-bug.png - result of a buggy remove_red()
   images/carnegie-reflect.png - result of reflect()
   images/scs-blur-slightly.png - result of blur with blur-slightly-mask.txt
   images/scs-blur.png - result of maskblur with mask blur-mask.txt

==========================================================

Using the imagediff utility: you have been given the result of running
remove_red on both a correct and a buggy implementation of
g5.png. Running the following command shows that the two images differ
by 600 pixels.

   % imagediff -i images/g5-remove-red.png -j images/g5-remove-red-bug.png
   Loaded image images/g5-remove-red.png. Dimensions are 800 by 600.
   Loaded image images/g5-remove-red-bug.png. Dimensions are 800 by 600.
   Number of pixels with different colors: 600 out of 480000.
   0

If you want to visually see where these 600 different pixels actually
are, you can get imagediff to print out a new image that highlights
the places where differences occured:

   % imagediff -i images/g5-remove-red.png -j images/g5-remove-red-bug.png -o diff.png
   % display diff.png &

In addition to writing test cases, this can be a useful way of
debugging your code.

==========================================================

Compiling remove-red and test cases:
   % cc0 -d -w -o remove-red-test pixel.c0 imageutil.c0 remove-red.c0 remove-red-test.c0
   % ./remove-red-test

   % cc0 -d -w -o remove-red pixel.c0 imageutil.c0 remove-red.c0 remove-red-main.c0
   % ./remove-red -i images/g5.png -o images/g5-my-output.png
       (This creates the output file images/g5-my-output.png. If you
        left off the "-o images/g5-my-output.png" part, the output
        file would be called images/g5_remove-red.png.)
Compiling your unit tests. You can omit reflect.c0 or blur.c0 if you haven't written test cases for reflect() or blur(), respectively:
   % cc0 -d -w -o images-test pixel.c0 imageutil.c0 reflect.c0 blur.c0 images-test.c0
   % ./images-test

Compiling reflect:
   % cc0 -d -w -o reflect pixel.c0 imageutil.c0 reflect.c0 reflect-main.c0
   % ./reflect -i images/carnegie.png -o images/carnegie-my-output.png
       (This creates the output file images/carnegie-my-output.png. If
        you left off the "-o images/carnegie-my-output.png" part, the
        output file would be called images/carnegie_reflect.png.)
   % imagediff -i images/carnegie-reflect.png -j images/carnegie-my-output.png

Compiling blur:
   % cc0 -d -w -o blur pixel.c0 imageutil.c0 blur.c0 blur-main.c0
   % ./blur -i images/scs.png -o images/scs-my-blur.png -m blur-mask.txt
       (This creates the output file images/scs-my-blur.png. If you
        left off the "-o images/scs-my-blur.png" part, the output
        file would be called images/scs_blur.png.)
   % imagediff -i images/scs-blur.png -j images/scs-my-blur.png

Compiling manipulate:
   % cc0 -d -w -o manipulate pixel.c0 imageutil.c0 reflect.c0 blur.c0 manipulate.c0 manipulate-main.c0
   % ./manipulate -i images/g5.png -o images/g5-my-manip.png

==========================================================

Submitting with Andrew handin script:
   % handin images imageutil.c0 reflect.c0 blur.c0 images-test.c0 manipulate.c0

Creating a tarball to submit with autolab.cs.cmu.edu web interface:
   % tar -czvf handin.tgz imageutil.c0 reflect.c0 blur.c0 images-test.c0 manipulate.c0
