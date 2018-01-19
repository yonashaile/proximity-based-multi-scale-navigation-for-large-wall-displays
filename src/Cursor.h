
#ifndef H_CUROSR
#define H_CUROSR

#include "Commons.h"
#include "tracker.h"
#include "LinePlane.h"
#include "math.h"
#include "Controller.h"

#define LC "[viewer] "

using namespace std;
using namespace osgEarth::Util;

#ifndef ZOOMS
#define ZOOMS 500
#endif

#ifndef ZOOM_FACTOR
#define ZOOM_FACTOR 0.005
#endif

enum collistionType {HORIZONTAL, VERTICAL,NONE};
enum collisionLocation {LEFT, RIGHT, TOP, DOWN};

class CursorUtility
{
public:
	CursorUtility(osgViewer::Viewer*);
	CursorUtility();
	void worldToScreenCoord(const osg::Vec3d, osg::Vec3d&);
	void fixBoundary(osg::Vec3d&);
	bool physicalZoom(EarthManipulator*, double, double, double, double, osg::View*);
	bool physicalPan(EarthManipulator*, double, double, osg::View*);
	bool hotspotPan(EarthManipulator*, double, double, double,osg::Vec3d, double, osg::View*);
	osg::Camera* createCursor();
	void applyBBox(EarthManipulator*, osg::Vec3d &);
	osg::Vec3d shiftPoint(osg::Vec3d);
	collistionType _collisionType;
	collisionLocation _collisionLocation;


protected:
	osg::Vec3d _pos;
	osg::Vec3d _prevPos;
	osg::Vec3d _prevApos;
	vector<bool> isZooming;
	vector<double> zoomRange;
	osg::Timer timer;
	bool firstTime;
	// collision detected means panning through a vector connection the center of a screen and collision point
	bool _boundaryCollisionFlag; 
	bool _allBoundaryCollisionFlag;
	osgViewer::Viewer* _viewer;

public:
	osg::Vec3d bottomLeftScreenCoordWorld;
	osg::Vec3d topRightScreenCoordWorld;
	osg::Vec3d bottomLeftScreenCoordLocal;
	osg::Vec3d topRightScreenCoordLocal;
	double scaleDisplacement;
	double spaceDisplacement;
	std::ofstream scaleStream;
	std::ofstream spaceStream;
	std::ofstream boundaryStream;
	bool fexist(char*);
	bool firstTimeReadingFile;
	bool firstTimeReadingFileBoundary;
	osg::Timer _tick;

};

class CursorCallback : public osg::NodeCallback, public CursorUtility
{
focusCircle* fCircle;

public:
	CursorCallback(osgViewer::Viewer* viewer, focusCircle* fcircle)
		: CursorUtility(viewer) 
	{ fCircle = fcircle;}

	virtual void operator ()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::MatrixTransform* mthud = dynamic_cast<osg::MatrixTransform*>(node);
		osg::Matrix mR, mT;

		osg::Vec3d pos, wpos, apos, vpos;
		osg::Quat quat;

		double t, t2, D=_prevPos.z(), val=0;
		if(DataReceiverThread::instance()->getPos(pos, quat))
		{
			pos.set(pos.x(), pos.y()-0.2, pos.z());
			apos = osg::Vec3d(pos.x(), 0.0, 0.0);
			vpos = pos;
			//osg::Vec3d rotatedVector = quat*osg::Vec3d(-1.0, 0.0, 0.0);
			osg::Vec3d rotatedVector = quat*osg::Vec3d(0.0, 0.0, 1.0);
			osg::Vec3d rotatedVector2 = osg::Vec3d(0.0, 0.0, 1.0);
			Line line(pos, rotatedVector);
			Line line2(vpos, rotatedVector2);
			//Plane plane(osg::Vec3d(-0.240, 0.853, -0.343),osg::Vec3d(-0.240, 2.050, -3.292), osg::Vec3d(-1.0, 0.0, 0.0));
			Plane plane(bottomLeftScreenCoordWorld,topRightScreenCoordWorld, osg::Vec3d(0.0, 0.0, 1.0));
			D=pos.z();
			bool res = plane.intersect(line,t);
			bool res2 = plane.intersect(line2,t2);
			pos = pos + rotatedVector*t;
			vpos = vpos + rotatedVector2*t2;
			double d=line.getLength(pos);
			double d2 = line2.getLength(vpos);
			//pos.set(pos.z(), pos.y(), d);
			pos.set(pos.x(), pos.y(), d);
			vpos.set(vpos.x(), vpos.y(), d2);
			fixBoundary(pos);
			//double diff = (pos-_prevPos).length();
			//if( diff >= 0.0)
			//{
			_pos.set(pos.x(), pos.y(), pos.z());

			//}
			//else
			//	_pos.set(_prevPos.x(), _prevPos.y(), _prevPos.z());

			//_pos.set(double(int(pos.x()*10))/10.0,double(int(pos.y()*10))/10.0,double(int(pos.z()*10))/10.0 );

		}
		osg::Vec3d vppos;
		worldToScreenCoord(_pos, wpos);
		worldToScreenCoord(vpos, vppos);
		wpos.set(wpos.x(), wpos.y(), vppos.z());
		EarthManipulator* manip = dynamic_cast<EarthManipulator*>(_viewer->getCameraManipulator());
		unsigned int currentZoom = floor(wpos.z()/ZOOM_FACTOR);
		if (wpos.z() < ZOOM_FACTOR) currentZoom = 10;
		if(wpos.z() > 2.5) currentZoom = 500;
		double range, duration;
		range = (currentZoom>180)?currentZoom*260.0-40000:4500.0;
		if(firstTime)
		{
			if(wpos.z() <=  0.0)
			{
				EarthManipulator* manip = dynamic_cast<EarthManipulator*>(_viewer->getCameraManipulator());
				osgEarth::Viewpoint view("Copenhagen", 12.583, 55.7, 0, 0, -90, 100000.0, SpatialReference::create("epsg:4326"));
				manip->setViewpoint(view, .0);				
			}
			else 
			{
				duration = 0.0;
				EarthManipulator* manip = dynamic_cast<EarthManipulator*>(_viewer->getCameraManipulator());
				osgEarth::Viewpoint view("Copenhagen", 12.583, 55.7, 0, 0, -90, 100000.0, SpatialReference::create("epsg:4326"));
				manip->setViewpoint(view, .0);	
				firstTime = false;			
			}
			_prevApos = apos;
			timer.setStartTick();
		}
		else
		{
			double radius = 600.0;
			if(currentZoom <= 200)
				radius = 600.0;
			else
			{
				radius = (currentZoom - 200)*1.333 +600.0;
			}
			if(mthud->getNumChildren()>1)
			{
				mthud->removeChild(1);
				Commons focusCircle("focusCircle.png", radius,-radius);
				mthud->addChild(focusCircle.createPart());

			}
			fCircle->setGazePos(osg::Vec2d(wpos.x(), wpos.y())); // update cursor postion
			fCircle->setFocusCircleRadius(radius);



			double displacementLength = 0.0;//displacement.length();
			
			//if(timer.time_s() >= 0.004)
			//{
			//	timer.setStartTick();
			osg::Vec3d Disp = apos - _prevApos;
				displacementLength = apos.x() - _prevApos.x();
				_prevApos = apos;
			//}

			double Zmovement = abs(D - _prevPos.z());
			double XYmovement = (osg::Vec2d(wpos.x(), wpos.y()) - osg::Vec2d(_prevPos.x(), _prevPos.y())).length();
			_prevPos.set(wpos.x(), wpos.y(), D); 

			// check for boundary collision and interpret it as physicall panning
			if(_boundaryCollisionFlag) // panning
			{
				duration = 0.0;
				bool stat = hotspotPan(manip, wpos.x(), wpos.y(), wpos.z(), vppos, duration, _viewer->asView());
			}
			//if(abs(displacementLength) > 0.01 && abs(Disp.x()) > abs(Disp.z())) // if valid horizontal movement is made
			//{
			//	duration = 0.0;
			//	bool stat = physicalPan(manip, duration, displacementLength, _viewer->asView());
			//}
			// check for true physical zooming and fast movement in the XY plane
			if(Zmovement >= 0.001 || XYmovement >= 7000000.0 && abs(Disp.x()) < abs(Disp.z())) // 1€ filter or Douglas algorithm might solve this problem!!
			{
				duration = 0.0;
				if(!isZooming[currentZoom])
				{
					for(unsigned int i=0; i<ZOOMS; i++)
					{
						if(i==currentZoom)
							isZooming[i] = true;
						else
							isZooming[i] = false;
					}
					if(currentZoom == 500)
					{
						osgEarth::Viewpoint view("Copenhagen", 12.583, 55.7, 0, 0, -90, 99999.0, manip->getSRS());
						manip->setViewpoint(view, .0);	
					}
					else
					{
						bool stat = physicalZoom(manip, wpos.x(), wpos.y(), range, duration, _viewer->asView());
					}
				}
			}
			else
			{
				Viewpoint here = manip->getViewpoint();
				here.setPitch(-90.0);
				manip->setViewpoint(here);
			}
		}
	
		mT.makeTranslate(wpos.x(), wpos.y(), 0.0);
		mthud->setMatrix(mT);
		traverse(node, nv);
	}

};
class CursorUpdateCallback : public osg::NodeCallback, public CursorUtility
{
public:
	CursorUpdateCallback(osgViewer::Viewer* viewer)
		: CursorUtility(viewer) {}

	virtual void operator ()(osg::Node* node, osg::NodeVisitor* nv)
	{
		// keep the camera inside the valid BBox

		double duration = 0.0;
		EarthManipulator* manip = dynamic_cast<EarthManipulator*>(_viewer->getCameraManipulator());
		osgEarth::Viewpoint here = manip->getViewpoint();
		if ( !here.getSRS() )
			return;

		osg::Vec3d focalPt = here.getFocalPoint();

		applyBBox(manip, focalPt);

		focalPt.set(focalPt);
		here.setFocalPoint(focalPt);
		manip->setViewpoint(here, duration);

		traverse(node, nv);
	}

};


#endif
