// -*- C++ -*-
/*!
 * @file  ImageViewerTest.cpp
 * @brief Image Viewer Component with common camera interface 2.0
 * @date $Date$
 *
 * $Id$
 */

#include "ImageViewerTest.h"

// Module specification
// <rtc-template block="module_spec">
static const char* imageviewer_spec[] =
  {
    "implementation_id", "ImageViewerTest",
    "type_name",         "ImageViewerTest",
    "description",       "Image Viewer Component with common camera interface 2.0",
    "version",           "2.0.0",
    "vendor",            "Kenichi Ohara, Meijo University",
    "category",          "ImageProcessing",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.capture_frame_num", "0",

    // Widget
    "conf.__widget__.capture_frame_num", "text",
    // Constraints

    "conf.__type__.capture_frame_num", "int",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ImageViewerTest::ImageViewerTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_ImageIn("Image", m_Image),
    m_CameraCaptureServicePort("CameraCaptureService")

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
ImageViewerTest::~ImageViewerTest()
{
}



RTC::ReturnCode_t ImageViewerTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  addOutPort("Image", m_ImageOut);
  
  // Set service provider to Ports
  m_CameraCaptureServicePort.registerProvider("CameraCaptureService", "Img::CameraCaptureService", m_CameraCaptureService);
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  addPort(m_CameraCaptureServicePort);
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("capture_frame_num", m_capture_frame_num, "0");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ImageViewerTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewerTest::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewerTest::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t ImageViewerTest::onActivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageViewerTest::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageViewerTest::onExecute(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ImageViewerTest::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewerTest::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewerTest::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewerTest::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewerTest::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void ImageViewerTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(imageviewer_spec);
    manager->registerFactory(profile,
                             RTC::Create<ImageViewerTest>,
                             RTC::Delete<ImageViewerTest>);
  }
  
};


