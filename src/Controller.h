#ifndef H_Controller
#define H_Controller

//#include "CommonHeaders.h"
#include "Commons.h"

class ControllerUtility
{
public:
	ControllerUtility()
	: _ControllerVisibility(false), _idx(12), _keyPress(false)
	{
		_pointerPos.push_back(114.0);_pointerPos.push_back(147.0);_pointerPos.push_back(180.0);_pointerPos.push_back(214.0);
		_pointerPos.push_back(248.0);_pointerPos.push_back(283.0);_pointerPos.push_back(318.0);_pointerPos.push_back(352.0);
		_pointerPos.push_back(385.0);_pointerPos.push_back(418.0);_pointerPos.push_back(452.0);_pointerPos.push_back(486.0);
		_pointerPos.push_back(520.0);_pointerPos.push_back(554.0);_pointerPos.push_back(588.0);_pointerPos.push_back(621.0);
		_pointerPos.push_back(656.0);_pointerPos.push_back(690.0);_pointerPos.push_back(724.0);_pointerPos.push_back(758.0);
		_pointerPos.push_back(792.0);_pointerPos.push_back(826.0);_pointerPos.push_back(860.0);_pointerPos.push_back(893.0);
	}

	static ControllerUtility* instance()
	{
		static ControllerUtility cntU;
		return &cntU;
	}

	bool _ControllerVisibility;
	bool _keyPress;
	osg::Timer _tick;
	std::vector<double> _pointerPos;
	int _idx;
	//int _prevIdx;
};

/* callback to update the value of the pointer on the gauge*/
class UpdateTextCallback : public osg::Drawable::UpdateCallback
{
public:
	UpdateTextCallback(ControllerUtility* cntu) 
	: _cntU(cntu) { }

	virtual void update( osg::NodeVisitor* nv, osg::Drawable* drawable )
	{
		osgText::Text* text = static_cast<osgText::Text*>(drawable);
		if ( text )
		{
			std::ostringstream os; os<<(4 + _cntU->_idx*4);
			text->setText( os.str() );
		}
	}

public:
	ControllerUtility* _cntU;
};


/* callback to hide/show controller GUI and to manipulate the pointer at the gauge*/
class ControllerCallback : public osg::NodeCallback
{
public:
	ControllerCallback(ControllerUtility* cntU)
		: _cntU(cntU) {}

	virtual void operator ()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::ref_ptr<osg::MatrixTransform> mtGaugePointerText = dynamic_cast<osg::MatrixTransform*>(node);

		if(_cntU->_keyPress)
		{

			if(_cntU->_tick.time_s() < 3.0 )
			{
				{
					osg::ref_ptr<osg::MatrixTransform> mtPointer = dynamic_cast<osg::MatrixTransform*>(mtGaugePointerText->getChild(0));
					osg::Matrix mTp, mSp;
					if(_cntU->_idx < 0) _cntU->_idx = 0; if(_cntU->_idx > 23) _cntU->_idx = 23;
					double trans = _cntU->_pointerPos[_cntU->_idx];//_cntU->_pointerPos[_cntU->_idx] - _cntU->_pointerPos[_cntU->_prevIdx];
					mTp.makeTranslate(osg::Vec3d(1500.0*(7680/1680), trans*(3240/1050), 0.0));
					mSp.makeScale(0.3*(7680/1680), 0.08*(3240/1050), 0.0);
					mtPointer->setMatrix(mSp*mTp);
					mtGaugePointerText->setNodeMask(0xffffffff); 
				}

			}
			else
			{
				mtGaugePointerText->setNodeMask(0x5); // hide gauge, pointer, text
				_cntU->_keyPress = false;
			}

		}
		traverse(node, nv);
	}

public:
	ControllerUtility* _cntU;
};

/*keyboard event handler, m for increasing, n for decreasing value */
class KeyboardEventHandler : public osgGA::GUIEventHandler
{
public: 
        KeyboardEventHandler(ControllerUtility* cntU)
		: _cntU(cntU) {}
    
        virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
        {

            switch(ea.getEventType())
            {
                case(osgGA::GUIEventAdapter::KEYDOWN):
                {
					int bb = ea.getKey();
					if(ea.getKey() == 110) //n decrease pointer value
					{
						_cntU->_keyPress = true;
						_cntU->_idx--;
						_cntU->_tick.setStartTick();

					}
					else if(ea.getKey() == 109) //m  increase pointer value
					{
						_cntU->_keyPress = true;
						_cntU->_idx++;
						_cntU->_tick.setStartTick();
					}
					else if(ea.getKey() == 102)
					{
						return false;
					}
					else
					{
						return false;
					}
                    return true;
                }
				case(osgGA::GUIEventAdapter::DOUBLECLICK):
				{
					return true;	
				}
				case(osgGA::GUIEventAdapter::DRAG):
				{
					return true;	
				}
				case(osgGA::GUIEventAdapter::SCROLL):
				{
					return true;	
				}
                default:
                    return false;
            }
        }    

public:
	ControllerUtility* _cntU;

};

	/*focusCircleEventHandler*/
class focusCircleEventHandler : public osgGA::GUIEventHandler
{
public: 
        focusCircleEventHandler(focusCircle* fcircle)
		: fCircle(fcircle) {}
    
        virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
        {
			// repositon cursor to gPos position here
			osg::Vec2d gpos = fCircle->getGazePos();
			double radius = fCircle->getFocusCircleRadius();
			double x = ea.getX()-gpos.x(), y = ea.getY()-gpos.y();
			if(x*x + y*y > radius*radius) // cursor is outside focus circle
			{
				osg::Vec2d Uv(ea.getX()-gpos.x(), ea.getY()-gpos.y());
				double trash = Uv.normalize();
				osg::Vec2d newCursorPos(gpos.x()+radius*Uv.x(), gpos.y()+radius*Uv.y());
				newCursorPos -= Uv*100;
				aa.requestWarpPointer(newCursorPos.x(),newCursorPos.y());
			}
			return false;
        }    

public:
	focusCircle* fCircle;

};


#endif