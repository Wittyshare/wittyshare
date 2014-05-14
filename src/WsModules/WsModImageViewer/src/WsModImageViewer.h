#ifndef WS_MOD_FHTML_H__
#define WS_MOD_FHTML_H__

#include <Wt/WContainerWidget>

#include <WsModule/src/WsModule.h>

#include <dlfcn.h>
#include <stdio.h>

#include <iostream>

#include <Wt/WContainerWidget>


class WsImageViewer : public Wt::WContainerWidget, public WsOptions {
public :
  WsImageViewer(Wt::WContainerWidget* parent = 0);
  ~WsImageViewer();
  void       setDiffPath(const std::string& diffPath);
  virtual void       load();

private :
  std::string        m_sDiffPath;
};


class WsModImageViewer : public WsModule {
public :
  WsModImageViewer();
  ~WsModImageViewer();
  Wt::WWidget*     createContentsMenuBar(Wt::WContainerWidget* parent = 0) const;
  Wt::WWidget*     createContents(Wt::WContainerWidget* parent = 0) const;
  WsEditorWidget*  createEditor(Wt::WContainerWidget* parent = 0) const;
  Wt::WWidget*     createAdmin(Wt::WContainerWidget* parent = 0) const;
  bool             saveEditor() const;
  std::string      description() const;

private :
};

extern "C" {
  void WsModImageViewerInit(void) __attribute__((constructor));

  WsModImageViewer* buildModule()
  {
    return new WsModImageViewer();
  }
}

#endif

