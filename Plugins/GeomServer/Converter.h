#ifndef PLUGIN_CONVERTER_H
#define PLUGIN_CONVERTER_H
#include "SenderSimp.h"

#include <map>

#include "arguments.h"

#include "rtCathDataObject.h"
#include "rt2dSliceDataObject.h"

//Definitons for undefined things
#define UNDEFINED_RT_OBJECT_TYPE rtConstants::OT_None
#define UNDEFINED_REMOTE_COIL_LOC -1

//name of new cath objects
#define CATH_OBJ_NAME "GeomCath"
//name of new image objects
#define IMAGE_OBJ_NAME "GeomImage"
//id returned if not found
#define NULL_ID -1

//! Object that stores and passes information from the local and remote objects
/*!
Stores maps that relate the local and remote objects. Also passes information along those objects
 */
class Converter {
  //! Struct that allows recursive mapping of embedded objects
  /*!
    This struct is a solution of the problem where objects with id's contain other objects.
    It allows the parent object to contain an id and a mapping to child objects with ids.

    For example:
      Cathater has coils, each with an id. For the localCathMap, remote ids map to Nodes.
      A Cathater is represented by a Node and "currId" is it's local id.

      "nodeMap" is the mapping between the remote id to Nodes representing Coils.
      The "currId" of those nodes are the local id of the coil.
   */
  struct Node {
    //! Id of the object represented by the node
    int currId;
    //! Mapping to the nodes representing the object's children
    std::map<int, Node *> * nodeMap;

    Node(int currId) {
      this->currId = currId;
      nodeMap = new std::map<int, Node *>;
    }

    ~Node() {
      delete nodeMap;
    }
  };

  //Typedefs to simplify working with maps
  typedef std::map<int, Node *> NodeMap;
  typedef std::pair<int, Node *> NodePair;
  typedef std::map<int, int> IdMap;
  typedef std::pair<int, int> IdPair;
  //Typedef to easily define a void function pointer with no params
  typedef void (Converter::*ConverterFunction)();

  //! remote Cath Id -> local Cath Id/(localCoilMao, remote Coil Id -> local Coil Id)
  NodeMap * localCathMap;
  //! local Cath Id -> remote Cath Id/(remoteCoilMap, local Coil Id -> remote Coil Id)
  NodeMap * remoteCathMap;
  //! remote Image Id -> local Image Id
  IdMap * localImageMap;
  //! //local Image Id -> remote Image Id
  IdMap * remoteImageMap;

  Converter::Node * getLocalObjNode(int id, NodeMap * nodeMap, NodeMap * remoteMap = NULL, rtConstants::rtObjectType objType = UNDEFINED_RT_OBJECT_TYPE, const char * name = NULL);
  Converter::Node * getNode(int id, NodeMap * nodeMap);
  rtCathDataObject * getLocalCath(int remoteId);
  int getLocalCoilId(int remoteCoilId, rtCathDataObject * localCath, int remoteCoilLoc = UNDEFINED_REMOTE_COIL_LOC);

  int getRemoteCathId(int localId);
  int getRemoteCoilId(int localCathId, int localCoilId);

  rt2DSliceDataObject * getLocalImage(int remoteId, int imageSize);
public:
  Converter();
  ~Converter();

  void printHeaderFooter(ConverterFunction function);
  void printAll();
  void printAllLocalCath();
  void printAllLocalImages();

  bool setLocalCath(CATHDATA & remote, rtCathDataObject * local);
  bool setLocalCathAll(SenderSimp & sender);

  bool setLocalImage(IMAGEDATA & remote, rt2DSliceDataObject * local);
  bool setLocalImageAll(SenderSimp & sender);
};
#endif
