#include "rtObjectManager.h"

rtObjectManager::rtObjectManager() {

}

rtObjectManager::~rtObjectManager() {

}

void rtObjectManager::setMainWinHandle(rtMainWindow* mainWin) {
  m_mainWinHandle = mainWin;
}

rtMainWindow* rtObjectManager::getMainWinHandle() {
  return m_mainWinHandle;
}
