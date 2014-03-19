
#ifndef WS_MOD_PDF_H__
#define WS_MOD_PDF_H__

#include <Wt/WContainerWidget>

#include <WsModule/src/WsModule.h>

#include <dlfcn.h>
#include <stdio.h>

#include <iostream>

#include <Wt/WContainerWidget>


class WsTwitterStream : public Wt::WContainerWidget, public WsOptions {
public :
  WsTwitterStream(Wt::WContainerWidget* parent = 0);
  ~WsTwitterStream();
  virtual void       load();
public slots:
  void execJs();

};


class WsModTwitterStream : public WsModule {
public :
  WsModTwitterStream();
  ~WsModTwitterStream();
  Wt::WWidget*     createContentsMenuBar(Wt::WContainerWidget* parent = 0) const;
  Wt::WWidget*     createContents(Wt::WContainerWidget* parent = 0) const;
  WsEditorWidget*  createEditor(Wt::WContainerWidget* parent = 0) const;
  Wt::WWidget*     createAdmin(Wt::WContainerWidget* parent = 0) const;
  bool             saveEditor() const;
  std::string      description() const;


private :
};

extern "C" {
  void WsModTwitterStreamInit(void) __attribute__((constructor));

  WsModTwitterStream* buildModule()
  {
    return new WsModTwitterStream();
  }
}

#endif

