#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include "arguments.h"

struct arguments args;

void setDefaults() {
  args.hostname = (char*)"panoptes";
  args.port = 1777;
  args.swap = false;
  args.iterations = 100;
  args.sleep = 1000;
  args.outputImages = false;
  args.interactive = false;
  args.cathFile = NULL;
  args.transFile = NULL;
  args.volFile = NULL;
  args.userPrompt = false;
  args.setAllFile = NULL;
  args.dicomFile = NULL;
  args.movieFile = NULL;
}

const char * USAGE_TEXT = " <mode>, where <mode> = 0 for read, = 1 for write";

#endif // MAIN_H
