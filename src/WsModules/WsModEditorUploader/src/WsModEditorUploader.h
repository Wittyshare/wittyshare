/*
* Copyright (C) 2006-Today Guy Deleeuw
*
* See the LICENSE file for terms of use.
*/

#ifndef WsModEditorUploader_H__
#define WsModEditorUploader_H__    1

#include <Wt/WContainerWidget>
#include <Wt/WFileUpload>
#include <Wt/WProgressBar>

#include <WsModule/WsModule.h>

using namespace Wt;

/*!
   \file WsModEditorUploader.h
   \brief a wittyShare module

 */

class WsEditorUploader : public Wt::WContainerWidget, public WsOptions {
public :
  /*! \brief CTor. */
  WsEditorUploader(Wt::WContainerWidget* parent = 0);
  ~WsEditorUploader();
  void                load();

public slots :
  void                doUpload();
  void                doFileTooLarge(int64_t nSize);
  void                doUploaded();

private :
  Wt::WFileUpload*    m_pFU;
  Wt::WText*           m_label;
  Wt::WText*           m_destPath;
  Wt::WPushButton*      m_uploadOther;
};

class WsModEditorUploader : public WsModule {
public :
  /*! \brief CTor. */
  WsModEditorUploader();
  ~WsModEditorUploader();
  Wt::WWidget*        createContentsMenuBar(Wt::WContainerWidget* parent = 0) const;
  Wt::WWidget*        createContents(Wt::WContainerWidget* parent = 0) const;
  WsEditorWidget*     createEditor(Wt::WContainerWidget* parent = 0) const;
  Wt::WWidget*        createAdmin(Wt::WContainerWidget* parent = 0) const;
  std::string         description() const;
};

extern "C" {
  // http://phoxis.org/2011/04/27/c-language-constructors-and-destructors-with-gcc/
  void WsModEditorUploaderInit(void) __attribute__((constructor));

  WsModEditorUploader* buildModule()
  {
    return new WsModEditorUploader();
  }
}

#endif // ifndef WsModEditorUploader_H__

