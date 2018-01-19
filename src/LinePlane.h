#include <osg/Vec3d>
#include <math.h>

class Line{

public:
	Line(osg::Vec3d origin, osg::Vec3d direction)
	{
		_o=origin; _d=direction;
	}
	Line(Line& l)
	{
		_o=l._o; _d=l._d;
	}
	inline osg::Vec3d getO() {return _o;}
	inline osg::Vec3d getD() {return _d;}
	inline void setO(osg::Vec3d o){_o=o;}
	inline void setD(osg::Vec3d d){_d=d;}
	inline double getLength(osg::Vec3d pt){return (pt-_o).length();}
	inline osg::Vec3d getPoint(double t) {return _o + _d*t;}

private:
	osg::Vec3d _o;
	osg::Vec3d _d;

};

struct Plane{

public:
	Plane(osg::Vec3d _p1, osg::Vec3d _p2, osg::Vec3d _n)
	{
		a = _n.x(); b = _n.y(); c = _n.z();
		osg::Vec3d _p = _p2 - _p1; osg::Vec3d _v = osg::Vec3d(0.0,0.0,0.0) - _p1;
		double trash = _p.normalize();
		double proj = _p * _v;
		d = -(sqrt(pow(_v.length(),2)-pow(proj,2)));
	}
	Plane(Plane& p) 
	{
		a=p.a, b=p.b; c=p.c; d=p.d;
	}

	inline bool intersect(Line& l, double& t)
	{
		double f0 = a * l.getO().x() + b * l.getO().y() + c * l.getO().z() + d;
		float f1 = a * -l.getD().x() + b * -l.getD().y() + c * -l.getD().z();
		if ((f1 < -0.0001f) || (f1 > 0.0001f))
		{
			t = f0 / f1;
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	double a, b, c, d;
};
