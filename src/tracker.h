#include "CommonHeaders.h"
#include "vrpn_Connection.h" // Missing this file?  Get the latest VRPN distro at
#include "vrpn_Tracker.h"
#include "conio.h"

class DataReceiverThread : public OpenThreads::Thread
{
public:
  static DataReceiverThread* instance()
  {
   static DataReceiverThread s_thread;
   return &s_thread;
  }
  virtual int cancel();
  virtual void run();

  void setPos(osg::Vec3d, osg::Quat);
  bool getPos(osg::Vec3d&, osg::Quat&);
  
protected:
  OpenThreads::Mutex _mutex;
  std::string _content;
  bool _done;
  bool _dirty;
  osg::Vec3d _pos;
  osg::Quat _orient;

};