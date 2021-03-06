#include <stdio.h>

#include <iostream>

#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WText>

#include <Logger/WsLogger.h>

#include "WsModVideo.h"
#include "WsVideo.h"

extern "C" {
  void WsModVideoInit(void)
  {
    void* hndl = dlopen("libwt.so", RTLD_NOW | RTLD_GLOBAL);
    if ( hndl == NULL ) {
      LOG(ERROR) << "WsModVideo :: Cannot load libwt.so shared library! " << dlerror();
      return;
    }
  }
}

using namespace Wt;

WsModVideo::WsModVideo()
  : WsModule()
{
}

WsModVideo::~WsModVideo()
{
}

WWidget* WsModVideo::createContentsMenuBar(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModVideo::createContents(WContainerWidget* parent) const
{
  WsVideo* pVideo = new WsVideo(parent);
  pVideo->setOptions(options());
  return pVideo;
}

WsEditorWidget* WsModVideo::createEditor(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModVideo::createAdmin(WContainerWidget* parent) const
{
  return 0;
}

std::string WsModVideo::description() const
{
  return "WsModVideo wittyShare module";
}

