#include "GeomServerPlugin.h"
#include <iostream>
#include "readOnlyMode.h"

using namespace std;

//TODO: No constructor? (use init and cleanup)
//TODO: How to generate doxygen docs (make doc)

void GeomServerPlugin::objectModified(int objID) {
  //TODO: objectModified()?
}

void GeomServerPlugin::setDefaults() {
  args.hostname = (char*)"panoptes";
  args.port = 1777;
  args.swap = false;
  args.iterations = 1;
  args.sleep = 0;
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

bool GeomServerPlugin::connectAndMessage(GeometrySender * sender) {
  if (!sender->connect(args.hostname,args.port,args.swap)) {
    char * server = "Geometry Server";
    cerr << "Error connecting to " << server <<". Check that " << server << " is running on port " << args.port << " on host: " << args.hostname << endl;
    return false;
  } else {
    cout << "Connection with: " << args.hostname << ":" << args.port << " established." << endl;
    return true;
  }
}

bool GeomServerPlugin::init() {
  //Setup m_mainWidget
  //TODO: What QWidget subclasses recommended? (Look at QLayout, QButton, QText...)
  //Setup and connect
  //TODO: Initial retriveInfo()? (no, have the user specify server/port)
  setDefaults();
  sender = new GeometrySender();
  mode = new ReadOnlyMode();

  connectAndMessage(sender);

  mode->init(sender, &args);
  setUpdateTime(UPDATE_TIME);
  return true;
}

void GeomServerPlugin::cleanup() {
  delete sender;
//  delete mode;
  std::cout << "Server out " << std::endl; //for testing
}

//TODO: guess i take the GeometrySender (sender.cpp/.h in tester) code. how do i compile with geom_client.h? (hard code for now...)
//TODO: what does the server store exactly? (just one stream of "video"?) (multiple planes, catheters. use: setPlaneID)

//TODO: What triggers retrieveInfo()? in update()? set m_updatetime too? (update) (yes, set update time)
void GeomServerPlugin::retrieveInfo() {
  //get info
  mode->runMode();

  //TODO: Guess there is only one obj right? (no, map it)
  //if (!objExists()) addObjectOfType();
  //update object
}

void GeomServerPlugin::update() {
    std::cout << "Updating" << std::endl; //for testing
//    retrieveInfo();
}

void GeomServerPlugin::point3DSelected(double px, double py, double pz, int intensity) {

}

//TODO: Assume GUI triggers this function? (yes, signals and slots QT)
bool sendInfo(/*info parm*/) {
  //change param to sendable obj

  //TODO: Assume send a rtObjectType::OT_Cath or ImageData?? (yes these two)
  //return true or false
}

// This last line will export the plugin.
Q_EXPORT_PLUGIN2(GeomServer, GeomServerPlugin)
