#include "Cursor.h"
#include "NavigationTask.h"

class ColorLabelMenu;
std::vector<ColorLabelMenu*> _menuList;

std::string taskName = "";
osg::Timer exp_time;
bool started = false;
std::string endoffile="";
int timeCounter = 1;
std::ofstream location_record;
std::ofstream gaze_record;
std::ofstream scale_record;
std::ofstream space_record;
std::ofstream boundary_record;

double elapsed_time;
//osg::Vec3d pos; osg::Quat quat;

std::string fileName = "Position_Data ";
std::string gazeFileName = "Gaze_Data ";
std::string scaleFileName = "Scale_Data.txt";
std::string spaceFileName = "Space_Data.txt";
std::string boundaryFileName = "Boundary_Data.txt";

struct ColorLabel: public osgWidget::Label {

public:
	osgViewer::Viewer* _viewer;
	Pos _latlon;
	NavigationTask _task;
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	bool _firstTime;
	

public:
    ColorLabel(const char* label, osgViewer::Viewer* viewer): 
	_viewer(viewer), osgWidget::Label("", ""), _mapNode(new osgEarth::MapNode()), _firstTime(true) {
        setFont("fonts/Vera.ttf");
        setFontSize(14);
        setFontColor(1.0f, 1.0f, 1.0f, 1.0f);
        setColor(0.3f, 0.3f, 0.3f, 1.0f);
        addHeight(18.0f);
        setCanFill(true);
        setLabel(label);
        setEventMask(osgWidget::EVENT_MOUSE_PUSH | osgWidget::EVENT_MASK_MOUSE_MOVE);
    }

	ColorLabel(std::string label, Pos latlon, osgViewer::Viewer* viewer, NavigationTask task, osgEarth::MapNode* mapNode):
		_viewer(viewer), _latlon(latlon), osgWidget::Label("", ""), _task(task), _mapNode(mapNode),  _firstTime(true) {
        setFont("fonts/Vera.ttf");
        setFontSize(14);
        setFontColor(1.0f, 1.0f, 1.0f, 1.0f);
        setColor(0.3f, 0.3f, 0.3f, 1.0f);
        addHeight(18.0f);
        setCanFill(true);
        setLabel(label);
        setEventMask(osgWidget::EVENT_MOUSE_PUSH | osgWidget::EVENT_MASK_MOUSE_MOVE);
    }

    bool mousePush(double, double, const osgWidget::WindowManager*) {	
		//smoothPanZoom();
		//if(_firstTime)
		//{
		//_task.createTask(_viewer, _mapNode);
		//	 _firstTime = false;
		//}
        return true;
    }

    bool mouseEnter(double, double, const osgWidget::WindowManager*) {
        setColor(0.4f, 0.4f, 0.4f, 1.0f);
        
        return true;
    }

    bool mouseLeave(double, double, const osgWidget::WindowManager*) {
        setColor(0.3f, 0.3f, 0.3f, 1.0f);
        
        return true;
    }

	void smoothPanZoom()
	{
		// do the usual setup
		double duration = 3.0;
		EarthManipulator* manip = dynamic_cast<EarthManipulator*>(_viewer->getCameraManipulator());
		osgEarth::Viewpoint view(_latlon._name, _latlon._lat, _latlon._lon, 0, 0, -90, 100000.0, SpatialReference::create("epsg:4326"));
		manip->setViewpoint(view, duration);
		/*********/

	}

};


struct keh : public osgGA::GUIEventHandler
{
public:
	ColorLabel* cl;
	DataCollector* _timeCollector;
	std::ostringstream os;

public:
	keh(ColorLabel* c)
	{ 
		cl = c;
		time_t rawtime;
		struct tm * timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
 
		os<<timeinfo->tm_hour<<"_"<<timeinfo->tm_min<<"_"<<timeinfo->tm_sec;
		fileName += os.str() + ".txt";
		gazeFileName += os.str() + ".txt"; 
		location_record.open(fileName, ios::app);
		gaze_record.open(gazeFileName, ios::app);

		//initializing scale space recording process


	}
    virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
    {
        switch(ea.getEventType())
        {
            case(osgGA::GUIEventAdapter::KEYDOWN):
            {
				int bb = ea.getKey();
				if(ea.getKey() == 49) // Navigatino Task
				{

					taskName = "Navigation";
					cl->_task.taskType = "Navigation"; 

					osg::Group* group = dynamic_cast<osg::Group*>(cl->_viewer->getSceneData());
					int num = group->getNumChildren();
					if(num == 3) // navigation
					{
						group->removeChild(2);
						cl->_viewer->setSceneData(group);					
					}
					if(num == 4) //search
					{
						group->removeChild(3);
						group->removeChild(2);
						cl->_viewer->setSceneData(group);					
					}					
					else if(num == 5) // route tracing
					{
						group->removeChild(4);group->removeChild(3);group->removeChild(2);
						cl->_viewer->setSceneData(group);					
					}

					std::vector<Pos> targetListCopenhagen;
					targetListCopenhagen.push_back(Pos("Vanlose St(Metro)", 12.491654, 55.687308));
					targetListCopenhagen.push_back(Pos("Frederikssund St(Metro)", 12.065669, 55.835608));
					targetListCopenhagen.push_back(Pos("AmagerBro St(Metro)", 12.602919, 55.663332));
					targetListCopenhagen.push_back(Pos("Roskilde St(Metro)", 12.088549, 55.638994));
					targetListCopenhagen.push_back(Pos("Farum St(Metro)", 12.373518, 55.811926));
					targetListCopenhagen.push_back(Pos("Solrød Strand St(Metro)", 12.218062, 55.53327));
					targetListCopenhagen.push_back(Pos("Fasanvej St(Metro)", 12.522415, 55.681813));
					targetListCopenhagen.push_back(Pos("Kokkedal St(Metro)", 12.50206, 55.903442));
					targetListCopenhagen.push_back(Pos("Christianshavn St(Metro)", 12.591372, 55.672109));
					targetListCopenhagen.push_back(Pos("Holbæk St(Metro)", 11.708635, 55.715644));

					cl->_task.setTargetList(targetListCopenhagen);

					cl->smoothPanZoom();

					// instantiate _timeCollector object here
					_timeCollector = new DataCollector();
					_timeCollector->_taskName = "Navigation"; // registering task name as Navigation
					_timeCollector->_fileName = "Time_Data_Navigation_"; 
					_timeCollector->_fileName += os.str() + ".txt";
					_timeCollector->timerFile.open(_timeCollector->_fileName, ios::app);

					cl->_task.createTaskForNavigation(cl->_viewer, cl->_mapNode, _timeCollector);

				}
				else if(ea.getKey() == 50) // Trace-1 task
				{
					taskName = "Trace-1";
					cl->_task.taskType = "Trace-1";
					osg::Group* group = dynamic_cast<osg::Group*>(cl->_viewer->getSceneData());
					int num = group->getNumChildren();
					if(num == 3) // navigation
					{
						group->removeChild(2);
						cl->_viewer->setSceneData(group);					
					}
					if(num == 4) //search
					{
						group->removeChild(3);
						group->removeChild(2);
						cl->_viewer->setSceneData(group);					
					}					
					else if(num == 5) // route tracing
					{
						group->removeChild(4);group->removeChild(3);group->removeChild(2);
						cl->_viewer->setSceneData(group);					
					}

					std::vector<Pos> targetListCopenhagen;
					targetListCopenhagen.push_back(Pos("Holbæk St(Metro)", 11.708635, 55.715644));
					targetListCopenhagen.push_back(Pos("Roskilde St(Metro)", 12.088549, 55.638994));
					targetListCopenhagen.push_back(Pos("Trekroner St(Metro)", 12.132272, 55.647935));
					targetListCopenhagen.push_back(Pos("Hedehusene St(Metro)", 12.197104, 55.649101));
					targetListCopenhagen.push_back(Pos("Albertslund St(Metro)", 12.353117, 55.65804));
					targetListCopenhagen.push_back(Pos("Rødovre St(Metro)", 12.45864, 55.664821));
					targetListCopenhagen.push_back(Pos("Vanlose St(Metro)", 12.491654, 55.687308));
					targetListCopenhagen.push_back(Pos("Fasanvej St(Metro)", 12.522415, 55.681813));
					targetListCopenhagen.push_back(Pos("Nørreport St(Metro)", 12.571068, 55.683848));
					targetListCopenhagen.push_back(Pos("AmagerBro St(Metro)", 12.602919, 55.663332));

					cl->_task.setTargetList(targetListCopenhagen);
					cl->smoothPanZoom();
					cl->_task.createTask2(cl->_viewer, cl->_mapNode);

				}
				else if(ea.getKey() == 51) // Trace-2 task
				{
					taskName = "Trace-2";
					cl->_task.taskType = "Trace-2";
					osg::Group* group = dynamic_cast<osg::Group*>(cl->_viewer->getSceneData());
					int num = group->getNumChildren();
					if(num == 3) // navigation
					{
						group->removeChild(2);
						cl->_viewer->setSceneData(group);					
					}
					if(num == 4) //search
					{
						group->removeChild(3);
						group->removeChild(2);
						cl->_viewer->setSceneData(group);					
					}					
					else if(num == 5) // route tracing
					{
						group->removeChild(4);group->removeChild(3);group->removeChild(2);
						cl->_viewer->setSceneData(group);					
					}

					std::vector<Pos> targetListCopenhagen;
					targetListCopenhagen.push_back(Pos("Kävlinge St(Metro)", 13.11145, 55.794428));
					targetListCopenhagen.push_back(Pos("Lund Centralstation St(Metro)", 13.18686, 55.705251));
					targetListCopenhagen.push_back(Pos("Malmö Triangeln St(Metro)", 13.001506, 55.593208));
					targetListCopenhagen.push_back(Pos("Lufthavnen St(Metro)", 12.649386, 55.630142));
					targetListCopenhagen.push_back(Pos("Nørreport St(Metro)", 12.571068, 55.683848));
					targetListCopenhagen.push_back(Pos("østerport St(Metro)", 12.589135, 55.693524));
					targetListCopenhagen.push_back(Pos("Bernstorffsvej St(Metro)", 12.557485, 55.743054));
					targetListCopenhagen.push_back(Pos("Nærum St(Metro)", 12.528313, 55.813927));
					targetListCopenhagen.push_back(Pos("Rungsted Kyst St(Metro)", 12.531473, 55.882229));
					targetListCopenhagen.push_back(Pos("Kokkedal St(Metro)", 12.502055, 55.90349));

					cl->_task.setTargetList(targetListCopenhagen);
					cl->smoothPanZoom();
					cl->_task.createTask2(cl->_viewer, cl->_mapNode);

				}
				else if(ea.getKey() == 52) // search-1
				{



					taskName = "Search";
					cl->_task.taskType = "Search";
					osg::Group* group = dynamic_cast<osg::Group*>(cl->_viewer->getSceneData());
					int num = group->getNumChildren();
					if(num == 3) // navigation
					{
						group->removeChild(2);
						//cl->_viewer->setSceneData(group);					
					}
					if(num == 4) //search
					{
						group->removeChild(3);
						group->removeChild(2);
						//cl->_viewer->setSceneData(group);					
					}					
					else if(num == 5) // route tracing
					{
						group->removeChild(4);group->removeChild(3);group->removeChild(2);
						//cl->_viewer->setSceneData(group);					
					}

					std::vector<Pos> targetListCopenhagen;
					srand((unsigned)time(0));
					// create random targets
					for(unsigned int i=0; i<10; i++)
					{
						float lon = 55.553107 + (float)rand()/((float)RAND_MAX/(55.911888-55.553107));
						float lat = 11.709847 + (float)rand()/((float)RAND_MAX/(13.238319-11.709847));

						targetListCopenhagen.push_back(Pos("Vanlose St(Metro)", lat, lon));

					}


					cl->_task.setTargetList(targetListCopenhagen);

					cl->smoothPanZoom();

					// instantiate _timeCollector object here
					_timeCollector = new DataCollector();
					_timeCollector->_taskName = "Search"; // registering task name as Navigation
					_timeCollector->_fileName = "Time_Data_Search_"; 
					_timeCollector->_fileName += os.str() + ".txt";
					_timeCollector->timerFile.open(_timeCollector->_fileName, ios::app);

					cl->_task.createTask(cl->_viewer, cl->_mapNode, _timeCollector);

				}

				else if(ea.getKey() == 110) // n start timer
				{
					if(started || taskName=="")
						return true;

					timeCounter = 1;
					started = true;
					exp_time.setStartTick();
					
					scale_record.open(scaleFileName, ios::app);
					space_record.open(spaceFileName, ios::app);
					boundary_record.open(boundaryFileName, ios::app);

					scale_record<<"... "<<taskName<<" ...."<<std::endl;
					space_record<<"... "<<taskName<<" ...."<<std::endl;
					boundary_record<<"... "<<taskName<<" ..."<<std::endl;

					// close scale space record files
					scale_record.close();
					space_record.close();
					boundary_record.close();

				}
				else if(ea.getKey() == 109) // m stop timer
				{
					if(!started)
						return true;


					//experiment.close();

					started = false;
					taskName = "";

				}
				else
				{
					return false;
				}
                return true;
            }
            default:
                return false;
       }
	}

};

//void fixBoundary(osg::Vec3d& pos)
//{
//	// LEGALIZE:- fix boundary condition
//	double xPos = pos.x(), yPos = pos.y();
//	_boundaryCollisionFlag = false;
//	if (pos.x()>bottomLeftScreenCoordWorld.x()) 
//	{
//		xPos=bottomLeftScreenCoordWorld.x();// : xPos = pos.x();
//		//_boundaryCollisionFlag = true; 
//	}
//	if(pos.x()<topRightScreenCoordWorld.x()) 
//	{
//		xPos=topRightScreenCoordWorld.x();// : xPos = pos.x();
//		//_boundaryCollisionFlag = true; 
//	}
//	if(pos.y()<bottomLeftScreenCoordWorld.y()) 
//	{
//		yPos=bottomLeftScreenCoordWorld.y();// : yPos = pos.y();
//		_boundaryCollisionFlag = true;  
//	}
//	if(pos.y()>topRightScreenCoordWorld.y()) 
//	{
//		yPos=topRightScreenCoordWorld.y();// : yPos = pos.y();
//		_boundaryCollisionFlag = true; 
//	}
//	pos.set(xPos, yPos, pos.z());
//	
//}


struct expTimerCallback : public osg::Drawable::UpdateCallback
{
public:
	double t;
	osg::Vec3d pos, gPos;
	osg::Quat quat;
	CursorUtility cu;

	expTimerCallback() :
		gPos(osg::Vec3d(0.0,0.0,0.0)), t(0)	
		{}

	virtual void update( osg::NodeVisitor* nv, osg::Drawable* drawable )
	{
		if(exp_time.time_s() >= timeCounter && started && taskName != "")
		{
			timeCounter++;
			elapsed_time = int(exp_time.time_s());
			if(DataReceiverThread::instance()->getPos(pos, quat))
			{
				pos.set(pos.x(), pos.y()-0.2, pos.z());
				osg::Vec3d rotatedVector = quat*osg::Vec3d(0.0, 0.0, 1.0);
				Line line(pos, rotatedVector);
				Plane plane(cu.bottomLeftScreenCoordWorld,cu.topRightScreenCoordWorld, osg::Vec3d(0.0, 0.0, 1.0));
				bool res = plane.intersect(line,t);
				gPos = pos + rotatedVector*t;
				cu.fixBoundary(gPos);
				gPos.set(gPos.x(), gPos.y(), 0.0);
			}

			pos *= 100; osg::Vec3d gPosLocal;
			cu.worldToScreenCoord(gPos, gPosLocal);
			location_record<<taskName<<setw(20)<<elapsed_time<<"  "<<pos.x()<<","<<pos.y()<<","<<pos.z()<<std::endl;
			gaze_record<<taskName<<setw(20)<<elapsed_time<<"  "<<gPosLocal.x()<<","<<gPosLocal.y()<<","<<gPosLocal.z()<<std::endl;
		}
	}
		
};


class ColorLabelMenu: public ColorLabel {

private:
    osg::ref_ptr<osgWidget::Window> _window;
	std::string _label;
	osgViewer::Viewer* _viewer;

public:
	ColorLabelMenu(const char* label, osgViewer::Viewer* viewer, osgEarth::MapNode* mapNode):
    _label(label), _viewer(viewer), ColorLabel(label, viewer) {
        _window = new osgWidget::Box(
            std::string("Menu_") + label,
            osgWidget::Box::VERTICAL,
            true
        );

		std::vector<Pos> latlonPos;
		latlonPos.push_back(Pos("Copenhagen", 12.583, 55.7));
		latlonPos.push_back(Pos("Aarthus",10.207131, 56.156928));
		latlonPos.push_back(Pos("Odense", 10.400594, 55.402218));
		latlonPos.push_back(Pos("Aalborg",  9.928722, 57.037181));

		/*creating Navigation Tasks*/
		std::vector<NavigationTask> taskList;

		std::vector<Pos> targetListCopenhagen;
		std::vector<Pos> targetListAarthus;
		std::vector<Pos> targetListOdense;
		std::vector<Pos> targetListAalborg;

		
		//targetListCopenhagen.push_back(Pos("Flintholm St(Metro)", 12.499525, 55.68587));
		//targetListCopenhagen.push_back(Pos("Lindevang St(Metro)", 12.512859, 55.683479));
		//targetListCopenhagen.push_back(Pos("Fasanvej St(Metro)", 12.522415, 55.681813));
		//targetListCopenhagen.push_back(Pos("Frederiksberg St(Metro)", 12.533989, 55.681205));
		//targetListCopenhagen.push_back(Pos("Forum St(Metro)", 12.552429, 55.681823));
		//targetListCopenhagen.push_back(Pos("Nørreport St(Metro)", 12.571068, 55.683848));
		//targetListCopenhagen.push_back(Pos("Kongens Nytorv St(Metro)", 12.585375, 55.679115));
		//targetListCopenhagen.push_back(Pos("Christianshavn St(Metro)", 12.591372, 55.672109));
		
		targetListCopenhagen.push_back(Pos("Vanlose St(Metro)", 12.491654, 55.687308));
		targetListCopenhagen.push_back(Pos("Frederikssund St(Metro)", 12.065669, 55.835608));
		targetListCopenhagen.push_back(Pos("AmagerBro St(Metro)", 12.602919, 55.663332));
		targetListCopenhagen.push_back(Pos("Roskilde St(Metro)", 12.088549, 55.638994));
		targetListCopenhagen.push_back(Pos("Farum St(Metro)", 12.373518, 55.811926));
		targetListCopenhagen.push_back(Pos("Solrød Strand St(Metro)", 12.218062, 55.53327));
		targetListCopenhagen.push_back(Pos("Fasanvej St(Metro)", 12.522415, 55.681813));
		targetListCopenhagen.push_back(Pos("Kokkedal St(Metro)", 12.50206, 55.903442));
		targetListCopenhagen.push_back(Pos("Christianshavn St(Metro)", 12.591372, 55.672109));
		targetListCopenhagen.push_back(Pos("Holbæk St(Metro)", 11.708635, 55.715644));

		taskList.push_back(NavigationTask(targetListCopenhagen));


		//taskList.push_back(NavigationTask(Pos("Østbanetorvet St(Metro)", 10.21641, 56.163274), Pos("Nørrevænget St(Metro)", 10.150432, 56.09239), targetListAarthus));
		//taskList.push_back(NavigationTask(Pos("Odense St(Metro)", 10.386746, 55.401785), Pos("Pederstrup St(Metro)", 10.486208, 55.283313), targetListOdense));
		//taskList.push_back(NavigationTask(Pos("Aalborg Vestby St(Metro)", 9.908603, 57.053283), Pos("Støvring St(Metro)", 9.843953, 56.890977), targetListAalborg));
		ColorLabel* cl = new ColorLabel(latlonPos[0]._name, latlonPos[0], _viewer, taskList[0], mapNode);
		_window->addWidget(cl);
		cl->setUpdateCallback(new expTimerCallback);
		_viewer->addEventHandler(new keh(cl));
	
        //_window->addWidget(new ColorLabel(latlonPos[1]._name, latlonPos[1], _viewer, taskList[1], mapNode));
        //_window->addWidget(new ColorLabel(latlonPos[2]._name, latlonPos[2], _viewer, taskList[2], mapNode));
        //_window->addWidget(new ColorLabel(latlonPos[3]._name, latlonPos[3], _viewer, taskList[3], mapNode));

        _window->resize();

        setColor(0.0f, 0.0f, 0.0f, 0.8f);
    }

	 osg::ref_ptr<osgWidget::Window> getWindow()
	 {
		return _window;
	 }

	 std::string getMenulabel()
	 {
		 return _label;
	 }

    void managed(osgWidget::WindowManager* wm) {
        osgWidget::Label::managed(wm);

        wm->addChild(_window.get());

        _window->hide();
    }

    void positioned() {
        osgWidget::Label::positioned();

        _window->setOrigin(_window->getX(), getHeight());
        _window->resize(getWidth());
    }

	bool mouseEnter(double, double, const osgWidget::WindowManager*) {
        setColor(0.4f, 0.4f, 0.4f, 1.0f);
        
        return true;
    }

    bool mousePush(double, double, const osgWidget::WindowManager*) {
        //if(!_window->isVisible()) _window->show();

        //else _window->hide();

		for(unsigned int i=0; i<_menuList.size(); i++)
		{
			if(_label == _menuList[i]->getLabel())
				if(!_window->isVisible()) _window->show();
				else  _window->hide();
			else 
				_menuList[i]->getWindow()->hide();
		}	

        return true;
    }

    bool mouseLeave(double, double, const osgWidget::WindowManager*) {
        if(!_window->isVisible()) setColor(0.0f, 0.0f, 0.0f, 0.8f);
        return true;
    }
};


class TaskMenu
{
public:
	TaskMenu()
	{}

	osgWidget::Window* createTaskMenu(osgViewer::Viewer* viewer, osgEarth::MapNode* mapNode)
	{
		_menu = new osgWidget::Box("menu", osgWidget::Box::HORIZONTAL);

		_menuList.push_back(new ColorLabelMenu("Navigation Task", viewer, mapNode));
		//_menuList.push_back(new ColorLabelMenu("Search Task", viewer, mapNode));
		//_menuList.push_back(new ColorLabelMenu("Pattern Task", viewer, mapNode));
		//_menuList.push_back(new ColorLabelMenu("Random Task", viewer, mapNode));

		for(unsigned int i=0; i<_menuList.size(); i++)
			_menu->addWidget(_menuList[i]);
		
		return _menu;
	}

private:
	osgWidget::Window* _menu;
};
