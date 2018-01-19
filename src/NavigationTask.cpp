#include "NavigationTask.h"
#include <ctime>

using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace osgEarth::Features;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;

std::string tasktype;
bool isAlive = true;
unsigned int index=0;
unsigned int children;
osg::ref_ptr<osg::Group> root;
osg::Group* annoGroup;
osg::Group* annoGroup2;
osg::Group* labelGroup;
osgEarth::MapNode* mapNode;
const SpatialReference* geoSRS;
std::vector<Pos> _tList;
osgViewer::Viewer* _viewer;

bool realDel = false;
RectangleNode* rec;
RectangleNode* recRandom;
Color color1(Color::Green, 0.8);
Color color2(Color::Blue, 0.8);
Color color3(Color::Red, 0.8);

Viewpoint pt;
EarthManipulator* manip;
Style recStl;
bool paintTime = false; // true for green, false for red


bool lastTask = false;

osg::Node* createRecs()
{
    // A rectangle around San Diego
    Style rectStyle;
	rectStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Blue, 0.8);
    RectangleNode* rect = new RectangleNode(
        mapNode, 
        GeoPoint(geoSRS, _tList[index]._lat, _tList[index]._lon),
        Linear(50, Units::METERS ),
        Linear(50, Units::METERS ),
        rectStyle,
        true );
	rect->setName(_tList[index]._name);
	//rect->addEventCallback(new colorizer);
	//rec->setDataVariance(osg::Object::DYNAMIC);
	return rect;
    //editorGroup->addChild( new RectangleNodeEditor( rect ) );
}

osg::Node* createRecsRandomTargets(float lon, float lat)
{
    // A rectangle around San Diego
    Style rectStyle;
	rectStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Blue, 0.8);
    RectangleNode* rect = new RectangleNode(
        mapNode, 
        GeoPoint(geoSRS, lat, lon),
        Linear(50, Units::METERS ),
        Linear(50, Units::METERS ),
        rectStyle,
        true );
	rect->setName("?");
	//rect->addEventCallback(new colorizer);
	//rec->setDataVariance(osg::Object::DYNAMIC);
	return rect;
    //editorGroup->addChild( new RectangleNodeEditor( rect ) );
}

struct deleteHandler2 : public osg::NodeCallback // CallBack for Navigation task
{
	virtual void operator ()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::Group* annoGroup = dynamic_cast<osg::Group*>(node);
		children = annoGroup->getNumChildren();
		if(annoGroup->getName() == "annoGroup")
		{
			if(children >= 2 && !isAlive)
			{
				annoGroup->removeChild(0, 2);
				realDel = true;
			}
			else if(isAlive)
			{
				manip = dynamic_cast<EarthManipulator*>(_viewer->getCameraManipulator());
				pt = manip->getViewpoint();
				rec = dynamic_cast<RectangleNode*>( annoGroup->getChild(1));
				if(pt.getRange() <= 5000 && !paintTime)
				{
					recStl.getOrCreate<PolygonSymbol>()->fill()->color() = color1;
					rec->setStyle(recStl);
					paintTime = true;
				}
				else if(pt.getRange() > 5000 && paintTime)
				{
					recStl.getOrCreate<PolygonSymbol>()->fill()->color() = color2;
					rec->setStyle(recStl);
					paintTime = false;
				}
				//
			}
		}
		traverse(node, nv);
	}

};

struct deleteHandler : public osg::NodeCallback // search
{
	virtual void operator ()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::Group* annoGroup = dynamic_cast<osg::Group*>(node);
		children = annoGroup->getNumChildren();
		if(annoGroup->getName() == "annoGroup")
		{
			if(children >= 2 && !isAlive)
			{
				annoGroup->removeChild(0, 3);
				annoGroup2->removeChild(0, 9);
				realDel = true;
			}
			else if(isAlive)
			{
				manip = dynamic_cast<EarthManipulator*>(_viewer->getCameraManipulator());
				pt = manip->getViewpoint();
				int numChild = annoGroup->getNumChildren();
				int numChild2 = annoGroup2->getNumChildren();

				if(pt.getRange() < 5000 && !paintTime)
				{
					for(unsigned int i=1; i<=8; i++)
					{
						recRandom = dynamic_cast<RectangleNode*>( annoGroup2->getChild(i));
						recStl.getOrCreate<PolygonSymbol>()->fill()->color() = color3;
						recRandom->setStyle(recStl);
					}
					for(unsigned int i=1; i<=2; i++)
					{
						rec = dynamic_cast<RectangleNode*>( annoGroup->getChild(i));
						recStl.getOrCreate<PolygonSymbol>()->fill()->color() = color1;
						rec->setStyle(recStl);
					}
					paintTime = true;
				}
				else if(pt.getRange() > 5000 && paintTime)
				{
					for(unsigned int i=1; i<=8; i++)
					{
						recRandom = dynamic_cast<RectangleNode*>( annoGroup2->getChild(i));
						recStl.getOrCreate<PolygonSymbol>()->fill()->color() = color2;
						recRandom->setStyle(recStl);
					}
					for(unsigned int i=1; i<=2; i++)
					{
						rec = dynamic_cast<RectangleNode*>( annoGroup->getChild(i));
						recStl.getOrCreate<PolygonSymbol>()->fill()->color() = color2;
						rec->setStyle(recStl);
					}
					paintTime = false;
				}
				//
			}
		}
		traverse(node, nv);
	}

};

void createSingleTask2Navigation() // create task for navigation
{

    labelGroup = new osg::Group();
    Decluttering::setEnabled( labelGroup->getOrCreateStateSet(), true );
    annoGroup->addChild( labelGroup );

    Style labelStyle;
    labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_CENTER;
    labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::Yellow;
    labelStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;


    Style placeStyle;
    placeStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;

	osg::Image* pin = osgDB::readImageFile( "placemark64.png");
	labelGroup->addChild( new PlaceNode(mapNode, GeoPoint(geoSRS,_tList[index]._lat, _tList[index]._lon), pin, _tList[index]._name, placeStyle));
	annoGroup->addChild(createRecs());

    //DecorationInstaller highlightInstaller("hover", new HighlightDecoration());
    //annoGroup->accept( highlightInstaller );
    // scale labels when hovering:
    DecorationInstaller scaleInstaller("hover", new ScaleDecoration(3.1f));
    labelGroup->accept( scaleInstaller );
	labelGroup->setNodeMask(0xffffffff); 
	labelGroup->setName("labelGroup");
	//labelGroup->setDataVariance(osg::Object::STATIC);
	//annoGroup->setDataVariance(osg::Object::STATIC);

}

void createSingleTask2()
{
	//srand((unsigned)time(0));
    labelGroup = new osg::Group();
    Decluttering::setEnabled( labelGroup->getOrCreateStateSet(), true );
    annoGroup->addChild( labelGroup );

	osg::Group* lblGroup = new osg::Group();
	annoGroup2->addChild( lblGroup );

    Style labelStyle;
    labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_CENTER;
    labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::Yellow;
    labelStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;


    Style placeStyle;
    placeStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;

	osg::Image* pin = osgDB::readImageFile( "placemark64.png");
	//labelGroup->addChild( new PlaceNode(mapNode, GeoPoint(geoSRS,_tList[index]._lat, _tList[index]._lon), pin, _tList[index]._name, placeStyle));
	//labelGroup->addChild( new PlaceNode(mapNode, GeoPoint(geoSRS,_tList[index+1]._lat, _tList[index+1]._lon), pin, _tList[index+1]._name, placeStyle));
	
	//annoGroup->addChild(createRecs());

	for(unsigned int i=0; i<2; i++)
	{
		float lon = 55.553107 + (float)rand()/((float)RAND_MAX/(55.911888-55.553107));
		float lat = 11.709847 + (float)rand()/((float)RAND_MAX/(13.238319-11.709847));
		labelGroup->addChild( new PlaceNode(mapNode, GeoPoint(geoSRS,lat, lon), pin, "?", placeStyle));
		annoGroup->addChild(createRecsRandomTargets(lon, lat));
	}

	// create random targets
	for(unsigned int i=0; i<8; i++)
	{
		float lon = 55.553107 + (float)rand()/((float)RAND_MAX/(55.911888-55.553107));
		float lat = 11.709847 + (float)rand()/((float)RAND_MAX/(13.238319-11.709847));
		lblGroup->addChild( new PlaceNode(mapNode, GeoPoint(geoSRS,lat, lon), pin, "?", placeStyle));
		annoGroup2->addChild(createRecsRandomTargets(lon, lat));
	}

    //DecorationInstaller highlightInstaller("hover", new HighlightDecoration());
    //annoGroup->accept( highlightInstaller );
    // scale labels when hovering:
    DecorationInstaller scaleInstaller("hover", new ScaleDecoration(3.1f));

 //   labelGroup->accept( scaleInstaller );
	//labelGroup->setNodeMask(0xffffffff); 
	labelGroup->setName("labelGroup");

 //   lblGroup->accept( scaleInstaller );
	//lblGroup->setNodeMask(0xffffffff); 
	lblGroup->setName("lblGroup");

	//labelGroup->setDataVariance(osg::Object::STATIC);
	//annoGroup->setDataVariance(osg::Object::STATIC);

}


struct MyAnnoEventHandler : public AnnotationEventHandler
{

public:
	DataCollector* _timeCollector;

public:
	MyAnnoEventHandler(DataCollector* timeCollector) :
	  _timeCollector(timeCollector) {}

    void onHoverEnter( AnnotationNode* anno, const EventArgs& args )
    {
        //anno->setDecoration( "hover" );

    }

    void onHoverLeave( AnnotationNode* anno, const EventArgs& args )
    {
		if((isAlive && !realDel) && (anno->getNumChildren() == 1))
		{
			int i = 1;
			i++;
		//	//anno->clearDecoration();

		//	//RectangleNode* rect = dynamic_cast<RectangleNode*>(anno);
		//	//Style rectStyle;
		//	//rectStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Green, 0.5);
		//	//rect->setStyle(rectStyle);

		}
		else
		{
			if(realDel && index < 9 && !lastTask)
			{			
				index++;
				if(tasktype == "Navigation")
				{
					_timeCollector->_elabsedTime = _timeCollector->_timer.time_s(); // get time elabsed for navigation task
					_timeCollector->printToFile(); // print a single time record to file

					createSingleTask2Navigation();

					_timeCollector->_timer.setStartTick(); // Navigation Task is started for the next target					

				}
				else if(tasktype == "Search")
				{
					_timeCollector->_elabsedTime = _timeCollector->_timer.time_s(); // get time elabsed for navigation task
					_timeCollector->printToFile(); // print a single time record to file

					createSingleTask2();
					
					_timeCollector->_timer.setStartTick(); // Navigation Task is started for the next target				

				}
				isAlive = true;
				realDel = false;
				if(index==9)
					lastTask = true;
				else
					lastTask = false;
			}
			else if(realDel && lastTask)
			{
				//index++;
				_timeCollector->_elabsedTime = _timeCollector->_timer.time_s(); // get time elabsed for navigation task	
				_timeCollector->printToFile(); // print a single time record to file
				_timeCollector->finalizeFile();
				lastTask = false;
				realDel = false;
			}

		}
    }

    virtual void onClick( AnnotationNode* node, const EventArgs& details )
    {        
		PlaceNode* place = dynamic_cast<PlaceNode*>(node);
		if (place == NULL)
		{

				EarthManipulator* mp = dynamic_cast<EarthManipulator*>(_viewer->getCameraManipulator());
				Viewpoint ptt = mp->getViewpoint();
				if(ptt.getRange() < 5000)
				{
					isAlive = false;
				}	
		}
		else
		{
			//OE_NOTICE << "Thanks for clicking the PlaceNode: " << place->getText() << std::endl;
		}
    }

};

void NavigationTask::createTaskForNavigation(osgViewer::Viewer* viewer, osgEarth::MapNode* mn, DataCollector* _timeCollector)
{
	tasktype = taskType;
	_viewer = viewer;
	_tList = _targetList;
	mapNode = mn;
	geoSRS = mapNode->getMapSRS()->getGeodeticSRS();
	index = 0;
	isAlive = true;//; delimiter = true;
	root = dynamic_cast<osg::Group*>(viewer->getSceneData());
	annoGroup = new osg::Group();
	root->addChild( annoGroup );


	createSingleTask2Navigation(); // create task for navigatino

	_timeCollector->_timer.setStartTick(); // Navigation Task is started for the first target

	    // install an event handler for picking and hovering.
    AnnotationEventCallback* cb = new AnnotationEventCallback();
    cb->addHandler( new MyAnnoEventHandler(_timeCollector));

    annoGroup->addEventCallback( cb );
	annoGroup->addEventCallback( new deleteHandler2() );
	annoGroup->setName("annoGroup");

	viewer->setSceneData( root );
    viewer->getCamera()->addCullCallback( new AutoClipPlaneCullCallback(mapNode) );
    viewer->getDatabasePager()->setDoPreCompile( true );
    viewer->addEventHandler(new osgViewer::WindowSizeHandler());
    viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
}

void NavigationTask::createTask(osgViewer::Viewer* viewer, osgEarth::MapNode* mn, DataCollector* _timeCollector)
{
	_viewer = viewer;
	_tList = _targetList;
	tasktype = taskType;
	mapNode = mn;
	geoSRS = mapNode->getMapSRS()->getGeodeticSRS();
	index = 0;
	isAlive = true;//; delimiter = true;
	root = dynamic_cast<osg::Group*>(viewer->getSceneData());
	annoGroup = new osg::Group();
	annoGroup2 = new osg::Group();
	
	double num = root->getNumChildren();

	root->addChild( annoGroup );
	root->addChild(annoGroup2);

	createSingleTask2();
	
	_timeCollector->_timer.setStartTick(); // Search Task is started for the first target

	    // install an event handler for picking and hovering.
    AnnotationEventCallback* cb = new AnnotationEventCallback();
    cb->addHandler( new MyAnnoEventHandler(_timeCollector));

	//AnnotationEventCallback* cb2 = new AnnotationEventCallback();
 //   cb2->addHandler( new MyAnnoEventHandler2);

    annoGroup->addEventCallback( cb );
	//annoGroup2->addEventCallback( cb2 );
	annoGroup->addEventCallback( new deleteHandler() );
	//annoGroup2->addEventCallback( new deleteHandler() );
	annoGroup->setName("annoGroup");
	annoGroup2->setName("annoGroup2");

	//viewer->setSceneData( root );
    //viewer->getCamera()->addCullCallback( new AutoClipPlaneCullCallback(mapNode) );
    viewer->getDatabasePager()->setDoPreCompile( true );
    //viewer->addEventHandler(new osgViewer::WindowSizeHandler());
    //viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
}


StyleSheet* NavigationTask::buildStyleSheet( osg::Vec4 color, float width )
{
    // Define a style for the feature data. Since we are going to render the
    // vectors as lines, configure the line symbolizer:
    Style style;

    LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
    ls->stroke()->color() = color;
    ls->stroke()->width() = width;

    //AltitudeSymbol* as = style.getOrCreate<AltitudeSymbol>();
    //as->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;

    StyleSheet* styleSheet = new StyleSheet();
    styleSheet->addStyle( style );
    return styleSheet;
}

void NavigationTask::createTask2(osgViewer::Viewer* viewer, osgEarth::MapNode* mapNode) // create tasl for search
{
	delete _navigationUtility;
	_navigationUtility = NULL;

	_navigationUtility = new NavigationUtility();

	_navigationUtility->s_viewer = viewer;
	// Start by creating the map:
	_navigationUtility->s_mapNode = mapNode;

	_navigationUtility->s_mapNode->setNodeMask( 0x01 );
    // Define a style for the feature data. Since we are going to render the
    // vectors as lines, configure the line symbolizer:
	StyleSheet* styleSheet = buildStyleSheet( Color::Red, 4.0f );

	    // create a feature list source with the map extents as the default extent.
    //_navigationUtility->s_source = new FeatureListSource( _navigationUtility->s_mapNode->getMap()->getProfile()->getExtent() );
	_navigationUtility->s_source = new FeatureListSource();
	const SpatialReference* geoSRS = _navigationUtility->s_mapNode->getMapSRS()->getGeodeticSRS();

    LineString* line = new LineString();

	double distortionFactor = .005;


	line->push_back( osg::Vec3d(_targetList[0]._lat+distortionFactor, _targetList[0]._lon, 0.0) );
	line->push_back( osg::Vec3d(_targetList[1]._lat-distortionFactor, _targetList[1]._lon, 0.0) );
	line->push_back( osg::Vec3d(_targetList[2]._lat, _targetList[2]._lon+distortionFactor, 0.0) );
	line->push_back( osg::Vec3d(_targetList[3]._lat-distortionFactor, _targetList[3]._lon, 0.0) );
	line->push_back( osg::Vec3d(_targetList[4]._lat+distortionFactor, _targetList[4]._lon, 0.0) );
	line->push_back( osg::Vec3d(_targetList[5]._lat, _targetList[5]._lon-distortionFactor, 0.0) );
	line->push_back( osg::Vec3d(_targetList[6]._lat, _targetList[6]._lon+distortionFactor, 0.0) );
	line->push_back( osg::Vec3d(_targetList[7]._lat-distortionFactor, _targetList[7]._lon, 0.0) );
	line->push_back( osg::Vec3d(_targetList[8]._lat+distortionFactor, _targetList[8]._lon, 0.0) );
	line->push_back( osg::Vec3d(_targetList[9]._lat, _targetList[9]._lon-distortionFactor, 0.0) );

    Feature* feature = new Feature(line, geoSRS, Style(), _navigationUtility->s_fid++);
    _navigationUtility->s_source->insertFeature( feature );
    _navigationUtility->s_activeFeature = feature;
  
	_navigationUtility->s_root = dynamic_cast<osg::Group*>(_navigationUtility->s_viewer->getSceneData());
    //s_root->addChild( s_mapNode.get() );

    Session* session = new Session(_navigationUtility->s_mapNode->getMap(), styleSheet, _navigationUtility->s_source.get());

    FeatureModelGraph* graph = new FeatureModelGraph( 
        session,
        FeatureModelSourceOptions(), 
        new GeomFeatureNodeFactory() );

    graph->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    graph->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

    _navigationUtility->s_root->addChild( graph );
   
	    // A series of place nodes (an icon with a text label)
	osg::Group* labelGroup = new osg::Group();
    Decluttering::setEnabled( labelGroup->getOrCreateStateSet(), true );
	
    // A lat/long SRS for specifying points.
	

    {
		    // set up a style to use for labels:
		Style placeStyle;
		placeStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;

        //Style pin;
        //pin.getOrCreate<MarkerSymbol>()->url()->set("C:/osgEarth/src/data/placemark32.png");
		osg::Image* pin = osgDB::readImageFile( "placemark64.png");

		labelGroup->addChild( new PlaceNode(_navigationUtility->s_mapNode, GeoPoint(geoSRS,_targetList[0]._lat, _targetList[0]._lon), pin, _targetList[0]._name, placeStyle));
		labelGroup->addChild( new PlaceNode(_navigationUtility->s_mapNode, GeoPoint(geoSRS,_targetList[1]._lat, _targetList[1]._lon), pin, _targetList[1]._name, placeStyle));
		labelGroup->addChild( new PlaceNode(_navigationUtility->s_mapNode, GeoPoint(geoSRS,_targetList[2]._lat, _targetList[2]._lon), pin, _targetList[2]._name, placeStyle));
		labelGroup->addChild( new PlaceNode(_navigationUtility->s_mapNode, GeoPoint(geoSRS,_targetList[3]._lat, _targetList[3]._lon), pin, _targetList[3]._name, placeStyle));
		labelGroup->addChild( new PlaceNode(_navigationUtility->s_mapNode, GeoPoint(geoSRS,_targetList[4]._lat, _targetList[4]._lon), pin, _targetList[4]._name, placeStyle));
		labelGroup->addChild( new PlaceNode(_navigationUtility->s_mapNode, GeoPoint(geoSRS,_targetList[5]._lat, _targetList[5]._lon), pin, _targetList[5]._name, placeStyle));
		labelGroup->addChild( new PlaceNode(_navigationUtility->s_mapNode, GeoPoint(geoSRS,_targetList[6]._lat, _targetList[6]._lon), pin, _targetList[6]._name, placeStyle));
		labelGroup->addChild( new PlaceNode(_navigationUtility->s_mapNode, GeoPoint(geoSRS,_targetList[7]._lat, _targetList[7]._lon), pin, _targetList[7]._name, placeStyle));
		labelGroup->addChild( new PlaceNode(_navigationUtility->s_mapNode, GeoPoint(geoSRS,_targetList[8]._lat, _targetList[8]._lon), pin, _targetList[8]._name, placeStyle));
		labelGroup->addChild( new PlaceNode(_navigationUtility->s_mapNode, GeoPoint(geoSRS,_targetList[9]._lat, _targetList[9]._lon), pin, _targetList[9]._name, placeStyle));

	}

    _navigationUtility->s_root->addChild(labelGroup);
    _navigationUtility->s_viewer->setSceneData( _navigationUtility->s_root.get() );
    //viewer.setCameraManipulator( new EarthManipulator() );

    if ( _navigationUtility->s_mapNode )
        _navigationUtility->s_viewer->getCamera()->addCullCallback( new osgEarth::Util::AutoClipPlaneCullCallback(_navigationUtility->s_mapNode) );

	_navigationUtility->_featureGraph = graph;
	if (!_navigationUtility->s_editor.valid() && _navigationUtility->s_activeFeature.valid())
	{
		Style* style = _navigationUtility->_featureGraph->getStyles()->getDefaultStyle();
		if ( style )
		{
			style->get<LineSymbol>()->stroke()->stipple() = 0x00FF;
			_navigationUtility->_featureGraph->dirty();
		}
		_navigationUtility->s_editor = new FeatureEditor(_navigationUtility->s_activeFeature.get(), _navigationUtility->s_source.get(), _navigationUtility->s_mapNode.get());
		_navigationUtility->s_root->addChild( _navigationUtility->s_editor.get() );
	} 

	_navigationUtility->s_viewer->addEventHandler(new osgViewer::WindowSizeHandler());
    _navigationUtility->s_viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
	//_navigationUtility->s_viewer->addEventHandler(_taskHandler.get());

}