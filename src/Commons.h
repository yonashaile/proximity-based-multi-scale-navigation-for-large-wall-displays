#ifndef COMMONS_H
#define COMMONS_H


#include "CommonHeaders.h"

class Commons
{
public:
	Commons(const std::string img, float r, float h)
	: _image(img), _radius(r), _height(h) {}

	osg::MatrixTransform* createPart();
	osg::Geode* circles( int plane, int approx );

private:
	std::string _image;
	float _radius;
	float _height;
};

/* Informationt tracker for the experiment */
// performance_time, physical_location(range), amount of virtual zoom and pan range

class DataCollector
{

public:
	DataCollector()
	: _taskName(""), _elabsedTime(0), _fileName("") {}

	void printToFile()
	{
		timerFile<<_taskName<<std::setw(20)<<_elabsedTime<<std::endl;	
	}
	void finalizeFile()
	{
		timerFile.close();
	}

public:
	osg::Timer _timer;
	std::string _taskName;
	std::string _fileName;
	std::ofstream timerFile;
	double _elabsedTime;

};


/* cute struct for holding geodetic/geographic information about a point */
struct Pos
{
	Pos()
	: _name(""), _lat(0.0), _lon(0.0) {}

	Pos(std::string name, double lat, double lon)
		: _name(name), _lat(lat), _lon(lon) { }

	std::string _name;
	double _lat;
	double _lon;
};

struct focusCircle {
private:
	osg::Vec2d gazePos;
	double focusCircleRadius;
public:
	focusCircle()
	{
		gazePos = osg::Vec2d(0,0);
		focusCircleRadius = 0.0;
	}
	void setGazePos(osg::Vec2d pos) {gazePos.set(pos.x(), pos.y());}
	void setFocusCircleRadius(double r) { focusCircleRadius = r;}
	osg::Vec2d getGazePos() { return gazePos; }
	double getFocusCircleRadius() { return focusCircleRadius; }
};




#endif