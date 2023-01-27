/*********************************************************************
 File:     chromakey.cpp

 Author:   Your name here

 Email address:

 Usage: program_name in.bmp background.bmp dist_threshold out1.bmp out2.bmp

 Notes:
 This program performs the chroma key operation on an input 
 using two different methods.

 Method 1 Utilize a user-defined distance threshold from the
          chromakey value as a discriminator

 Method 2 Devise a method that to determine the chromakey mask
          that doesn't require a user-input threshold

********************************************************************/

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "bmplib.h"

using namespace std;

// Prototypes
// IMPORTANT: you must exactly use these input types, function names, and 
// return types. Otherwise the grader can't test them.
void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold);

void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE]);

void replace(bool mask[][SIZE],
	     unsigned char inImage[][SIZE][RGB],
	     unsigned char bgImage[][SIZE][RGB],
	     unsigned char outImage[][SIZE][RGB]);

int main(int argc, char *argv[])
{
  // Image data array
  // Note:  DON'T use the static keyword except where we expressly say so.
  //        It puts the large array in a separate, fixed, area of memory. 
  //        It is bad practice. But useful until we have dynamic allocation.
  static unsigned char inputImage[SIZE][SIZE][RGB];
  static unsigned char bgrndImage[SIZE][SIZE][RGB];
  static unsigned char outputImage[SIZE][SIZE][RGB];
  static bool chromaMask[SIZE][SIZE];

  double threshold;

  if (argc < 6) {
    cerr << "usage: program_name in.bmp background.bmp dist_threshold " 
         << "out1.bmp out2.bmp" << endl;
    return 0;
  }
	
  if (readRGBBMP(argv[1], inputImage)) {
    cerr << "Error reading file: " << argv[1] << endl;
    return 1;
  }

  if (readRGBBMP(argv[2], bgrndImage)) {
    cout << "Error reading file: " << argv[2] << endl;
    return 1;
  }
  
  // Write code to convert the threshold (argv[3])
  //  from string format to a double and assign the 'threshold'
  threshold  = stod(argv[3]);


  // Call Method 1 Function
  method1(inputImage,chromaMask,threshold);


  // Produce the output by calling replace()
  replace(chromaMask,inputImage,bgrndImage,outputImage);


  // Write the output image to a file using the filename argv[4]
  if (writeRGBBMP(argv[4], outputImage)) {
    cout << "Error writing file: " << argv[4] << endl;
    exit(1);
  }
  else {
    writeRGBBMP(argv[4],outputImage);
  }	

  // Call Method 2 Function
  method2(inputImage,chromaMask);


  // Produce the output by calling replace()
  replace(chromaMask,inputImage,bgrndImage,outputImage);

  
  // Write the output image to a file using the filename argv[5]
  if (writeRGBBMP(argv[5], outputImage)) {
    cout << "Error writing file: " << argv[5] << endl;
    exit(1);
  }
  else {
    writeRGBBMP(argv[5],outputImage);
  }
  		

  return 0;
}



// Use user-provided threshold for chroma-key distance
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold)
{
  int chromakey_red = 0;
  int chromakey_green = 0;
  int chromakey_blue = 0;

  //find chromakey values
  for (int i  = 0; i < SIZE; i++) {
    chromakey_red += inImage[i][0][0];
    chromakey_red += inImage[0][i][0];
    chromakey_green += inImage[i][0][1];
    chromakey_green += inImage[0][i][1];
    chromakey_blue += inImage[i][0][2];
    chromakey_blue += inImage[0][i][2];
  }

  chromakey_red = chromakey_red / (2*SIZE);
  chromakey_green = chromakey_green / (2*SIZE);
  chromakey_blue = chromakey_blue / (2*SIZE);

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (sqrt(pow(chromakey_red-inImage[i][j][0],2) + pow(chromakey_green-inImage[i][j][1],2) + pow(chromakey_blue-inImage[i][j][2],2)) <= threshold) {
        mask[i][j] = 0;
      }
      else {
        mask[i][j] = 1;
      }
    }
  }


}

// Devise a method to automatically come up with a threshold
//  for the chroma key determination
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE])  
{
  int chromakey_red = 0;
  int chromakey_green = 0;
  int chromakey_blue = 0;
  double threshold = 0.0;

  //find chromakey values
  for (int i  = 0; i < SIZE; i++) {
    chromakey_red += inImage[i][0][0];
    chromakey_red += inImage[0][i][0];
    chromakey_green += inImage[i][0][1];
    chromakey_green += inImage[0][i][1];
    chromakey_blue += inImage[i][0][2];
    chromakey_blue += inImage[0][i][2];
  }

  chromakey_red = chromakey_red / (2*SIZE);
  chromakey_green = chromakey_green / (2*SIZE);
  chromakey_blue = chromakey_blue / (2*SIZE);

  //find the threshold
  for (int i  = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      threshold += sqrt(pow(chromakey_red-inImage[i][j][0],2) + pow(chromakey_green-inImage[i][j][1],2) + pow(chromakey_blue-inImage[i][j][2],2));
    }
  }

  threshold = threshold / (SIZE*SIZE);
  
  
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (sqrt(pow(chromakey_red-inImage[i][j][0],2) + pow(chromakey_green-inImage[i][j][1],2) + pow(chromakey_blue-inImage[i][j][2],2)) < threshold) {
        mask[i][j] = 0;
      }
      else {
        mask[i][j] = 1;
      }
    }
  }

  cout << threshold;
}

// If mask[i][j] = 1 use the input image pixel for the output image
// Else if mask[i][j] = 0 use the background image pixel
void replace(bool mask[SIZE][SIZE],
	     unsigned char inImage[SIZE][SIZE][RGB],
	     unsigned char bgImage[SIZE][SIZE][RGB],
	     unsigned char outImage[SIZE][SIZE][RGB])
{
  // Create the output image using the mask to determine
  //  whether to use the pixel from the Input or Background image
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (mask[i][j] == 1) {
        outImage[i][j][0] = inImage[i][j][0];
        outImage[i][j][1] = inImage[i][j][1];
        outImage[i][j][2] = inImage[i][j][2];
      }
      else {
        outImage[i][j][0] = bgImage[i][j][0];
        outImage[i][j][1] = bgImage[i][j][1];
        outImage[i][j][2] = bgImage[i][j][2];
      }
    }
  }


}
