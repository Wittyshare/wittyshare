/*
* Copyright (C) 2006-Today Guy Deleeuw
*
* See the LICENSE file for terms of use.
*/

#include <dlfcn.h>
#include <stdio.h>

#include <iostream>

#include <boost/filesystem.hpp>

#include <Wt/WLogger>
#include <Wt/WText>

#include <gdcore/gdCore.h>

#include <Logger/WsLogger.h>

#include <Main/WsApplication.h>

#include "WsModEditorUploader.h"

extern "C" {
  void WsModEditorUploaderInit(void)
  {
    fprintf(stderr, "preparing required libraries : libwt.so! %s\n", dlerror());
    void* hndl = dlopen("libwt.so", RTLD_NOW | RTLD_GLOBAL);
    if ( hndl == NULL ) {
      fprintf(stderr, "cannot load libwt.so shared library! %s\n", dlerror());
      return;
    }
  }
}

// Class WsEditorUploader
// ==================
WsEditorUploader::WsEditorUploader(WContainerWidget* parent)
  : WContainerWidget(parent), m_pFU(0), m_label(0), m_destPath(0), m_uploadOther(0)
{
  addStyleClass("WsEditorUploader");
}

void WsEditorUploader::load()
{
  if(m_uploadOther != 0){
      m_uploadOther->hide();
      delete m_uploadOther;
      m_uploadOther = 0;
  }
  if(m_destPath != 0){
      m_destPath->hide();
      delete m_destPath;
  }

  WContainerWidget::load();
  if(m_label != 0){
      m_label->hide();
      delete m_label;
  }
  m_label =  new WText("Upload a file with a maximum size of 8Mb");
  addWidget(m_label);
  m_pFU = new WFileUpload();
  m_pFU->setProgressBar(new WProgressBar());
  //  m_pFU->changed().connect(this, &WFileUpload::upload);
  m_pFU->changed().connect(this, &WsEditorUploader::doUpload);
  m_pFU->uploaded().connect(this, &WsEditorUploader::doUploaded);
  m_pFU->fileTooLarge().connect(this, &WsEditorUploader::doFileTooLarge);
  addWidget(m_pFU);
  WApplication::instance()->log("notice") << "WsEditorUploader::load : end !";
}

WsEditorUploader::~WsEditorUploader()
{
  LOG(DEBUG) << "WsEditorUploader::~WsEditorUploader() !";
}

void WsEditorUploader::doUpload()
{
  wApp->log("notice") << " WsEditorUploader::doUpload() : start upload";
  m_pFU->upload();
}


void WsEditorUploader::doUploaded()
{
  // TODO : Verif des paths, quid si multiples file ?, optimiser les strings, progressbar marche pas, , ajout d'un champ  : nom desire a l'arrivee ?, test si file existe : replace ?
  wApp->log("notice") << " WsEditorUploader::doUploaded() " << m_pFU->spoolFileName();
  std::string sCurUrl          = wApp->internalPath();
  std::string sWithoutPrefix   = WsApp->WsModules().pathWithoutPrefix(sCurUrl);
  WsUser*     pUser            = WsApp->wsUser();
  std::string m_sDocumentRoot  = pUser->getRootPath(); // /var/www/demo_site
  std::string sCurPath         = m_sDocumentRoot + sWithoutPrefix;
  wApp->log("notice") << " WsEditorUploader::doUploaded() current path = " << sCurPath;
  std::string sNewFile;

  if ( gdcore_isPathFile(sCurPath) )
    sNewFile = boost::filesystem::path(sCurPath).parent_path().string() + "/ws.res/" + m_pFU->clientFileName().toUTF8();
  else
    sNewFile = sCurPath + "/ws.res/" + m_pFU->clientFileName().toUTF8();
  LOG(DEBUG)<<"WsEditorUploader::doUploaded(): sNewFile is "<<sNewFile;
  if(exists(sNewFile)){
      LOG(DEBUG)<<"WsEditorUploader::doUploaded(): sNewFile already exist "<<sNewFile;
      if(m_uploadOther != 0){
          m_uploadOther->hide();
          delete m_uploadOther;
          m_uploadOther = 0;
      }
    m_pFU->hide();
    m_label->setText("File already exist");
    m_uploadOther = new WPushButton("Upload another file");
    m_uploadOther->clicked().connect(this, &WsEditorUploader::load);
    addWidget(m_uploadOther);
    return;
  }
  //Check if the ws.res directory exists otherwise create it.
  if(!exists(path(sNewFile).parent_path())){
      LOG(DEBUG)<<"WsEditorUploader::doUploaded(): creating ws.res directory in "<<path(sNewFile).parent_path();
      if(!create_directory(path(sNewFile).parent_path())){
          if(m_uploadOther != 0){
              m_uploadOther->hide();
              delete m_uploadOther;
              m_uploadOther = 0;
          }
          m_pFU->hide();
          m_label->setText("Error: Could not create ws.dir directory");
          m_uploadOther = new WPushButton("Upload another file");
          m_uploadOther->clicked().connect(this, &WsEditorUploader::load);
          addWidget(m_uploadOther);
          return;
      }
  }

  wApp->log("notice") << " WsEditorUploader::doUploaded() current dir = " << sNewFile;
  try {
    boost::filesystem::copy_file(m_pFU->spoolFileName(), sNewFile, copy_option::overwrite_if_exists);
    //  The remove is made by the Wt::WFileUpload class
    //    boost::filesystem::remove(m_pFU->spoolFileName());
    boost::algorithm::replace_first(sNewFile, m_sDocumentRoot, "/Edit");
    wApp->log("notice") << " WsEditorUploader::doUploaded() set internalPath to : " << sNewFile;
    sleep(1);
    std::string newNode = sNewFile;
    boost::algorithm::replace_all(newNode, "/Edit", "");
    boost::algorithm::replace_first(newNode, "//", "/");
    boost::algorithm::replace_first(sNewFile, "//", "/");
    pUser->createNode(newNode, WsUser::File);
    m_pFU->hide();
    m_label->setText("File Uploaded to: ");
    m_destPath = new WText(newNode);
    addWidget(m_destPath);
    m_uploadOther = new WPushButton("Upload another file");
    m_uploadOther->clicked().connect(this, &WsEditorUploader::load);
    addWidget(m_uploadOther);
    

  } catch  (boost::filesystem::filesystem_error& e) {
    wApp->log("ERROR") << " WsEditorUploader::doUploaded() cannot move " << m_pFU->spoolFileName() << " to " << sNewFile << " error = " << e.what();
  }
}

void WsEditorUploader::doFileTooLarge(int64_t nSize)
{
  wApp->log("notice") << " WsEditorUploader::doFileTooLarge() : size " << nSize;
}


// Class WsModEditorUploader
// =====================
WsModEditorUploader::WsModEditorUploader()
  : WsModule()
{
  WApplication::instance()->log("notice") << "end ctor of WsModEditorUploader !";
}

WsModEditorUploader::~WsModEditorUploader()
{
  //  WApplication::instance()->log("notice") << "end dtor of WsModEditorUploader !";
}

WWidget* WsModEditorUploader::createContentsMenuBar(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModEditorUploader::createContents(WContainerWidget* parent) const
{
  WsEditorUploader* pFU = new WsEditorUploader(parent);
  wApp->log("notice") <<  "WsModEditorUploader::createContents() : prepare options ";
  pFU->setOptions(options());
  pFU->outOptions("WsModEditorUploader::createContents()");
  return pFU;
}

WsEditorWidget* WsModEditorUploader::createEditor(WContainerWidget* parent) const
{
  return 0;
}

WWidget* WsModEditorUploader::createAdmin(WContainerWidget* parent) const
{
  return 0;
}

std::string WsModEditorUploader::description() const
{
  return "WsModEditorUploader wittyShare module";
}

