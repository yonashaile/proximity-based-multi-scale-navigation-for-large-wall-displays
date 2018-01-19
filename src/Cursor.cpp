
#include "Commons.h"
#include "Cursor.h"

using namespace osgEarth::Util;

bool bbox = false;
double prev_range = 100000.0;
//CursorUtility::CursorUtility(osgViewer::Viewer* v) : _pos(osg::Vec3d(0.0,0.0, 0.0)), _prevPos(osg::Vec3d(0.0,0.0, 0.0)), _viewer(v), firstTime(true), 
//	bottomLeftScreenCoordWorld(osg::Vec3d(-0.01146,0.08315,0.06625)), topRightScreenCoordWorld(osg::Vec3d(-2.73772,1.29538,0.06625)),
//	bottomLeftScreenCoordLocal(osg::Vec3d(0.0,0.0,0.0)), topRightScreenCoordLocal(osg::Vec3d(7680.0, 3240.0, 0.0)), _prevApos(osg::Vec3d(0.0,0.0,0.0)),
//	_boundaryCollisionFlag(false), _allBoundaryCollisionFlag(false)
//	{ 
//		for(unsigned int i=0; i<=(unsigned int)ZOOMS; i++ )
//		{
//			isZooming.push_back(false); zoomRange.push_back(i*ZOOM_FACTOR);
//		}
//	}
CursorUtility::CursorUtility() : _pos(osg::Vec3d(0.0,0.0, 0.0)), _prevPos(osg::Vec3d(0.0,0.0, 0.0)), firstTime(true), 
	bottomLeftScreenCoordWorld(osg::Vec3d(-0.01146,0.08315,0.06625)), topRightScreenCoordWorld(osg::Vec3d(-2.73772,1.29538,0.06625)),
	bottomLeftScreenCoordLocal(osg::Vec3d(0.0,0.0,0.0)), topRightScreenCoordLocal(osg::Vec3d(7680.0, 3240.0, 0.0)), _prevApos(osg::Vec3d(0.0,0.0,0.0)),
	_boundaryCollisionFlag(false), _allBoundaryCollisionFlag(false)	
{ 
		_tick.setStartTick();

		for(unsigned int i=0; i<=(unsigned int)ZOOMS; i++ )
		{
			isZooming.push_back(false); zoomRange.push_back(i*ZOOM_FACTOR);
		}
		
		firstTimeReadingFile = true;
		firstTimeReadingFileBoundary = true;
		scaleDisplacement = 0.0;
		spaceDisplacement = 0.0;

}

CursorUtility::CursorUtility(osgViewer::Viewer* v) : _pos(osg::Vec3d(0.0,0.0, 0.0)), _prevPos(osg::Vec3d(0.0,0.0, 0.0)), _viewer(v), firstTime(true), 
	bottomLeftScreenCoordWorld(osg::Vec3d(-0.01146,0.08315,0.06625)), topRightScreenCoordWorld(osg::Vec3d(-2.73772,1.29538,0.06625)),
	bottomLeftScreenCoordLocal(osg::Vec3d(0.0,0.0,0.0)), topRightScreenCoordLocal(osg::Vec3d(7680.0, 3240.0, 0.0)), _prevApos(osg::Vec3d(0.0,0.0,0.0)),
	_boundaryCollisionFlag(false), _allBoundaryCollisionFlag(false)	
{ 
		_tick.setStartTick();

		for(unsigned int i=0; i<=(unsigned int)ZOOMS; i++ )
		{
			isZooming.push_back(false); zoomRange.push_back(i*ZOOM_FACTOR);
		}
		
		firstTimeReadingFile = true;
		firstTimeReadingFileBoundary = true;
		scaleDisplacement = 0.0;
		spaceDisplacement = 0.0;

}


osg::Vec3d CursorUtility::shiftPoint(osg::Vec3d pos)
{
	osg::Vec3d screenCenter(topRightScreenCoordLocal.x()/2.0, topRightScreenCoordLocal.y()/2.0, 0.0);
	osg::Vec3d path(screenCenter-pos); double trash = path.normalize();
	Line line(pos, path);
	return line.getPoint(1);
}

void CursorUtility::worldToScreenCoord(osg::Vec3d p, osg::Vec3d& pos)
{
	double rx = abs(topRightScreenCoordWorld.x()-bottomLeftScreenCoordWorld.x()); 
	double ry = abs(topRightScreenCoordWorld.y()-bottomLeftScreenCoordWorld.y());
	double Rx = abs((p.x()-bottomLeftScreenCoordWorld.x())/rx); 
	double Ry = abs((p.y()-bottomLeftScreenCoordWorld.y())/ry);
	double rx_ = topRightScreenCoordLocal.x() - bottomLeftScreenCoordWorld.x(); 
	double ry_ = topRightScreenCoordLocal.y() - bottomLeftScreenCoordWorld.y();
	double sTx = bottomLeftScreenCoordWorld.x() + rx_*Rx; 
	double sTy = bottomLeftScreenCoordWorld.y() + ry_*Ry;
	pos = osg::Vec3d(sTx, sTy, p.z());
}

void CursorUtility::applyBBox(EarthManipulator* manip, osg::Vec3d& pos)
{
	bbox = false;
	double Wx = 0.989672026015, Wy = 0.238018423946;
	osgEarth::Viewpoint here = manip->getViewpoint();
	double range = here.getRange();
	double H = 100000.0 - range;
	double wx = (H*Wx) / 100000.0; 
	double wy = (H*Wy) / 100000.0;
	
	double minX = 12.583-wx;
	double maxX = 12.583+wx;
	double minY = 55.7-wy;
	double maxY = 55.7+wy;
	if(pos.x() <= minX)
	{
		pos.set(minX, pos.y(), pos.z()); bbox = true;
		_collisionType = HORIZONTAL;
	}
	else if(pos.x() >= maxX)
	{
		pos.set(maxX, pos.y(), pos.z()); bbox = true;
		_collisionType = HORIZONTAL;
	}
	else if(pos.y() <= minY)
	{
		pos.set(pos.x(), minY, pos.z());bbox = true;
		_collisionType = VERTICAL;
	}
	else if(pos.y() >= maxY)
	{
		pos.set(pos.x(), maxY, pos.z()); bbox = true;
		_collisionType = VERTICAL;
	}
	else
	{
		_collisionType = NONE;		
	}
}


void CursorUtility::fixBoundary(osg::Vec3d& pos)
{

	// LEGALIZE:- fix boundary condition
	double xPos = pos.x(), yPos = pos.y();
	_boundaryCollisionFlag = false;
	_allBoundaryCollisionFlag = false;
	if (pos.x()>bottomLeftScreenCoordWorld.x()) 
	{
		_collisionLocation = LEFT;
		xPos=bottomLeftScreenCoordWorld.x();// : xPos = pos.x();
		_boundaryCollisionFlag = true; 
		_allBoundaryCollisionFlag = true;
	}
	if(pos.x()<topRightScreenCoordWorld.x()) 
	{
		_collisionLocation = RIGHT;
		xPos=topRightScreenCoordWorld.x();// : xPos = pos.x();
		_boundaryCollisionFlag = true; 
		_allBoundaryCollisionFlag = true;
	}
	if(pos.y()<bottomLeftScreenCoordWorld.y()) 
	{
		_collisionLocation = DOWN;
		yPos=bottomLeftScreenCoordWorld.y();// : yPos = pos.y();
		_boundaryCollisionFlag = true;  
		_allBoundaryCollisionFlag = true;
	}
	if(pos.y()>topRightScreenCoordWorld.y()) 
	{
		_collisionLocation = TOP;
		yPos=topRightScreenCoordWorld.y();// : yPos = pos.y();
		_boundaryCollisionFlag = true; 
		_allBoundaryCollisionFlag = true;
	}
	pos.set(xPos, yPos, pos.z());
	
}

bool CursorUtility::fexist(char* fname)
{
	ifstream ifile(fname);
	return ifile;
}

bool CursorUtility::physicalZoom(EarthManipulator* manip, double mx, double my, double range, double duration, osg::View* view )
{
	/* make sure that streaming file is open before zooming begins */
	if(!fexist("Scale_Data.txt"))
	{
		return false;
	}
	if(firstTimeReadingFile)
	{
		scaleStream.open("Scale_Data.txt", ios::app);
		firstTimeReadingFile = false;
	}


    osg::Vec3d point;
	osg::Vec3d wwpos(mx, my, 0.0);

		// if boundary collision is detected screentoworld() returns false, which is ugly!!, 
	// the workaround is to move boudnary collision point a bit closer to the center of the screen
	// i.e, on the vector made from screen center
	if(_allBoundaryCollisionFlag)
		wwpos = shiftPoint(wwpos);


	if ( manip->screenToWorld( wwpos.x(), wwpos.y(), view, point ))
    {
        Viewpoint here = manip->getViewpoint();

        if ( !here.getSRS() )
            return false;

        osg::Vec3d pointVP;
        here.getSRS()->transformFromWorld(point, pointVP);
		/*
			C --> here,   Pt --> pointVP
		*/
		//double duration_s = .5;
        //double range_factor = 0.4;
		double Xc, Yc, Xt, Yt, H, H_, R, R_, X, Y, nor;
		Xc = here.x(); Yc = here.y(); Xt = pointVP.x(); Yt = pointVP.y();
		X = Xt-Xc; Y = Yt-Yc;
		H = sqrt(pow(X,2) + pow(Y,2));
		R = here.getRange(); 
		R_ = range;//R_ = R * range_factor;
		H_ = (R_/R)*H;
		nor = H - H_;
		osg::Vec3d Un(X/H, Y/H, 0.0);
		osg::Vec3d newV = Un * nor;
		osg::Vec3d Pt_ = newV + here.getFocalPoint(); 
		
		applyBBox(manip, Pt_);

		here.setFocalPoint(Pt_);

		/*start streaming scale here */
		if(scaleStream.is_open())
		{
			scaleStream<<(R - R_)<<std::endl;
		}

		here.setRange( R_ );

		here.setPitch(-90.0);
        manip->setViewpoint( here, duration );
		 
		return true;
    }
	else
		return false;
}

bool CursorUtility::hotspotPan(EarthManipulator* manip, double mx, double my, double mz, osg::Vec3d projectedHeadPt, double duration, osg::View* view )
{
		/* make sure that streaming file is open before zooming begins */
	if(!fexist("Boundary_Data.txt"))
	{
		return false;
	}
	if(firstTimeReadingFileBoundary)
	{
		boundaryStream.open("Boundary_Data.txt", ios::app);
		firstTimeReadingFileBoundary = false;
	}
	/*start streaming collision type here */
	if(boundaryStream.is_open())
	{
		switch(_collisionLocation)
		{
		case LEFT:
			boundaryStream<<"LEFT"<<std::endl;
			break;
		case DOWN:
			boundaryStream<<"BOTTOM"<<std::endl;
			break;
		case RIGHT:
			boundaryStream<<"RIGHT"<<std::endl;
			break;
		case TOP:
			boundaryStream<<"TOP"<<std::endl;
			break;
		default:
			boundaryStream<<"NONE"<<std::endl;
			break;
		}	
	}


	if(_collisionType == VERTICAL)
	{
		if(_collisionLocation == LEFT)
			projectedHeadPt.set(projectedHeadPt.x(), my, projectedHeadPt.z()); 
		if(_collisionLocation == RIGHT)
			projectedHeadPt.set(projectedHeadPt.x(), my, projectedHeadPt.z()); 
	}
	if(_collisionType == HORIZONTAL)
	{
		if(_collisionLocation == TOP)
			projectedHeadPt.set(mx, projectedHeadPt.y(), projectedHeadPt.z()); 
		if(_collisionLocation == DOWN)
			projectedHeadPt.set(mx, projectedHeadPt.y(), projectedHeadPt.z()); 
	}

	osg::Vec3d screenCenter(topRightScreenCoordLocal.x()/2.0, topRightScreenCoordLocal.y()/2.0, 0.0);
	osg::Vec3d projectedScreenCenter(projectedHeadPt.x(), projectedHeadPt.y(), 0.0);
	osg::Vec3d paningPath(osg::Vec3d(mx, my, 0.0)-projectedScreenCenter); double trash = paningPath.normalize();
	Line line(screenCenter, paningPath);
	
	// find appropriate speed for panning
	/* Refer some paper to calculate appropriate pannning speed */
	/* distance, duration=time, speed=? */
	double distance,speed;
	//distance=0.0;//ControllerUtility::instance()->_idx;
	Viewpoint here = manip->getViewpoint();

	double range = here.getRange();
	double factor = (100)*log(range/500.0);// - 1000000/range;
	distance = factor*100.0/530;

	osg::Vec3d newScreenCenter = line.getPoint(distance);

    osg::Vec3d point;
	if ( manip->screenToWorld( newScreenCenter.x(), newScreenCenter.y(), view, point ))
    {
        //Viewpoint here = manip->getViewpoint();

        if ( !here.getSRS() )
            return false;

        osg::Vec3d pointVP;
        here.getSRS()->transformFromWorld(point, pointVP);
		applyBBox(manip, pointVP);
		if(!bbox)
		{
			here.setFocalPoint(pointVP);
			here.setRange( here.getRange() );
			here.setPitch(-90.0);
			manip->setViewpoint( here, duration );
		}

		 
		return true;
    }
	else
		return false;
}

bool CursorUtility::physicalPan(EarthManipulator* manip, double duration, double displacement, osg::View* view )
{

	double distance = 100.0;

	Viewpoint here = manip->getViewpoint();

	osg::Vec3d pt;// = here.getFocalPoint();
	
	pt = (bottomLeftScreenCoordLocal + topRightScreenCoordLocal)/2.0;
	if(displacement > 0)
		pt.set(pt.x()-distance, pt.y(), pt.z());
	else
		pt.set(pt.x()+distance, pt.y(), pt.z());

    osg::Vec3d point;
	if ( manip->screenToWorld( pt.x(), pt.y(), view, point ))
    {
		if ( !here.getSRS() )
			return false;
        osg::Vec3d pointVP;
        here.getSRS()->transformFromWorld(point, pointVP);
		applyBBox(manip, pointVP);
		here.setFocalPoint(pointVP);
		here.setPitch(-90.0);
		manip->setViewpoint( here, duration );
	}
		 
	return true;

}