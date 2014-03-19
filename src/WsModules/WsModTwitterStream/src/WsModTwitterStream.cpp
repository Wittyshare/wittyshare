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
{ 
	addStyleClass("WsTwitterStream");
}

WsTwitterStream::~WsTwitterStream()
{ }


void WsTwitterStream::load()
{
  Wt::WApplication::instance()->require("http://platform.twitter.com/widgets.js");
  WContainerWidget::load();
  setId("twitterstream");
  //resize(WLength(25, WLength::Percentage), WLength(25, WLength::Percentage));
  setOverflow(WContainerWidget::OverflowAuto);
  std::string html = "<a class=\"twitter-timeline\" data-dnt=\"true\" href=\"https://twitter.com/EUROFER_eu\" data-widget-id=\"441216909459984384\">Tweets by @EUROFER_eu</a>";

  WText* textField = new WText(this);
  textField->setTextFormat(Wt::XHTMLUnsafeText);
  textField->setText(html);
  addWidget(textField);
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

