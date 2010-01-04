/*******************************************************************************
    Vurtigo: Visualization software for interventional applications in medicine
    Copyright (C) 2009 Sunnybrook Health Sciences Centre

    This file is part of Vurtigo.

    Vurtigo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#ifndef ARGUMENTS_H
#define ARGUMENTS_H


// File used to define Data Types that will be used. 
#include <vector>
#include "GeomServerConsts.h"

struct arguments
{
  char * hostname;
  int port;
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
  int arraySize;
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
