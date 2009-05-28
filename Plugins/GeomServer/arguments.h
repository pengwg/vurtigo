#ifndef ARGUMENTS_H
#define ARGUMENTS_H


// File used to define Data Types that will be used. 
#include <vector>
#include "GeomServerConsts.h"

struct arguments
{
  char* hostname;
  int port;
  bool swap;
  int iterations;
  int sleep;
  bool outputImages;
  char* cathFile;
  bool interactive;
  char* transFile;
  char* volFile;
  bool userPrompt;
  char* setAllFile;
  char* dicomFile;
  char* movieFile;
};

struct IMAGEDATA {
  int trig;
  int resp;
  float rotMatrix[ROT_MATRIX_SIZE];
  float transMatrix[TRANS_MATRIX_SIZE];
  int FOV;
  int imgSize;
  int numChannels;
  int oldImageSize;
  unsigned char* img;
};

struct COILDATA {
  float coords[COORDS_SIZE];
  int angles[ANGLES_SIZE];
  int locID;
  int SNR;
};

struct CATHDATA {
  int cathMode;
  int numCoils;
  std::vector<COILDATA> coils;
};


struct Point3D {
  float x;
  float y;
  float z;
};

#endif
