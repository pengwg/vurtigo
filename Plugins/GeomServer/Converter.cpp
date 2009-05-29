#include "Converter.h"
#include <iostream>
#include <vector>

#include "rtBaseHandle.h"

#include "readOnlyMode.h"

#define REMOTE_CATH_ID 1 //only one Cathater for now, thus a hardcorded id
#define NULL_ID -1 //id returned if not found

//#ifndef DEBUG_CONVERTER
//#define DEBUG_CONVERTER
//#endif

using namespace std;

void Converter::printAllLocalCath() {
  cout << "-------------------------------- VURTIGO START PRINT" << endl;
  cout << "Number of Cathaters: " << remoteCathMap->size() << endl;

  int cathCounter = 0;
  int coilCounter;
  QHash<int, rtCathDataObject::CathCoilData> * coils;
  rtCathDataObject::CathCoilData currCoil;
  for (NodeMap::iterator cathIt = remoteCathMap->begin(); cathIt != remoteCathMap->end(); cathIt++) {
    cout << "Cath Number: " << cathCounter++ << endl;

    //cast check
    coils = ((rtCathDataObject *) rtBaseHandle::instance().getObjectWithID(cathIt->first))->getListHandle();
    coilCounter = 0;
    for (QHash<int, rtCathDataObject::CathCoilData>::iterator coilIt = coils->begin(); coilIt != coils->end(); coilIt++) {
      currCoil = coilIt.value();
      cout << "Coil Number: " << coilCounter++
              << " locationID: " << currCoil.locationID
              << " SNR: " << currCoil.SNR << endl;
      cout << "Coords: [" << currCoil.cx << ", " << currCoil.cy << ", " << currCoil.cz << "]" << endl;
      cout << "Angles: [" << currCoil.angles[0] << ", " << currCoil.angles[1] << "]" << endl;
    }
  }

  cout << "-------------------------------- VURTIGO END PRINT" << endl;
}

Converter::Converter() {
  localCathMap = new NodeMap();
  remoteCathMap = new NodeMap();
}

Converter::~Converter() {
  delete localCathMap;
  delete remoteCathMap;
}

Converter::Node * Converter::getRTObjNode(int id, NodeMap * localMap, NodeMap * remoteMap, rtConstants::rtObjectType objType, char * name) {
#ifdef DEBUG_CONVERTER
  cout << "Converter::getRTObjNode()" << endl;
#endif
  Node * node = getNode(id, localMap);
  if (node == NULL && objType != rtConstants::OT_None && name != "") {
      rtDataObject * obj = rtBaseHandle::instance().getObjectWithID(rtBaseHandle::instance().requestNewObject(objType, name));
      node = new Node(id);
      remoteMap->insert(NodePair(obj->getId(), node));
      node = new Node(obj->getId());
      localMap->insert(NodePair(id, node));
  }
  #ifdef DEBUG_CONVERTER
    cout << "Converter::getRTObjNode() out" << endl;
  #endif
  return node;
}

//given the parameters, it searches in the node map with the id  and is able to add new objects int eh core if the
//last two paramters are specified
Converter::Node * Converter::getNode(int id, NodeMap * nodeMap) {
  NodeMap::iterator foundNode = nodeMap->find(id);
  if (foundNode != nodeMap->end()) {
#ifdef DEBUG_CONVERTER
        cout << "Converter::getNode() - found" << endl;
#endif
      return foundNode->second;
  }
#ifdef DEBUG_CONVERTER
      cout << "Converter::getNode() - not found" << endl;
#endif
  return NULL;
}

int Converter::getRemoteCathId(int localId) {
  Node * node = getNode(localId, remoteCathMap);
  if (node != NULL) {
    #ifdef DEBUG_CONVERTER
      cout << "Converter::getRemoteCathId() - found" << endl;
    #endif
    return node->currId;
  }
  else {
    #ifdef DEBUG_CONVERTER
      cout << "Converter::getRemoteCathId() - not found" << endl;
    #endif
    return NULL_ID;
  }
}

int Converter::getRemoteCoilId(int localCathId, int localCoilId) {
  //find the node for the remote cath
  Node * node = getNode(localCathId, remoteCathMap);
  if (node != NULL) {
    //if remote cath is found, search its NodeMap for the coil
    NodeMap::iterator foundNode = node->nodeMap->find(localCoilId);
    if (foundNode != node->nodeMap->end()) {
      return foundNode->second->currId;
    }
  }
  return NULL_ID;
}

//gets the local cath does the internal casting
rtCathDataObject * Converter::getLocalCath(int remoteId) {
  Node * node = getRTObjNode(remoteId, localCathMap, remoteCathMap, rtConstants::OT_Cath, "Cath");
  rtDataObject * obj = NULL;
  if (node != NULL)
    obj = rtBaseHandle::instance().getObjectWithID(node->currId);

  //Cast Check?
  return (rtCathDataObject *) obj;
}

int Converter::getLocalCoilId(int remoteCoilId, rtCathDataObject * localCath, int remoteCoilLoc) {
#ifdef DEBUG_CONVERTER
  cout << "Converter::getLocalCoilId()" << endl;
#endif
  int remoteCathId = getRemoteCathId(localCath->getId());

  //remote cath not found
  if (remoteCathId == NULL_ID) {
    #ifdef DEBUG_CONVERTER
      cout << "Converter::getLocalCoilId() not remote cath, return null id: " << NULL_ID << endl;
    #endif
    return NULL_ID;
  }

  //get node for the local cath to get the corresponding local coil map
  Node * localCathNode = getNode(remoteCathId, localCathMap);
  if (localCathNode == NULL) {
    cout << "Converter::getLocalCoilId() internal bug localCathMap and remoteCathMap don't match, return null id " << NULL_ID << endl;
    return NULL_ID;
  }

  //search local coil map to get the local coil
  NodeMap::iterator localCoilNode = localCathNode->nodeMap->find(remoteCoilId);
  if (localCoilNode != localCathNode->nodeMap->end()) {
    //found
    #ifdef DEBUG_CONVERTER
      cout << "Converter::getLocalCoilId() return found coil: " << localCoilNode->first << endl;
    #endif
    return localCoilNode->first;
  }
  else if (remoteCoilLoc != UNDEFINED_REMOTE_COIL_LOC) {
    //add a new local coil and return the coil id
    int localCoilId = localCath->addCoil(remoteCoilLoc);
    //add to localCathMap
    localCathNode->nodeMap->insert(NodePair(remoteCoilId, new Node(localCoilId)));
    //add to remote Cath Map
    getNode(localCath->getId(), remoteCathMap)->nodeMap->insert(NodePair(localCoilId, new Node(remoteCoilId)));

    #ifdef DEBUG_CONVERTER
      cout << "Converter::getLocalCoilId() return new coil: " << localCoilId << endl;
    #endif
    return localCoilId;
  }
  #ifdef DEBUG_CONVERTER
    cout << "Converter::getLocalCoilId() return null coil: " << NULL_ID << endl;
  #endif
  return NULL_ID;
}

bool Converter::setLocalCath(CATHDATA & remote, rtCathDataObject * local) {
  #ifdef DEBUG_CONVERTER
    cout << "Converter::setLocalCath()" << endl;
  #endif
  int localCoilId;

  bool success = true;
  //for all remote coils

  int coilIndex = 0;
  for (vector<COILDATA>::iterator currCoil = remote.coils.begin(); currCoil != remote.coils.end(); currCoil++) {
    #ifdef DEBUG_CONVERTER
      cout << "loop iteration: " << coilIndex << endl;
    #endif
    //get the local coil id to search with
    localCoilId = getLocalCoilId(coilIndex, local, currCoil->locID);

    //no local coil id found
    if (localCoilId == NULL_ID) continue;

    //set the local values
    success = success & local->setCoilAngles(localCoilId, currCoil->angles[0], currCoil->angles[1]);
    success = success & local->setCoilCoords(localCoilId, currCoil->coords[0], currCoil->coords[1], currCoil->coords[2]);
    success = success & local->setCoilSNR(localCoilId, currCoil->SNR);
    coilIndex++;
  }
  #ifdef DEBUG_CONVERTER
    cout << "Converter::setLocalCath() return " << success << endl;
  #endif
  return success;
}

bool Converter::setLocalCathAll(GenericMode & mode) {
  mode.runMode();

  //get all caths
  vector<CATHDATA> & remoteCaths = mode.getCath();
  rtCathDataObject * localCath;

  //for all remote caths
  for (vector<CATHDATA>::iterator it = remoteCaths.begin(); it != remoteCaths.end(); it++) {
    localCath = getLocalCath(REMOTE_CATH_ID);
    setLocalCath(*it, localCath);
  }

  printAllLocalCath();
}

