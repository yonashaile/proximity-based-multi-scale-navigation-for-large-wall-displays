#include "Commons.h"


#ifndef NAVIGATIONTASK_H
#define NAVIGATIONtASK_H

using namespace osgEarth;
using namespace osgEarth::Features;
using namespace osgEarth::Drivers;
using namespace osgEarth::Symbology;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;

using namespace osgEarth::Annotation;

class NavigationUtility
{
public:
	NavigationUtility()
	:	s_fid(0), addMode(true)
	{ }
	
public:
	int s_fid;
	osg::ref_ptr<AddPointHandler > s_addPointHandler;
	osg::ref_ptr< osg::Node > s_editor;
	osg::ref_ptr< Feature > s_activeFeature;
	osgViewer::Viewer* s_viewer;
	osg::ref_ptr< osg::Group > s_root;
	osg::ref_ptr< FeatureListSource > s_source;
	osg::ref_ptr< MapNode > s_mapNode;
	osg::ref_ptr< FeatureModelGraph > _featureGraph;
	bool addMode;
};

static NavigationUtility* _navigationUtility;

class NavigationTask
{
public:
	NavigationTask()
	{
	}

	NavigationTask(std::vector<Pos> tl)
	: _targetList(tl), taskType("")
	
	{
	}
	std::string taskType;
	StyleSheet* buildStyleSheet(osg::Vec4, float);
	void createTask(osgViewer::Viewer* viewer, osgEarth::MapNode*, DataCollector*);
	void createTaskForNavigation(osgViewer::Viewer* viewer, osgEarth::MapNode*, DataCollector*);
	void createTask2(osgViewer::Viewer*, osgEarth::MapNode*);
	void createTask2ForNavigation(osgViewer::Viewer*, osgEarth::MapNode*);
	void setTargetList(std::vector<Pos> tl) { _targetList = tl;}

private:
	std::vector<Pos> _targetList;
};


#endif