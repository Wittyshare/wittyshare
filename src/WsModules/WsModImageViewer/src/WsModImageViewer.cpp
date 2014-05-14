#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WImage>

#include <Logger/WsLogger.h>
#include "Main/WsApplication.h"
#include "Main/WsErrorPage.h"

#include "WsModImageViewer.h"

using namespace Wt;

extern "C" {
  void WsModImageViewerInit(void)
  {
    void* hndl = dlopen("libwt.so", RTLD_NOW | RTLD_GLOBAL);
    if ( hndl == NULL ) {
      LOG(ERROR) << "WsModImageViewer :: Cannot load libwt.so shared library! " << dlerror();
      return;
    }
  }
}

WsImageViewer::WsImageViewer(WContainerWidget* parent)
  : WContainerWidget(parent)
{ }

WsImageViewer::~WsImageViewer()
{ }

void WsImageViewer::setDiffPath(const std::string& diffPath)
{
  m_sDiffPath = diffPath;
}

void WsImageViewer::load()
{
  std::string sysPath ="";
  if(asString(option("src")).narrow().size() == 0)
          sysPath = m_sDiffPath + Wt::WApplication::instance()->internalPath();
  else 
    sysPath= m_sDiffPath + "/" + asString(option("src")).narrow();
  clear();
  WsApp->hideImages(false);
  WImage* wimage = new WImage(sysPath);
  addWidget(wimage);
}


    WsModImageViewer::WsModImageViewer()
: WsModule()
{
}

WsModImageViewer::~WsModImageViewer()
{
}

WWidget* WsModImageViewer::createContentsMenuBar(WContainerWidget* parent) const
{
    return 0;
}

WWidget* WsModImageViewer::createContents(WContainerWidget* parent) const
{
    WsImageViewer* pcw = new WsImageViewer(parent);
    pcw->setDiffPath(diffPath());
  pcw->setOptions(options());
  return pcw;
}

WsEditorWidget* WsModImageViewer::createEditor(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModImageViewer::createAdmin(WContainerWidget* parent) const
{
  return 0;
}

bool WsModImageViewer::saveEditor() const
{
  return true;
}

std::string WsModImageViewer::description() const
{
  return "WsModImageViewer wittyShare module";
}

