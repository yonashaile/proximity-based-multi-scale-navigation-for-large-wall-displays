
#include "HUD.h"

osg::Camera* HUD::getCamera()
{
	double layydherX = (7680/1680), layydherY = (3240/1050);
	osg::Camera* hudCamera = new osg::Camera();
	osg::ref_ptr<osg::MatrixTransform> mtCursor = new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> mtGauge = new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> mtPointer = new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> mtMainText = new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> mtGaugePointerText = new osg::MatrixTransform;

	hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	hudCamera->setProjectionMatrixAsOrtho2D(0,7680,0,3240);
	hudCamera->setViewMatrix(osg::Matrix::identity());
	hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
	hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);

	/* Cursor Geometry and its transformation */
	focusCircle* fcircle= new focusCircle();
	{
		mtCursor->setName("Cursor Transform");
		mtCursor->setDataVariance(osg::Object::DYNAMIC);
		
		
		mtCursor->setUpdateCallback(new CursorCallback(_viewer, fcircle)); // update callback for the cursor
		Commons cursorGeom("cursorNew.png", 100.5f, -100.0f);
		Commons focusCircle("focusCircle.png", 400.5f,-400.0f);
		
		mtCursor->addChild(cursorGeom.createPart());
		mtCursor->addChild(focusCircle.createPart());
	}
	/* Controller Geometry and its transformation */
	{
		mtGauge->setName("Gauge Transform");
		mtPointer->setName("Pointer Transform");

		mtGauge->setDataVariance(osg::Object::STATIC);
		mtPointer->setDataVariance(osg::Object::DYNAMIC);

		//mtPointer->setUpdateCallback(new ControllerCallback()); // update callback for the pointer geometry
		
		Commons gaugeGeome("scale.png", 200.5f, -200.0f);
		Commons pointerGeome("pointer.png", 200.5f, -200.0f);

		mtGauge->addChild(gaugeGeome.createPart());		
		mtPointer->addChild(pointerGeome.createPart());

		/* reposition gauge and pointer to the left of the screen */
		{
			osg::Matrix mTg, mSg, mTp, mSp;
			
			mTg.makeTranslate(1500.0*layydherX, 500.0*layydherY, 0.0);
			mSg.makeScale(osg::Vec3d(0.4*layydherX,2.2*layydherY,0.0));
			
			mTp.makeTranslate(1500.0*layydherX, 520.0*layydherY, 0.0);
			mSp.makeScale(0.3*layydherX, 0.08*layydherY, 0.0);
			
			mtGauge->setMatrix(mSg*mTg);
			mtPointer->setMatrix(mSp*mTp);
			
		}
	}
	ControllerUtility* cntU = new ControllerUtility();
	{ /* Text Decoration*/
		
		{/* the Text label  */
			osg::ref_ptr<osg::MatrixTransform> mtText = new osg::MatrixTransform;
			osg::Matrix mTt;
			osg::ref_ptr<osgText::Text> text = new osgText::Text;
			text->setFont( "fonts/arial.ttf" );
			text->setColor(osg::Vec4d(0.0, 0.0, 0.0, 1.0));
			text->setAxisAlignment( osgText::TextBase::SCREEN );
			text->setDataVariance( osg::Object::STATIC );
			text->setInitialBound(osg::BoundingBox(osg::Vec3(), osg::Vec3(400.0f, 20.0f, 0.0f)));
			text->setText("Panning Speed: ");
			text->setCharacterSize(100);
			osg::ref_ptr<osg::Geode> geode = new osg::Geode;
			geode->addDrawable( text.get() );
			geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF );
			mtText->addChild(geode.get());
		
			// translate the text box to upper left corner at the top of the gauge
			mTt.makeTranslate(1300.0*layydherX, 970.0*layydherY, 0.0);
			mtText->setMatrix(mTt);
			mtMainText->addChild(mtText.get());
		}
		/* Text value  */
		{
			osg::ref_ptr<osg::MatrixTransform> mtValue = new osg::MatrixTransform;
			osg::Matrix mTt;
			osg::ref_ptr<osgText::Text> text = new osgText::Text;
			text->setFont( "fonts/arial.ttf" );
			text->setColor(osg::Vec4d(0.0, 0.0, 0.0, 1.0));
			text->setCharacterSize(100.0);
			text->setAxisAlignment( osgText::TextBase::SCREEN );
			text->setDataVariance( osg::Object::DYNAMIC );
			text->setInitialBound(osg::BoundingBox(osg::Vec3(), osg::Vec3(400.0f, 20.0f, 0.0f)));
			//text->setUpdateCallback( new UpdateTextCallback );
			text->setUpdateCallback(new UpdateTextCallback(cntU));
			osg::ref_ptr<osg::Geode> geode = new osg::Geode;		
			geode->addDrawable( text.get() );
			geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF );
			mtValue->addChild(geode.get());
		
			// translate the text box to upper left corner at the top of the gauge
			mTt.makeTranslate(1580.0*layydherX, 970.0*layydherY, 0.0);
			mtValue->setMatrix(mTt);
			mtMainText->addChild(mtValue.get());
		}
	}


	/* insert all transformtion to HUD camera */
	{
		mtGaugePointerText->setUpdateCallback(new ControllerCallback(cntU));
		mtGaugePointerText->addChild(mtPointer.get());
		mtGaugePointerText->addChild(mtGauge.get());
		mtGaugePointerText->addChild(mtMainText.get());

		hudCamera->addChild(mtCursor.get());
		//hudCamera->addChild(mtGaugePointerText.get());
	}

	_viewer->addEventHandler(new KeyboardEventHandler(cntU));
	_viewer->addEventHandler(new focusCircleEventHandler(fcircle));
	//_viewer->getCamera()->setCullMask(0x2);
	hudCamera->setCullMask(0x2);

	return hudCamera;

}