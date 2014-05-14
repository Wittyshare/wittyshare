#ifndef WS_MOD_FHTML_H__
#define WS_MOD_FHTML_H__

#include <Wt/WContainerWidget>

#include <WsModule/src/WsModule.h>

#include <dlfcn.h>
#include <stdio.h>

#include <iostream>

#include <Wt/WContainerWidget>


class WsFhtml : public Wt::WContainerWidget, public WsOptions {
public :
  WsFhtml(Wt::WContainerWidget* parent = 0);
  ~WsFhtml();
  void       setDiffPath(const std::string& diffPath);
  virtual void       load();

private :
  std::string        m_sDiffPath;
};


class WsModFhtml : public WsModule {
public :
  WsModFhtml();
  ~WsModFhtml();
  Wt::WWidget*     createContentsMenuBar(Wt::WContainerWidget* parent = 0) const;
  Wt::WWidget*     createContents(Wt::WContainerWidget* parent = 0) const;
  WsEditorWidget*  createEditor(Wt::WContainerWidget* parent = 0) const;
  Wt::WWidget*     createAdmin(Wt::WContainerWidget* parent = 0) const;
  bool             saveEditor() const;
  std::string      description() const;

private :
};

extern "C" {
  void WsModFhtmlInit(void) __attribute__((constructor));

  WsModFhtml* buildModule()
  {
    return new WsModFhtml();
  }
}

#endif

