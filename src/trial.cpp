//#include <osg/LineWidth>
//#include <osg/Geometry>
//#include <osg/math>
//#include <math.h>
//#include "CommonHeaders.h"
//
//osg::Vec3Array*
//circleVertss( int plane, int approx )
//{
//    const double angle( osg::PI * 2. / (double) approx );
//    osg::Vec3Array* v = new osg::Vec3Array;
//    int idx;
//    for( idx=0; idx<approx; idx++)
//    {
//        double cosAngle = cos(idx*angle);
//        double sinAngle = sin(idx*angle);
//        double x(0.), y(0.), z(0.);
//        switch (plane) {
//            case 0: // X
//                y = cosAngle;
//                z = sinAngle;
//                break;
//            case 1: // Y
//                x = cosAngle;
//                z = sinAngle;
//                break;
//            case 2: // Z
//                x = cosAngle;
//                y = sinAngle;
//                break;
//        }
//        v->push_back( osg::Vec3( x, y, z ) );
//    }
//    return v;
//}
// 
//osg::Geode*
//circless( int plane, int approx )
//{
//    osg::Geode* geode = new osg::Geode;
//    osg::LineWidth* lw = new osg::LineWidth( 3. );
//    geode->getOrCreateStateSet()->setAttributeAndModes( lw,
//osg::StateAttribute::ON );
// 
//
//    osg::Geometry* geom = new osg::Geometry;
//    osg::Vec3Array* v = circleVertss( plane, approx );
//    geom->setVertexArray( v );
// 
//    osg::Vec4Array* c = new osg::Vec4Array;
//    c->push_back( osg::Vec4( 1., 1., 1., 1. ) );
//    geom->setColorArray( c );
//    geom->setColorBinding( osg::Geometry::BIND_OVERALL );
//    geom->addPrimitiveSet( new osg::DrawArrays( GL_LINE_LOOP, 0, approx ) );
// 
//    geode->addDrawable( geom );
//    return geode;
//}
//
//int main(int argc, char** argv)
//{
//	osgViewer::Viewer viewer;
//	viewer.setSceneData(circless(2, 50));
//	viewer.run();
//}