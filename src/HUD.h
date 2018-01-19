#include "Commons.h"
#include "Controller.h"
#include "Cursor.h"

class HUD
{
public:
	HUD(osgViewer::Viewer *viewer)
		:_viewer(viewer) {}
	osg::Camera* getCamera();

private:
	osgViewer::Viewer* _viewer;
};