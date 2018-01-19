//#include "tracker.h"
#include "HUD.h"
#include "Compass.h"
#include "widgetMenu.h"
#include "Commons.h"

int main(int argc, char** argv)
{
	osg::ArgumentParser arguments(&argc,argv);
	osgViewer::Viewer viewer;//(arguments);
	osg::ref_ptr<osgEarth::MapNode> s_mapNode = osgEarth::MapNode::load(arguments);
	
	// install our default manipulator (do this before calling load)
    viewer.setCameraManipulator( new EarthManipulator() );
	osg::Node* node = MapNodeHelper().load( arguments, &viewer );
	if(node)
	{
		osg::ref_ptr<osg::Group> group = new osg::Group();

		HUD hudCamera(&viewer);
		
		Compass compass(viewer.getCamera());
		TaskMenu taskMenu;

		osg::ref_ptr<osg::Camera> hudCam = hudCamera.getCamera();
		osg::ref_ptr<osg::Camera> compassCam = compass.createCompass();
		osgWidget::Window* menu = taskMenu.createTaskMenu(&viewer, s_mapNode);

		group->addChild(s_mapNode);
		//group->addChild(node);
		group->addChild(hudCam);
		group->addChild(compassCam.get());

		viewer.setSceneData(group);
		viewer.setUpViewInWindow( 0, 0, 7680.0, 3240.0 );
		viewer.addEventHandler(new osgViewer::StatsHandler);
        viewer.getCamera()->setNearFarRatio(0.00002);	
        viewer.getDatabasePager()->setDoPreCompile( true );

		osgWidget::WindowManager* wm = new osgWidget::WindowManager(
			&viewer,
			7680.0f,
			3240.0f,
			MASK_2D,
			osgWidget::WindowManager::WM_PICK_DEBUG
		);

		wm->addChild(menu);

		menu->getBackground()->setColor(1.0f, 1.0f, 1.0f, 0.0f);
		menu->resizePercent(100.0f);

		DataReceiverThread::instance()->startThread();
		int res = osgWidget::createExample(viewer, wm, group);
		DataReceiverThread::instance()->cancel();
		return res;
	}
	else
    {
        OE_NOTICE 
            << "\nUsage: " << argv[0] << " file.earth" << std::endl
            << MapNodeHelper().usage() << std::endl;
    }

}