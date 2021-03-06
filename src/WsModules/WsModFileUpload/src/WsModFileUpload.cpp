/*
* Copyright (C) 2006-Today Guy Deleeuw
*
* See the LICENSE file for terms of use.
*/

#include <dlfcn.h>
#include <stdio.h>

#include <iostream>
#include <boost/filesystem.hpp>

#include <fcntl.h>   // open
#include <unistd.h>  // read, write, close
#include <cstdio>    // BUFSIZ

#include <Wt/WLogger>
#include <Wt/WText>

#include <gdcore/gdCore.h>

#include <Logger/WsLogger.h>

#include <Main/WsApplication.h>

#include "WsModFileUpload.h"

extern "C" {
  void WsModFileUploadInit(void)
  {
    void* hndl = dlopen("libwt.so", RTLD_NOW | RTLD_GLOBAL);
    if ( hndl == NULL ) {
      fprintf(stderr, "cannot load libwt.so shared library! %s\n", dlerror());
      return;
    }
  }
}

// Class WsFileUpload
// ==================
WsFileUpload::WsFileUpload(WContainerWidget* parent)
  : WContainerWidget(parent), m_pFU(0)
{
  // addStyleClass("WsFileUpload");
}

void WsFileUpload::load()
{
  WContainerWidget::load();
  m_dialog = new WDialog();
  m_dialog->addStyleClass("WsFileUpload");
  m_dialog->setTitleBarEnabled(false);
  m_dialog->contents()->addWidget(new WText("Upload a file with a maximum size of 8Mb"));
  m_pFU = new WFileUpload();
  m_pFU->setProgressBar(new WProgressBar());
  m_pFU->changed().connect(this, &WsFileUpload::doUpload);
  m_pFU->uploaded().connect(this, &WsFileUpload::doUploaded);
  m_pFU->fileTooLarge().connect(this, &WsFileUpload::doFileTooLarge);
  m_dialog->contents()->addWidget(m_pFU);
  WPushButton* cancel = new WPushButton("Cancel ", m_dialog->contents());
  cancel->clicked().connect(SLOT(this, WsFileUpload::doCancel));
  m_dialog->show();
  WApplication::instance()->log("notice") << "WsFileUpload::load : end !";
}

WsFileUpload::~WsFileUpload()
{
}

void WsFileUpload::doUpload()
{
  m_pFU->upload();
}

void WsFileUpload::doCancel()
{
  m_dialog->hide();
  delete m_dialog;
  string internal = wApp->internalPath();
  boost::algorithm::replace_all(internal, "/FileUpload", "");
  wApp->setInternalPath(internal, false);
}

void WsFileUpload::doUploaded()
{
  // TODO : Verif des paths, quid si multiples file ?, optimiser les strings, progressbar marche pas, , ajout d'un champ  : nom desire a l'arrivee ?, test si file existe : replace ?
  std::string sCurUrl          = wApp->internalPath();
  std::string sWithoutPrefix   = WsApp->WsModules().pathWithoutPrefix(sCurUrl);
  WsUser*     pUser            = WsApp->wsUser();
  std::string m_sDocumentRoot  = pUser->getRootPath(); // /var/www/demo_site
  std::string sCurPath         = m_sDocumentRoot + sWithoutPrefix;
  std::string sNewFile;
  if ( gdcore_isPathFile(sCurPath) )
    sNewFile = boost::filesystem::path(sCurPath).parent_path().string() + "/" + m_pFU->clientFileName().toUTF8();
  else
    sNewFile = sCurPath + "/" + m_pFU->clientFileName().toUTF8();
  if (exists(sNewFile)) {
    LOG(DEBUG) << "WsFileUpload::doUploaded(): sNewFile already exist " << sNewFile;
    m_dialog->hide();
    StandardButton
    result = WMessageBox::show("Error", "File already exist in current directory. Replace ?", Ok | Cancel);
    if (result != Ok) {
      //delete m_pFU;
      //delete m_dialog;
      string internal = wApp->internalPath();
      boost::algorithm::replace_all(internal, "/FileUpload", "");
      wApp->setInternalPath(internal, false);
      return;
    }
  }
  try {

    boost::filesystem::copy_file(m_pFU->spoolFileName(), sNewFile);

    boost::algorithm::replace_first(sNewFile, m_sDocumentRoot, "/Edit");
    sleep(1);
    std::string newNode = sNewFile;
    boost::algorithm::replace_all(newNode, "/Edit", "");
    boost::algorithm::replace_first(newNode, "//", "/");
    boost::algorithm::replace_first(sNewFile, "//", "/");
    LOG(DEBUG) << "WsFileUpload::doUploaded(): will now insert node " << newNode;
    pUser->createNode(newNode, WsUser::File);
    m_dialog->hide();
    delete m_dialog;
    wApp->setInternalPath(sNewFile, true);
  } catch  (boost::filesystem::filesystem_error& e) {
    wApp->log("ERROR") << " WsFileUpload::doUploaded() cannot move " << m_pFU->spoolFileName() << " to " << sNewFile << " error = " << e.what();
  }
}

void WsFileUpload::doFileTooLarge(int64_t nSize)
{
  wApp->log("notice") << " WsFileUpload::doFileTooLarge() : size " << nSize;
}


// Class WsModFileUpload
// =====================
WsModFileUpload::WsModFileUpload()
  : WsModule()
{
}

WsModFileUpload::~WsModFileUpload()
{
}

WWidget* WsModFileUpload::createContentsMenuBar(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModFileUpload::createContents(WContainerWidget* parent) const
{
  WsFileUpload* pFU = new WsFileUpload(parent);
  pFU->setOptions(options());
  pFU->outOptions("WsModFileUpload::createContents()");
  return pFU;
}

WsEditorWidget* WsModFileUpload::createEditor(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModFileUpload::createAdmin(WContainerWidget* parent) const
{
  return 0;
}

std::string WsModFileUpload::description() const
{
  return "WsModFileUpload wittyShare module";
}

