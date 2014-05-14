#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WText>

#include <Logger/WsLogger.h>
#include <gdcore/gdCore.h>
#include "Main/WsApplication.h"
#include "Main/WsErrorPage.h"

#include "WsModFhtml.h"

using namespace Wt;

extern "C" {
  void WsModFhtmlInit(void)
  {
    void* hndl = dlopen("libwt.so", RTLD_NOW | RTLD_GLOBAL);
    if ( hndl == NULL ) {
      LOG(ERROR) << "WsModFhtml :: Cannot load libwt.so shared library! " << dlerror();
      return;
    }
  }
}

WsFhtml::WsFhtml(WContainerWidget* parent)
  : WContainerWidget(parent)
{ }

WsFhtml::~WsFhtml()
{ }

void WsFhtml::setDiffPath(const std::string& diffPath)
{
  m_sDiffPath = diffPath;
}

void WsFhtml::load()
{
  std::string sysPath ="";
  if(asString(option("src")).narrow().size() == 0)
          sysPath = WsApp->wsUser()->getRootPath() + Wt::WApplication::instance()->internalPath();
  else 
    sysPath= WsApp->wsUser()->getRootPath() + "/" + asString(option("src")).narrow();
  
  std::string fileContent;
    if ( !gdcore_file_content2string(sysPath.c_str(), fileContent) ) {
        LOG(ERROR)<<"WsFhtml :: load() : Error while loading Fhtml file "<<sysPath;
        addWidget(new WsErrorPage(WsErrorPage::Error, wApp->internalPath(), 0, ""));
        return;
    }
    clear();
    WsApp->hideImages(false);
    WText* wtext = new WText();
    wtext->setTextFormat(Wt::XHTMLUnsafeText);
    wtext->setText(fileContent);
    addWidget(wtext);
}


WsModFhtml::WsModFhtml()
  : WsModule()
{
}

WsModFhtml::~WsModFhtml()
{
}

WWidget* WsModFhtml::createContentsMenuBar(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModFhtml::createContents(WContainerWidget* parent) const
{
  WsFhtml* pcw = new WsFhtml(parent);
  pcw->setDiffPath(diffPath());
  pcw->setOptions(options());
  return pcw;
}

WsEditorWidget* WsModFhtml::createEditor(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModFhtml::createAdmin(WContainerWidget* parent) const
{
  return 0;
}

bool WsModFhtml::saveEditor() const
{
  return true;
}

std::string WsModFhtml::description() const
{
  return "WsModFhtml wittyShare module";
}

