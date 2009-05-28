#include "Main.h"
#include <stdlib.h>
#include <iostream>

#include "sender.h"
#include "readOnlyMode.h"
#include "writeOnlyMode.h"

using namespace std;

int main (int argc, char** argv) {
  if (argc > 2) {
      cerr << "Usage: " << argv[0] << USAGE_TEXT;
      exit(-1);
  }

  GeometrySender sender;
  GenericMode * mode = new WriteOnlyMode();

  setDefaults();

  // Try to connect.
  if (!sender.connect(args.hostname,args.port,args.swap)) {
    cerr << "Error connecting to geometry Server. Check that geometry server is running on port " << args.port << " on host: " << args.hostname << endl;
    cerr << "GeomTester exiting..." << endl;
    return 0;
  } else {
    cout << "Connection with: " << args.hostname << ":" << args.port << " established." << endl;
  }

  mode->init(&sender, &args);
  mode->runMode();
}
