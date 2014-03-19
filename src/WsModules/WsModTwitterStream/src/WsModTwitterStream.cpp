#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/WTimer>

#include <Logger/WsLogger.h>

#include "WsModTwitterStream.h"

using namespace Wt;

extern "C" {
  void WsModTwitterStreamInit(void)
  {
    LOG(DEBUG) << "WsModTwitterStream :: Preparing required libraries : libwt.so";
    void* hndl = dlopen("libwt.so", RTLD_NOW | RTLD_GLOBAL);
    if ( hndl == NULL ) {
      LOG(ERROR) << "WsModTwitterStream :: Cannot load libwt.so shared library! " << dlerror();
      return;
    }
  }
}

WsTwitterStream::WsTwitterStream(WContainerWidget* parent)
  : WContainerWidget(parent)
{ }

WsTwitterStream::~WsTwitterStream()
{ }


void WsTwitterStream::execJs(){
  std::string js = "!function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0],p=/^http:/.test(d.location)?'http':'https';if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src=p+\"://platform.twitter.com/widgets.js\";fjs.parentNode.insertBefore(js,fjs);}}(document,\"script\",\"twitter-wjs\");";
  doJavaScript(js);
}

void WsTwitterStream::load()
{
  WContainerWidget::load();
  setId("twitterstream");
  resize(WLength(25, WLength::Percentage), WLength(25, WLength::Percentage));
  setOverflow(WContainerWidget::OverflowAuto);
  std::string html = "<a class=\"twitter-timeline\" data-dnt=\"true\" href=\"https://twitter.com/EUROFER_eu\" data-widget-id=\"441216909459984384\">Tweets by @EUROFER_eu</a>";

  WTextArea* textField = new WTextArea(html);
  addWidget(textField);
  WTimer::singleShot(400, SLOT(this, WsTwitterStream::execJs));
}

WsModTwitterStream::WsModTwitterStream()
  : WsModule()
{
  LOG(DEBUG) << "end ctor of WsModTwitterStream !";
}

WsModTwitterStream::~WsModTwitterStream()
{
  LOG(DEBUG) << "end dtor of WsModTwitterStream !";
}

WWidget* WsModTwitterStream::createContentsMenuBar(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModTwitterStream::createContents(WContainerWidget* parent) const
{
  WsTwitterStream* pcw = new WsTwitterStream(parent);
  return pcw;
}

WsEditorWidget* WsModTwitterStream::createEditor(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModTwitterStream::createAdmin(WContainerWidget* parent) const
{
  return 0;
}

bool WsModTwitterStream::saveEditor() const
{
  return true;
}

std::string WsModTwitterStream::description() const
{
  return "WsModTwitterStream wittyShare module";
}

