#include <Logger/WsLogger.h>

#include "WsModEditor.h"

extern "C" {
  void WsModEditorInit(void)
  {
    void* hndl = dlopen("libwt.so", RTLD_NOW | RTLD_GLOBAL);
    if ( hndl == NULL ) {
      LOG(ERROR) << "WsModEditor :: Cannot load libwt.so shared library! " << dlerror();
      return;
    }
  }
}

WsModEditor::WsModEditor()
  : WsModule()
{
  // Add the required javascript file
  WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "codemirror/lib/codemirror.js");
  WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "codemirror/lib/.js");
}

WsModEditor::~WsModEditor()
{
}

WWidget* WsModEditor::createContentsMenuBar(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModEditor::createContents(WContainerWidget* parent) const
{
  WContainerWidget* pcw = new WContainerWidget(parent);
  pcw->setId("editor");
  pcw->resize(WLength(100, WLength::Percentage), WLength(100, WLength::Percentage));
  pcw->setOverflow(WContainerWidget::OverflowAuto);
  std::string  p1 = Wt::WApplication::instance()->internalPath();
  //std::string  p = "/demo_site/about/wt.odt";
  std::string  p(diffPath() + p1);
  if ( p.size() > 0 ) {
    std::string javaScript = "\
                })";
    WApplication::instance()->doJavaScript(javaScript);
  }
  return pcw;
}

WsEditorWidget* WsModEditor::createEditor(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModEditor::createAdmin(WContainerWidget* parent) const
{
  return 0;
}

std::string WsModEditor::description() const
{
  return "WsModEditor wittyShare module";
}

