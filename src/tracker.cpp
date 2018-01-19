#include "tracker.h"

using namespace std;

double xPos=0.0, yPos=0.0, zPos=0.0;
double xOrient, yOrient, zOrient, wOrient;


//== Callback prototype ==--
void VRPN_CALLBACK handle_pos (void *, const vrpn_TRACKERCB t);

int DataReceiverThread::cancel()
{
  _done = true;
  while( isRunning() ) 
	  YieldCurrentThread();
  return 0;
}

void DataReceiverThread::run()
{
	_done = false;
	_dirty = true;

    vrpn_Connection *connection;
    char connectionName[128];
    int  port = 3883;
    sprintf(connectionName,"130.225.99.236:%d", port);  
    connection = vrpn_get_connection_by_name(connectionName);
    vrpn_Tracker_Remote *tracker = new vrpn_Tracker_Remote("head", connection);
	tracker->register_change_handler(NULL, handle_pos);

	do
	{	/* pos and rot info from VRPN*/
	
		YieldCurrentThread();
		if(kbhit())
			_done = true;
		else
		{
			setPos(osg::Vec3d(xPos, yPos, zPos), osg::Quat(xOrient, yOrient, zOrient, wOrient));
			tracker->mainloop();
			connection->mainloop();
			Sleep(.0);
		}
	} while( !_done );
}

//== Position/Orientation Callback ==--
void VRPN_CALLBACK	handle_pos (void *, const vrpn_TRACKERCB t)
{
    printf("Tracker Position:(%.4f,%.4f,%.4f) Orientation:(%.2f,%.2f,%.2f,%.2f)\n",
        t.pos[0], t.pos[1], t.pos[2],
        t.quat[0], t.quat[1], t.quat[2], t.quat[3]);
	xPos = t.pos[0];
	yPos = t.pos[1];
	zPos = t.pos[2];
	xOrient = t.quat[0];
	yOrient = t.quat[1];
	zOrient = t.quat[2];
	wOrient = t.quat[3];
	//xOrient = double(int(t.quat[0]*100+0.5))/100.0;
	//yOrient = double(int(t.quat[1]*100+0.5))/100.0;
	//zOrient = double(int(t.quat[2]*100+0.5))/100.0;
	//wOrient = double(int(t.quat[3]*100+0.5))/100.0;
	//zPos = double(int(zPos*10+0.5))/10.0;	

}

void DataReceiverThread::setPos(osg::Vec3d pos, osg::Quat orient)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
	_pos = pos;
	_orient = orient;
	_dirty = true;
}

bool DataReceiverThread::getPos(osg::Vec3d& pos, osg::Quat& orient)
{
  OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
  if ( _dirty )
  {
    pos = _pos;
	orient = _orient;
    _dirty = false;
    return true;
  }
  return false;
}