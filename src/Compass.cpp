#include "Compass.h"
#include "Commons.h"

osg::Camera* Compass::createCompass()
{
	osg::Camera* compassCam = new osg::Camera();
    compassCam->setViewport( 0.0, 0.0, 100.0, 100.0 );
    compassCam->setProjectionMatrix( osg::Matrixd::ortho(-1.5, 1.5, -1.5, 1.5, -10.0, 10.0) );
  
    compassCam->setRenderOrder( osg::Camera::POST_RENDER );
    compassCam->setClearMask( GL_DEPTH_BUFFER_BIT );
    compassCam->setAllowEventFocus( false );
    compassCam->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    compassCam->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    compassCam->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );

	osg::ref_ptr<osg::MatrixTransform> mtMain = new osg::MatrixTransform();

	{
		Commons compassPlate("compass_plate.png", 1.5f, -1.0f);
		Commons compassNeedle("compass_needle.png", 1.5f, 0.0f);

		osg::ref_ptr<osg::MatrixTransform> plateTransform = compassPlate.createPart(); 
		osg::ref_ptr<osg::MatrixTransform> needleTransform = compassNeedle.createPart();

		plateTransform = compassPlate.createPart(); 
		needleTransform = compassNeedle.createPart();

		plateTransform->setDataVariance(osg::Object::STATIC);
		needleTransform->setDataVariance(osg::Object::STATIC);
		
		mtMain->setDataVariance(osg::Object::DYNAMIC);

		mtMain->setUpdateCallback(new CompassCallback(_mainCamera.get(), plateTransform.get(), needleTransform.get()));
		mtMain->addChild(plateTransform.get());
		mtMain->addChild(needleTransform.get());
	}

	compassCam->addChild(mtMain.get());

	return compassCam;

}

