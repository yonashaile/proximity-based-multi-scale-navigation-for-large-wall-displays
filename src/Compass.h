

#ifndef H_COMPASS
#define H__COMPASS

#include "CommonHeaders.h"

class Compass
{
public:
	Compass(osg::Camera* mainCam)
		: _mainCamera(mainCam){}

    osg::Camera* createCompass();
private:
    osg::observer_ptr<osg::Camera> _mainCamera;
};


class CompassCallback : public osg::NodeCallback
{
public:
	CompassCallback(osg::Camera* mainCam, osg::MatrixTransform* pt, osg::MatrixTransform* nt)
		: _mainCamera(mainCam), _plateTransform(pt), _needleTransform(nt){}
    /** Set the plate of the compass inside the camera projection range. */
    void setPlate( osg::MatrixTransform* plate ) { _plateTransform = plate; }
    osg::MatrixTransform* getPlate() { return _plateTransform.get(); }
    const osg::MatrixTransform* getPlate() const { return _plateTransform.get(); }
    
    /** Set the needle of the compass inside the camera projection range. */
    void setNeedle( osg::MatrixTransform* needle ) { _needleTransform = needle; }
    osg::MatrixTransform* getNeedle() { return _needleTransform.get(); }
    const osg::MatrixTransform* getNeedle() const { return _needleTransform.get(); }
    
    /** Set the main camera for the compass; otherwise there will be no effect. */
    void setMainCamera( osg::Camera* camera ) { _mainCamera = camera; }
    osg::Camera* getMainCamera() { return _mainCamera.get(); }
    const osg::Camera* getMainCamera() const { return _mainCamera.get(); }

	virtual void operator ()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::MatrixTransform* mtCompass = dynamic_cast<osg::MatrixTransform*>(node);
		osg::Matrix mR, mT;


		osg::Matrix matrix = _mainCamera->getViewMatrix();
        matrix.setTrans( osg::Vec3() );
        
        osg::Vec3 northVec = osg::Z_AXIS * matrix;
        northVec.z() = 0.0f;
        northVec.normalize();
        
        osg::Vec3 axis = osg::Y_AXIS ^ northVec;
        float angle = atan2(axis.length(), osg::Y_AXIS*northVec);
        axis.normalize();
        
        if ( _plateTransform.valid() )
		{
            _plateTransform->setMatrix( osg::Matrix::rotate(angle, axis) );
			
		}
		_needleTransform->setMatrix(osg::Matrix::translate(osg::Vec3d(0.0, 0.0,0.0)));

		traverse(node, nv);
	}

protected:
	
    osg::ref_ptr<osg::MatrixTransform> _plateTransform;
    osg::ref_ptr<osg::MatrixTransform> _needleTransform;
    osg::observer_ptr<osg::Camera> _mainCamera;
};

#endif
