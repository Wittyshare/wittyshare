#include <dlfcn.h>
#include <stdio.h>

#include <iostream>

#include <Include/WsGlobalConfig.h>

#include <Wt/WLogger>
#include <Wt/WTheme>
#include <Wt/WText>

#include <Main/WsApplication.h>

#include <Wt/WApplication>
#include <Wt/WTextArea>
#include <Wt/WTimer>

#include <Logger/WsLogger.h>

#include "WsModTwitterStream.h"
#include <boost/lexical_cast.hpp>

using namespace Wt;

extern "C" {
  void WsModTwitterStreamInit(void)
  {
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
  Wt::WApplication::instance()->require("//platform.twitter.com/widgets.js");
  WContainerWidget::load();
  std::string width = Wt::asString(option("width")).toUTF8();
  std::string url = Wt::asString(option("url")).toUTF8();
  std::string widgetId = Wt::asString(option("widgetId")).toUTF8();
  std::string text = Wt::asString(option("text")).toUTF8();
  setId("twitterstream");
  //resize(WLength(25, WLength::Percentage), WLength(25, WLength::Percentage));
  setOverflow(WContainerWidget::OverflowAuto);
  std::string html = "<a class=\"twitter-timeline\"";
  if (width!="")
    html+=" width=\""+width+"\"";
  html+=" data-dnt=\"true\" href=\""+url+"\" data-widget-id=\""+widgetId+"\">"+text+"</a>";
  wApp->log("notice") << "WsModTwitterStream::load() "+html;
  WText* textField = new WText(this);
  textField->setTextFormat(Wt::XHTMLUnsafeText);
  textField->setText(html);
  addWidget(textField);
}

WsModTwitterStream::WsModTwitterStream()
  : WsModule()
{
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

