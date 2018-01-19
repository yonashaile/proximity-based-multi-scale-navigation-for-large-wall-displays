#include "Commons.h"

osg::MatrixTransform*  Commons::createPart()
{
	osg::Vec3 center(-_radius, -_radius, _height);
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(
		createTexturedQuadGeometry(center, osg::Vec3(_radius*2.0f,0.0f,0.0f), osg::Vec3(0.0f,_radius*2.0f,0.0f)) );
    
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage( osgDB::readImageFile(_image) );
	texture->setResizeNonPowerOfTwoHint(false); 
    
	osg::ref_ptr<osg::MatrixTransform> part = new osg::MatrixTransform;
	part->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get() );
	part->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	part->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	part->addChild( geode.get() );
	return part.release();
}

osg::Vec3Array* circleVerts( int plane, int approx )
{
    const double angle( osg::PI * 2. / (double) approx );
    osg::Vec3Array* v = new osg::Vec3Array;
    int idx;
    for( idx=0; idx<approx; idx++)
    {
        double cosAngle = cos(idx*angle);
        double sinAngle = sin(idx*angle);
        double x(0.), y(0.), z(0.);
        switch (plane) {
            case 0: // X
                y = cosAngle;
                z = sinAngle;
                break;
            case 1: // Y
                x = cosAngle;
                z = sinAngle;
                break;
            case 2: // Z
                x = cosAngle;
                y = sinAngle;
                break;
        }
        v->push_back( osg::Vec3( x, y, z ) );
    }
    return v;
}
 
osg::Geode* Commons::circles( int plane, int approx )
{
    osg::Geode* geode = new osg::Geode;
    osg::LineWidth* lw = new osg::LineWidth( 3. );
    geode->getOrCreateStateSet()->setAttributeAndModes( lw,
osg::StateAttribute::ON );
 

    osg::Geometry* geom = new osg::Geometry;
    osg::Vec3Array* v = circleVerts( plane, approx );
    geom->setVertexArray( v );
 
    osg::Vec4Array* c = new osg::Vec4Array;
    c->push_back( osg::Vec4( 1., 1., 1., 1. ) );
    geom->setColorArray( c );
    geom->setColorBinding( osg::Geometry::BIND_OVERALL );
    geom->addPrimitiveSet( new osg::DrawArrays( GL_LINE_LOOP, 0, approx ) );
 
    geode->addDrawable( geom );
    return geode;
}