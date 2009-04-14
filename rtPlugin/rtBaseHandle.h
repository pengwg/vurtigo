#ifndef RT_BASE_HANDLE_H
#define RT_BASE_HANDLE_H

//! The handle that the Plugin can use.
/*!
  Plugins may need to ask the base for instances of object or they may need to register for callbacks. This base handle is the way plugins should access the base. The functions here are specifically designed for the plugins to use.
  The plugins SHOULD NOT use the rtObjectManager or rtPluginLoader directly. 
  This class is also a singleton meaning that there will only be one instance of it for the whole program. Plugins should get access to this via the instance() function.
 */
class rtBaseHandle {
 public:
  ~rtBaseHandle();

  static rtBaseHandle& instance() {
    static rtBaseHandle handle;
    return handle;
  }

 private:
  //! Private constructor
  rtBaseHandle();
  //! Private copy-constructor
  rtBaseHandle(const rtBaseHandle&);
  rtBaseHandle& operator=(const rtBaseHandle&);

};

#endif
