// -*- C++ -*-
/*!
 * @file  ImageViewer.cpp
 * @brief Image Viewer Component for common camera interface
 * @date $Date$
 *
 * $Id$
 */

#include "ImageViewer.h"

// Module specification
// <rtc-template block="module_spec">
static const char* imageviewer_spec[] =
  {
    "implementation_id", "ImageViewer",
    "type_name",         "ImageViewer",
    "description",       "Image Viewer Component for common camera interface",
    "version",           "1.0.0",
    "vendor",            "Kenichi Ohara, Meijo University",
    "category",          "Image Process",
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
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ImageViewer::ImageViewer(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_ImageIn("Image", m_Image),
    m_CameraCaptureServicePort("CameraCaptureService")

    // </rtc-template>
{
  //Initialize varuables
  image				= NULL;
  width				= 0;
  height			= 0;
  channels			= 0;
  connection_check  = NULL;
  saved_image_counter= 0;
}

/*!
 * @brief destructor
 */
ImageViewer::~ImageViewer()
{
}

RTC::ReturnCode_t ImageViewer::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("Image", m_ImageIn);
  
  // Set OutPort buffer
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  m_CameraCaptureServicePort.registerConsumer("CameraCaptureService", "Img::CameraCaptureService", m_CameraCaptureService);
  
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
RTC::ReturnCode_t ImageViewer::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t ImageViewer::onActivated(RTC::UniqueId ec_id)
{
  PortServiceList* portlist;
  
  portlist = this->get_ports();
  connection_check = new bool [portlist->length()];
  for(unsigned int i=0; i<portlist->length(); ++i)
  {
	PortService_ptr port;
	port = (*portlist)[i];
	if(port->get_port_profile()->connector_profiles.length()!=0)
	  connection_check[i]=true;
	else
      connection_check[i]=false;
  }

  //サービスポート接続状態のチェック
  if(connection_check[1])
  {
      //連続画像取得モードに設定
	  if(m_capture_frame_num == 0)
	  {
		std::cout << "start continuous image streaming." << std::endl;
		RTC_INFO(("Send command of \"start continuous\" via CameraCaptureService."));
		m_CameraCaptureService->start_continuous();
	  }
	  //1shotキャプチャモードに設定
	  else if(m_capture_frame_num == 1)
	  {
		std::cout << "take one frame." << std::endl;
		RTC_INFO(("Send command of \"take one frame\" via CameraCaptureService."));
		m_CameraCaptureService->take_one_frame();
	  }
	  //指定枚数キャプチャモードに設定
	  else if(m_capture_frame_num > 1)
	  {
		std::cout << "take multi frames (" << m_capture_frame_num << " frames)." << std::endl;
		RTC_INFO(("Send command of \"take multi frames\" via CameraCaptureService."));
		m_CameraCaptureService->take_multi_frames(m_capture_frame_num);
	  }
	  else
	  {
		std::cerr << "Please set capture_frame_num to more than 0" << std::endl;
		RTC_ERROR(("Configuration Param <frames_num> should be over 0. [%d]",m_capture_frame_num));
		return RTC::RTC_ERROR;
	  }
  }

  cv::namedWindow("Image Window", CV_WINDOW_AUTOSIZE);
  std::cout << "Start image view" << std::endl;
  std::cout << "If you want to take a 1 shot image as image file, please push s on Captured Image Window!" << std::endl;

  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageViewer::onDeactivated(RTC::UniqueId ec_id)
{
  if(connection_check[1]){
	//連続キャプチャモードの場合は、キャプチャを停止
    if(m_capture_frame_num == 0)
	{
      std::cout << "Stop image streaming" << std::endl;
      RTC_INFO(("Send command of \"stop continuous\" via CameraCaptureService."));
      m_CameraCaptureService->stop_continuous();
    }
  }

  if(connection_check != NULL)
	delete [] connection_check;
  
  //描画ウィンドウの消去
  cv::destroyWindow("Image Window");
  std::cout << "Stop image view" << std::endl;

  //描画用画像メモリの解放
  image.release();
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ImageViewer::onExecute(RTC::UniqueId ec_id)
{
  //Inport data check
  if(m_ImageIn.isNew())
  {
	m_ImageIn.read();
	std::cerr<< "tm.sec " << m_Image.tm.sec << std::endl;
	std::cerr<< "tm.nsec " << m_Image.tm.nsec << std::endl;
	
	width = m_Image.data.image.width;
	height = m_Image.data.image.height;
	channels = (m_Image.data.image.format == Img::CF_GRAY) ? 1 :
			   (m_Image.data.image.format == Img::CF_RGB) ? 3 :
			   (m_Image.data.image.raw_data.length()/width/height);
	RTC_TRACE(("Capture image size %d x %d", width, height));
	RTC_TRACE(("Channels %d", channels));
	
	if(channels == 3)
      image.create(height, width, CV_8UC3);
	else
      image.create(height, width, CV_8UC1);

    for(int i=0; i<height; ++i)
	  memcpy(&image.data[i*image.step],&m_Image.data.image.raw_data[i*width*channels],sizeof(unsigned char)*width*channels);
	
	std::cerr<< "Intrinsic matrix elements: " << std::endl;
	for(int i=0; i<5; ++i)
	{
	  std::cerr<< m_Image.data.intrinsic.matrix_element[i] << " " ;
	  RTC_TRACE(("Intrinsic matrix element[%d] %f",i, m_Image.data.intrinsic.matrix_element[i]));
    }
	std::cerr << std::endl;

	std::cerr << "Distortion Parameters" << std::endl;
    for(unsigned int i=0; i<m_Image.data.intrinsic.distortion_coefficient.length(); ++i)
	{
	  std::cerr << m_Image.data.intrinsic.distortion_coefficient[i] << " , ";
	  RTC_TRACE(("Distortion parameter[%d] %f", i, m_Image.data.intrinsic.distortion_coefficient[i]));
    }
	std::cerr << std::endl;

	if(channels == 3)
	  cv::cvtColor(image, image, CV_RGB2BGR);
  }

  //画像データが入っている場合は画像を表示
  if(!image.empty())
    cv::imshow("Image Window", image);

  char key = cv::waitKey(3);

  //Image save process
  if ( key == 's')
  {
    char file[80];
    sprintf( file, "CapturedImage%03d.png",++saved_image_counter);
    cv::imwrite( file, image );
  }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ImageViewer::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ImageViewer::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/



extern "C"
{
 
  void ImageViewerInit(RTC::Manager* manager)
  {
    coil::Properties profile(imageviewer_spec);
    manager->registerFactory(profile,
                             RTC::Create<ImageViewer>,
                             RTC::Delete<ImageViewer>);
  }
  
};


