#ifndef RT_PLUGIN_INTERFACE_H
#define RT_PLUGIN_INTERFACE_H

#include <QtPlugin>
#include <QWidget>

class DataInterface {
 public:
  ~DataInterface() {}

  //! Get the base widget that controls this plugin.
  QWidget* getWidget() { return m_mainWidget; }

  //! Called when an object is modified in the base.
  /*!
    Plugin has to register with the base in order to get this call. 
   */
  virtual void objectModified(int objID) = 0;

  //! This is the first thing that is called when a plugin is loaded. 
  /*!
    Called by the base when the plugin is first loaded. This is a good place to allocate memory or to start threads. 
    @return Flase if the plugin could not be initialized and true otherwise.
   */
  virtual bool init() = 0;

  //! Called just before the plugin is unloaded by the base. 
  /*!
    This is where memory previously allocated should be cleaned and where extra running threads can be terminated. 
   */
  virtual void cleanup() = 0;

  //! Base calls this function at regular intervals. 
  /*!
    The plugin can register so that the base will call this function at regular intervals.
   */
  virtual void update() = 0;

  //! Called when the user selects a point in the 3D view. 
  virtual void point3DSelected(double px, double py, double pz, int intensity) = 0;

 protected:
  QWidget* m_mainWidget;
};

Q_DECLARE_INTERFACE(DataInterface, "rt.Vurtigo.DataInterface/1.0")

#endif
