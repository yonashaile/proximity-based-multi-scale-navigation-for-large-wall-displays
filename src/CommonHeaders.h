#include <osgGA/GUIEventHandler>
#include <osg/Notify>
#include <osgGA/StateSetManipulator>
#include <osg/Geode>
#include <osgDB/ReadFile>
#include <osgText/Text>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osg/NodeCallback>
#include <osg/Camera>
#include <osg/Group>
#include <osgWidget/Util>
#include <osgWidget/WindowManager>
#include <osgWidget/Box>
#include <osgWidget/Label>

#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/ExampleResources>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarth/Utils>

#include <osgEarthSymbology/Style>
#include <osgEarthFeatures/FeatureModelGraph>
#include <osgEarthFeatures/FeatureListSource>
#include <osgEarthFeatures/GeometryCompiler>
#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/HighlightDecoration>
#include <osgEarthAnnotation/ScaleDecoration>
#include <osgEarthUtil/AnnotationEvents>
#include <osgEarthAnnotation/AnnotationRegistry>

#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthDrivers/agglite/AGGLiteOptions>
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#include <osgEarthDrivers/model_feature_stencil/FeatureStencilModelOptions>

#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthUtil/Controls>

#include <osgEarthAnnotation/FeatureEditing>

#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#ifndef MASK_2D
#define MASK_2D  0xF0000000
#endif

#ifndef MASK_3D
#define MASK_3D  0x0F000000
#endif


#include <osgEarth/ECEF>
#include <osgEarthAnnotation/AnnotationEditing>

#include <osgEarthAnnotation/ImageOverlay>
#include <osgEarthAnnotation/ImageOverlayEditor>

#include <osgEarthAnnotation/RectangleNode>
#include <osgEarthAnnotation/EllipseNode>
#include <osgEarthAnnotation/LabelNode>

#include <osgEarthAnnotation/LocalGeometryNode>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/Decluttering>

#include <osgEarthAnnotation/ScaleDecoration>

#include <osgEarthSymbology/GeometryFactory>


#include <osgEarth/Pickers>

#include <osg/LineWidth>
#include <osg/Geometry>
#include <osg/math>