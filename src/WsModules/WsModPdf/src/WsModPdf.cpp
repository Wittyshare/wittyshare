#include <Wt/WApplication>
#include <Wt/WLogger>
#include <Wt/WText>

#include <Logger/WsLogger.h>

#include "WsModPdf.h"

using namespace Wt;

extern "C" {
  void WsModPdfInit(void)
  {
    void* hndl = dlopen("libwt.so", RTLD_NOW | RTLD_GLOBAL);
    if ( hndl == NULL ) {
      LOG(ERROR) << "WsModPdf :: Cannot load libwt.so shared library! " << dlerror();
      return;
    }
  }
}

WsPdf::WsPdf(WContainerWidget* parent)
  : WContainerWidget(parent)
{ }

WsPdf::~WsPdf()
{ }

void WsPdf::setDiffPath(const std::string& diffPath)
{
  m_sDiffPath = diffPath;
}

void WsPdf::load()
{
  WContainerWidget::load();
  resize(WLength(100, WLength::Percentage), WLength(100, WLength::Percentage));
  setOverflow(WContainerWidget::OverflowAuto);
  std::string  p1 = Wt::WApplication::instance()->internalPath();
  std::string  p(m_sDiffPath + p1);
  WText* text = new WText();
  text->setTextFormat(XHTMLUnsafeText);
  text->setText("<canvas id='the-canvas' style='border:1px solid black;'/><button id='prev' onclick='goPrevious()'>Previous</button> <button id='next' onclick='goNext()'>Next</button>");
  addWidget(text);
  if ( p.size() > 0 ) {
      Wt::WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "pdfjs/src/pdf.js");
      Wt::WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "pdfjs/src/shared/util.js");
      Wt::WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "pdfjs/src/shared/colorspace.js");
      Wt::WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "pdfjs/src/shared/function.js");
      Wt::WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "pdfjs/src/shared/annotation.js");
      Wt::WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "pdfjs/src/display/api.js");
      Wt::WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "pdfjs/src/display/metadata.js");
      Wt::WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "pdfjs/src/display/canvas.js");
      Wt::WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "pdfjs/src/display/webgl.js");
      Wt::WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "pdfjs/src/display/pattern_helper.js");
      Wt::WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "pdfjs/src/display/font_loader.js");
      std::string javaScript = "\
    PDFJS.workerSrc='" + WApplication::instance()->resourcesUrl() + "pdfjs/src/worker_loader.js';\
    PDFJS.getDocument('"+p+"').then(function(pdf) {\
        pdf.getPage(1).then(function(page) {\
            var scale = 1;\
            var viewport = page.getViewport(scale);\
            var canvas = document.getElementById('the-canvas');\
            var context = canvas.getContext('2d');\
            canvas.height = viewport.height;\
            canvas.width = viewport.width;\
            var renderContext = {\
                canvasContext: context,\
                viewport: viewport\
            };\
            page.render(renderContext);\
            });\
        });";
    WApplication::instance()->doJavaScript(javaScript);
  }
}





WsModPdf::WsModPdf()
  : WsModule()
{
  // Add the required javascript file
  WApplication::instance()->require(WApplication::instance()->resourcesUrl() + "webpdf/src/pdf.js");
}

WsModPdf::~WsModPdf()
{
}

WWidget* WsModPdf::createContentsMenuBar(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModPdf::createContents(WContainerWidget* parent) const
{
  WsPdf* pcw = new WsPdf(parent);
  pcw->setDiffPath(diffPath());
  pcw->setOptions(options());
  return pcw;
}

WsEditorWidget* WsModPdf::createEditor(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModPdf::createAdmin(WContainerWidget* parent) const
{
  return 0;
}

bool WsModPdf::saveEditor() const
{
  return true;
}

std::string WsModPdf::description() const
{
  return "WsModPdf wittyShare module";
}

