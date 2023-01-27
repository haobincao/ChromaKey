# PR2

# ***2. Background and Notes***

## **Background Information and Notes**

Image processing is a major subfield of computer science and electrical engineering and to a lesser extent biomedical engineering. Graphics are usually represented via two methods: vector or bitmap. Vector graphics take a more abstract approach and use mathematical equations to represent lines, curves, polygons and their fill color. When a program opens the vector image it has to translate those equations and render the image. This vector approach is often used to represent clipart and 3D animations. Bitmap images take the opposite approach and simply represent the image as a 2D array of pixels. Each pixel is a small dot or square of color. The bitmap approach is used most commonly for pictures, video, and other images. In addition, bitmaps do not force us to translate the vector equations, and thus are simpler to manipulate for our purposes.

Color bitmaps can use one of several different color representations. The simplest of these methods is probably the RGB color space (HSL, HSV, & CMYK are others), where each pixel has separate red, green, and blue components that are combined to produce the desired color. Usually each component is an 8-bit (`unsigned char` in C++) value. Given 3-values this yields a total of 24-bits for representing a specific color (known as 24-bit color = 224224 = 16 million unique colors). Storing a red, green and blue value for each pixel can be achieved using 3 separate 2D arrays (one for each RGB component) or can be combined into a 3D array with dimensions `[256][256][3]` as shown below (Note: BMP image files use a simple format like this one and thus will be the file format used in our assignment.)

| Color | RGB value |
| --- | --- |
| White | 255,255,255 |
| Red | 255,0,0 |
| Yellow | 255,255,0 |
| Orange | 255,128,0 |
| Blue | 0,0,255 |
| Green | 0,255,0 |
| Magenta | 255,0,255 |
| Black | 0,0,0 |

![https://domainmineral-adrianjet.codio.io/.guides/img/chromakey-fig1-rgb.png](https://domainmineral-adrianjet.codio.io/.guides/img/chromakey-fig1-rgb.png)

*Figure 1 - Representation of color images*

The primary task in performing the chroma key operation is to determine which pixels of the image should be classified as the chroma key (green). Part of the problem is that shadows and other artifacts may make the background pixels significantly lighter or darker shades of the key color, especially the closer they are to the foreground image. We would like an algorithm that is robust enough to classify these outlier pixels correctly without incorrectly classifying pixels of the foreground image. Whatever method we use, the goal is to create a 2D “mask” array that marks foreground image pixels (say with a 1) vs. chroma key pixels (with a 0). Once this mask is created, we can easily create the output image by using the mask to select pixels from the input or background image. This process is shown in Figure 2.

![https://domainmineral-adrianjet.codio.io/.guides/img/chromakey-fig2-mask.png](https://domainmineral-adrianjet.codio.io/.guides/img/chromakey-fig2-mask.png)

*Figure 2 - Chroma-key process*

Many methods could be devised for determining whether a pixel is part of the chroma key. One may think of simply checking if a pixel’s green component is above a certain value. However, usually this does not work because light colors like white and gray are represented as the superposition of all three RGB colors [i.e. white = (255,255,255)].

A better approach may be to think of each pixel’s RGB components as a coordinate (vector) of a point in 3D space. Next, take some number of sample pixels that we would know to be the chroma key color (ones near the edges for example). We can average them to create a reference point that represents the average chroma key color. Then any pixel (RGB point) within a certain distance from the reference can be considered part of the chroma key while pixels (RGB points) further away will be considered part of the foreground image. This method can work for any chroma key but does require some trial and error to set the threshold distance for which pixel values will be determined to be the chroma key or foreground image. You will need to experiment with this in your lab. [Note: The example image on the first page was created using this method.]

Many other methods could also be devised. One desirable feature that would make the algorithm more robust is automatic threshold determination (i.e. no user-interaction). An “adaptive” algorithm that can determine the parameters from the image values itself is desirable. Consider this feature when you create your own method.

![https://domainmineral-adrianjet.codio.io/.guides/img/chromakey-fig3-rgbspace.png](https://domainmineral-adrianjet.codio.io/.guides/img/chromakey-fig3-rgbspace.png)

*Figure 3 - RGB Color Space and Distance Determination*

### **C++ Language Implementation**

Some features of C that we will utilize include:

**2- and 3-D Arrays**: We will store the mask and images in 2- and 3-D arrays, respectively. Recall the declaration and access syntax:

**`int** example2d[10][5];
**unsigned** **char** array3d[256][256][3];

x = example2d[9][4];    *// accesses element in the bottom row, last column*
pixel_red = array3d[255][255][0];  *// access lower, right pixel’s red value*`

Note that each of the RED, GREEN, and BLUE components of a pixel is an 8-bit value, logically between 0 and 255. Thus, we will use the `unsigned char` type to represent these numbers.

Also, the library functions that we will provide use the convention that the **first index is the row** and the **second index is the column** (with the **third index being the R,G, or B plane**.)

**Math functions**: You will likely need the ability to compute the square root of a number. This `sqrt` function is part of the `cmath` library.

**BMP Library & File I/O**: For this lab, we will provide you predefined functions that you can use to read in (open) and write out (save) `.bmp` image files. These functions are given in the compiled library `bmplib.o` and are prototyped in the header file `bmplib.h`. Be sure to include `bmplib.h` in your program by adding the following line with the other #include statements.

`#include "bmplib.h"`

The functions you you will use are `readRGBBMP()` and `writeRGBBMP()`. Prototypes are shown below. You must pass each function a character array (text string) of the filename you wish to read/write and a 256x256x3 array of unsigned char’s (8-bit values) that represent the data to be read/written.

**`int** **readRGBBMP**(**char** filename[], **unsigned** **char** inputImage[][256][3]);
**int** **writeRGBBMP**(**char** filename[], **unsigned** **char** outputImage[][256][3]);`

Note: These functions return 0 if successful and non-zero if they cannot open, read, or write the particular file.

**Command Line Arguments**: Rather than prompting the user during your program to enter the files you will process, we will pass the filenames to your program as *command line arguments*. Command line arguments provide a way to pass a program some initial input values without having to prompt the user explicitly when your program executes. Most programs provide this kind of feature. E.g. in Windows from the Start…Run box, type `notepad mydoc.txt`. This will start notepad and attempt to open a file named mydoc.txt without requiring you to use the GUI interface. Your OS provides this ability by parsing the command line when you start your program and passing the additional command line words as arguments: `int argc` and `char *argv[]` to the `main()` routine.

**`int** **main**(**int** argc, **char** *argv[]) { ... }`

The `argc` value is an integer indicating how many command line arguments were entered (note that the executable program name is included in the count, so `argc` will always be at least one.) The `argv` argument is an array of character strings. For example if we run

`$ ./chromakey input.bmp bgnd.bmp 40.5 output1.bmp output2.bmp`

Then

`argc is 6
argv[0] is "./chromakey"
argv[1] is "input.bmp"
argv[2] is "bgnd.bmp"
argv[3] is "40.5"
argv[4] is "output1.bmp"
argv[5] is "output2.bmp"`

Note: Numeric arguments such as `40.5` are passed in as character (text) strings and need to be converted to the appropriate numeric types before operated upon. This can be accomplished with functions like `atoi` (ASCII to Integer) or `atof` (ASCII to floating point) which are defined in `<cstdlib>` and whose prototypes are shown below.

*`// returns the integer value of the number represented by//  the character string str***int** **atoi**(**char** *str);  

*// returns the double floating point value of the number//  represented by the character string str***double** **atof**(**char** *str);`

As an example, the `"40.5"` argument can be converted by:

**`double** x;
x = atof(argv[3]);`

**Multi-file compilation**: Most real-world program contain more than one source code file and thus require the compiler to generate code and then link several files together to produce an executable. In this lab, we have provided `bmplib.h` and `bmplib.o`. `bmplib.o` is an *object file* (`.o` extension) representing the compiled (but not linked) functions to perform .BMP image I/O. It is not a text file but binary instructions and memory initialization commands for the Linux operating system that Codio runs (it won’t work if you download it to your own Windows/Mac host).

An object file can be created from a C++ file by using the `-c` extension to the compiler.

`$ g++ -g –Wall –c bmplib.cpp`

This command will create the `bmplib.o` file (if you have the `.cpp` source, which you don’t).

`bmplib.h` is a header file that includes prototypes and other declarations that you can include into your C code that will allow you to call the functions in `bmplib.o`. To compile your code with the BMP functions and then link them together you could run:

`$ g++ -g –Wall –o chromakey bmplib.o chromakey.cpp`

You can list any number of .o files and C files on the command line. The C files will be compiled and then linked together with all of the .o files specified producing an executable as output.

**`make` and `Makefiles`**: As more files become part of your program, you will not want to compile EVERY file again when you make a simple change to one file. However, keeping track of which files have changed and thus require recompilation can also become difficult. Enter the `make` utility. This program takes as input a text file usually named `Makefile` which includes command that identify the order dependencies of files on each other (i.e. if file 1 changes, then it may require re-compiling file 2 and file 3) and the commands to perform the compilation. It will then examine the modification times of the files and determine which files have changed and only compile those. It also acts as a script of compilation commands so you do not have to type in the `g++` command line each time. For this program we will use make and provide you a Makefile shown below:

`CC = g++
CFLAGS = -g -Wall
SRCS =  chromakey.cpp bmplib.cpp
OBJ = $(SRCS:.cpp=.o)
TARGET = chromakey

all: chromakey

chromakey: chromakey.o bmplib.o
    $(CC) $(CFLAGS) chromakey.o bmplib.o -o chromakey

chromakey.o: chromakey.cpp bmplib.h
    $(CC) $(CFLAGS) -c chromakey.cpp

bmplib.o: bmplib.cpp bmplib.h
    $(CC) $(CFLAGS) -c bmplib.cpp

clean:
    rm -f chromakey.o chromakey *~`

In general, a `Makefile` is made of rules that indicate which files they are dependent upon. Each rule then has a set of commands that should be run when any dependency file changes.

`rulename:  dependency1 dependency2 …
  <TAB>  command1
  <TAB>  command2…`

Typing `make` at the command line will run the `all` target (this is the default rule to run) which depends on the `chromakey` executable target. The `chromakey` executable target depends on `chromakey.o` and `bmplib.o`. Thus, if those files have changed it will trigger the commands under the `chromakey` target to run. But the `.o` files each have their own dependencies which are their respective source (`.cpp`) files and header files. Thus, if we had changed our `chromakey.cpp` file, it would first be compiled to an object file using `chromakey.o` rule and, in turn, trigger the `chromakey` command to run and generate the executable. All of this happens by typing `make` and without your interaction. Finally, typing `make clean` at the command prompt will run the rules associated with that `clean` target. The associated command will remove the files listed to “clean up” the directory. Having a `clean` rule is customary to remove all the intermediate compilation files from a directory.

More information about the `make` utility and Makefiles can be found at:[here](https://www.gnu.org/software/make/manual/make.html) and [here](https://www.tutorialspoint.com/makefile/index.htm)

# ***3. Procedure***

## **Procedure**

### **Before You Begin**

The first task will be to better understand an example image, `colbert_gs.bmp` that we provide. Find that file in the Filetree and download it to your laptop (you can just right-click and choose `Download`).

You can use [this online tool](https://redketchup.io/color-picker) (or another tool you are familiar with) to view individual pixel locations and RGB values.

![https://domainmineral-adrianjet.codio.io/.guides/img/chromakey-fig4-colorpicker.png](https://domainmineral-adrianjet.codio.io/.guides/img/chromakey-fig4-colorpicker.png)

*Figure 4 - RGB pixel color picker*

On that website, choose and open the image from your laptop that you just downloaded. You can then click and drag your mouse over portions of the image to a specific `x`/`y` location (**note the difference that `x` is the column and `y` is the row**). You can zoom in if it is easier to find particular locations. If you scroll down you’ll also see the Red, Green, Blue values for the given pixel.

Let’s record a few RGB values that we can use to help us in part 1 of the procedure below.

Position your mouse cursor over the following pixel locations and record their RGB values:

- Top left green-screen pixel (background / chroma-key)
- Any of the flesh colored pixels on Stephen’s arm.
- Any of the pixels on his dark vest
- Any of the pixels on his white shirt

Compute the distance between the background (chroma-key) pixel and these three pixels (arm, vest, shirt). This may help in identifying a suitable threshold value for part 1.

| Pixel Location | RGB value | Distance to Chromakey |
| --- | --- | --- |
| Upper-left (background/chroma-key |  | N/A |
| Arm |  |  |
| Vest |  |  |
| Shirt |  |  |

If you would like to create and use your own green screen or background images, you can take any image `.jpg`, `.png`, etc. and use any image software that allows resizing (to 256x256) to convert it to a `.bmp` file. **[This online converter](https://online-converting.com/image/convert2bmp/)** works well enough. To use it, just upload/drag your green-screen image file, then below ensure you:

- Set **Dithering** to `No`
- Modify the new size text boxes to read  by  (for the height and width)
    
    256
    
    256
    
    256
    
    256
    
- Uncheck **Keep Proportions**

![https://apollo-media.codio.com/media%2F1%2Ffa0b919cf71d4fb4ae95329fb8bac47b-afe99c7339788f36.webp](https://apollo-media.codio.com/media%2F1%2Ffa0b919cf71d4fb4ae95329fb8bac47b-afe99c7339788f36.webp)

- You can keep the remaining settings and it will automatically start converting your file.
- You can then save the file using the Download link just below where you uploaded the file to save the new file to your laptop.
- From there, you can find the image file on your laptop (using Explorer or a Finder window) and drag it to the File-Tree in Codio to upload it to the Codio workspace.
- You are now ready to use it as an input image by passing its name as the input file command line argument.

Please note: Images whose aspect ratio is roughly square will work the best.

### **Method Overview**

You must implement **two different methods** for chroma key detection. Both of them share the same common step, which is to use a mask produced by the earlier portion of each method to select pixels from the input image or background image for use in the output.

**`void** **replace**(   
  **bool** mask[][SIZE],      
  **unsigned** **char** inImage[][SIZE][RGB],
  **unsigned** **char** bgImage[][SIZE][RGB], 
  **unsigned** **char** outImage[][SIZE][RGB]);`

`replace()` uses a 2D `mask` (that you will produce earlier in each method), which should indicate whether a particular location/pixel in the output image (`outImage`) should come from the foreground (green-screen) image (`inImage`) (when the corresponding `mask` bit is 1) or the background image (`bgImage`) (when the corresponding `mask` bit is 0).

You will write two functions for mask creation that use slightly different approaches. The first one, `method1` must follow the process outlined below to ensure the same results can be obtained and checked from all students.

**`void** **method1**(   **unsigned** **char** inImage[][SIZE][RGB], 
                **bool** mask[][SIZE], **double** threshold);`

Specifically, `method1` must:

- determine the chromakey (background color) by averaging the red, green, and blue values, respectively, from the top-most row and left-most column of the image.
- Use the chromakey as a reference point, and scan through the entire image comparing the RGB distance of each pixel to the threshold value provided on the command line and classify the pixel as background (setting the corresponding `mask` value to `0`) or foreground (setting the corresponding `mask` value to `1`).

must take `inImage` determine the chroma-key (background) color by averaging sample points (assume a reasonable number of top rows, left columns, and right columns will be background), classify all points as either foreground or background, and write the resulting classification to mask. The classification of pixels must be according to their whether their RGB distance from the background color is greater or less than threshold, which corresponds to a command-line input the program takes.

Note that `method1()` just *creates the mask*, so `main()` will have to use `replace()` in conjunction with the `method1()` function.

`method2()` has a similar signature,

**`void** **method2**(**unsigned** **char** inImage[][SIZE][RGB], **bool** mask[][SIZE]);`

except that it

- may not use the hard-coded threshold from the command line and
- may use additional areas of the image to determine the chromakey (i.e. you may “assume” a reasonable number of top rows along with left and right columns are background, using the sample green screen images as guides). So you may choose to still average some amount of background pixels to find the chroma-key but then would need to determine an automated scheme for identifying a threshold based only on the input image data itself. Alternatively, you may create and use some other means to classify the pixels of the input image as background (chroma-key) or foreground but it must be derived from the input image data itself (i.e. no use of an input argument, like the threshold from method 1). You can use any approach you like – just make sure to document it!

When you run your program it will run both methods and so produce two new `.bmp` files. Specifically, the command-line arguments will be

`./chromakey input.bmp bgnd.bmp THRESH output1.bmp output2.bmp`

with the arguments meaning the input file, the background file, the numerical threshold (e.g. 34.5 or 57.2), the first output file, and the second output file.

### **Procedure**

1. Examine the starter files
    - `chromakey.cpp`: Skeleton code file to complete
    - `bmplib.h`: Header file to include into `chromakey.cpp`
    - `bmplib.o`: Compiled BMP library implementation
    - `Makefile`: Script to compile your code to an executable named `chromakey`
    - `colbert_gs.bmp`, `bird_gs.cmp`, `astro.bmp`, `faded.bmp`: Input (green-screen) images for testing
    - `campus1.bmp`, `village.bmp`: Background images for testing
2. Edit the `chromakey.cpp` file taking note of the comments embedded in it. You will implement `replace()`, `method1()`, `method2()`, and `main()`. In addition to prototypes for the methods already mentioned, in `main()` we provide some starter code. When calling the read and write BMP file functions, note that we check the return value to ensure successful operation. If one of these functions ever returns a non-zero value, exit the program immediately using `return 1;`.
3. Review how to pass an array to a function. You will need this when you write your calls to method1(), method2(), and replace().
4. For **method 1** you must implement the distance method described earlier earlier in the lab. To find the "chromakey", we will need to find the average background color by iterating over some set of pixels in the input image. For this method, you **must use only the 0-th row and 0-th column (using pixel 0,0 in each)** to find the chromakey (i.e. average R,G,B values). Then iterate over the entire image computing the distance between each pixel and the chromakey value, and setting the mask array appropriately. Use the distance threshold that the user entered at the command line. You will need to determine a good threshold value by repeatedly re-running your program and examining the resulting `o1.bmp` file (or you can use different names for the outputs based on the input image, such as `colbert1.bmp` / `bird1.bmp` for `colbert_gs.bmp` and `bird_gs.bmp`, respectively).
5. At this point you can complete the `replace()` function and `main()` function, compile, and run your code to test whether your `method1()` worked, and experiment with different distance thresholds to find one that removes as much of chromakey (background/green) pixels as possible without also removing parts in the foreground image.
6. For **method 2** you will need to devise another method that can perform the chromakey operation without using any user-defined input (i.e. the threshold) or is keyed on the specific input image. Rather it must only use the input image pixel values and be generic to work similarly for any input image. You can either use a similar approach to method 1 but with a technique you devise to find a threshold automatically or a completely new approach (but it should not require user input and easily be able to run in C++ on Codio and pass our test suite). As you find the the “chromakey” (average R, G, B), you may use more than the 0th row and column. Determine which set of pixels (location of pixels) seem “safe” to assume are background in any green-screen image and then you can iterate over them to find the "chromakey".

You are NOT allowed to hard code a constant that was determined by examining pixels on a specific image. We will look for this and take a major deduction if you do. You may use a constant (scale or fudge factor that adds tolerance to your approach) if it would likely work well for any image that might be input.

1. Be sure you ALWAYS compile your program by using `make` since it will need to compile together both `bmplib.o` as well as `chromakey.cpp`.
2. Test your program with the following command line arguments:

`$ ./chromakey colbert_gs.bmp campus1.bmp threshold_num o1.bmp o2.bmp`

- `o1.bmp` should be the output of method 1
- `o2.bmp` should be the output of method 2

You can view each file by simply clicking on it in the File-tree and it will bring up a tab with the image.

## **Refreshing an Image**

If you rerun your program (i.e. to test it again and overwrite your old images), the tab with the image will **NOT** refresh automatically, **NOR will clicking on the image filename** in the File tree again.

You must **close the tab with the image** and then click on the file again to have the tab re-open and refresh.

Other tests can be performed by using the `bird_gs.bmp` file and `astro.bmp`. Note: `astro.bmp` is much HARDER due to irregularity in the background. You don’t have to handle it perfectly to get a full score.

To view your output, just click on the files that are output by your program in the File-Tree view. Codio should display them (albeit small).

### **Troubleshooting**

Before you post a question or seek help make sure you:

1. Can view the images you downloaded. If you have problems with gqview make sure you are in the right directory where your images are located.
2. Try printing out some pixel values to ensure you are reading in the input images correctly. Just be warned if you want to print out the numeric values of pixels, you’ll need to cast to an integer first, as in:

`cout << (**int**)inputImage[0][0][0] << endl;`

This is because otherwise, the image arrays are unsigned chars which will print out as actual ASCII characters.

1. Try using `cout` to print out the chromakey R,G,B value you find to make sure it is sensible.
2. Try using the debugger to run through and examine your values.

**Using Codio integrated debugger:**

Due to the need for command line arugments, we have had to pre-configure to debug options. You can run with `colbert_gs.bmp` as your input and a threshold of `100` or with `bird_gs.bmp` and a threshold of 100. Use the dropdown Debugger menu at the top to choose which input you’d like to debug.

**Using command line `gdb` debugger:**

When you want to debug a program that needs command line arguments (like ours which requires you to pass it the name of the input files, the threshold, etc.) when you start the program, as in:

`$ gdb ./chromakey 
<gdb license and notice>
(gdb) run colbert_gs.bmp campus1.bmp 40.5 output1.bmp output2.bmp`