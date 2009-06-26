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
  char* cathFile;
  char* dicomFile;
};

struct IMAGEDATA {
  int trig;
  int resp;
  float rotMatrix[ROT_MATRIX_SIZE];
  float transMatrix[TRANS_MATRIX_SIZE];
  int FOV;
  int imgSize;
  int numChannels;
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

#endif
