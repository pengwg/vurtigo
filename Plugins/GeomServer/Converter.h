#ifndef PLUGIN_CONVERTER_H
#define PLUGIN_CONVERTER_H
#include "SenderSimp.h"

#include <map>
#include "arguments.h"
#include "rtCathDataObject.h"
#include "rtImageBufferDataObject.h"

#define UNDEFINED_RT_OBJECT_TYPE rtConstants::OT_None
#define UNDEFINED_RT_OBJECT_NAME ""
#define UNDEFINED_REMOTE_COIL_LOC -1

class Converter {
  struct Node {
    int currId;
    std::map<int, Node *> * nodeMap;

    Node(int currId) {
      this->currId = currId;
      nodeMap = new std::map<int, Node *>;
    }

    ~Node() {
      delete nodeMap;
    }
  };

  typedef std::map<int, Node *> NodeMap;
  typedef std::pair<int, Node *> NodePair;

  NodeMap * localCathMap; //remote Cath Id -> local Cath Id/(localCoilMao, remote Coil Id -> local Coil Id)
  NodeMap * remoteCathMap; //local Cath Id -> remote Cath Id/(remoteCoilMap, local Coil Id -> remote Coil Id)

  Converter::Node * getRTObjNode(int id, NodeMap * nodeMap, NodeMap * remoteMap, rtConstants::rtObjectType objType = UNDEFINED_RT_OBJECT_TYPE, char * name = UNDEFINED_RT_OBJECT_NAME);
  Converter::Node * getNode(int id, NodeMap * nodeMap);
  rtCathDataObject * getLocalCath(int remoteId);
  int getLocalCoilId(int remoteCoilId, rtCathDataObject * localCath, int remoteCoilLoc = UNDEFINED_REMOTE_COIL_LOC);

  int getRemoteCathId(int localId);
  int getRemoteCoilId(int localCathId, int localCoilId);
public:
  Converter();
  ~Converter();

  void printAllLocalCath();

  bool setLocalCath(CATHDATA & remote, rtCathDataObject * local);
  bool setLocalCathAll(SenderSimp & sender);

  bool setImage(IMAGEDATA & remote, rtImageBufferDataObject * local);
};
#endif
