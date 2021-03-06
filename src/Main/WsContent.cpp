/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <iostream>
#include <fstream>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/thread.hpp>

#include <Wt/WLogger>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WOverlayLoadingIndicator>
#include <Wt/WVBoxLayout>
#include <Wt/WProgressBar>
#include <Wt/WTheme>

#include <User/WsUser.h>

#include <gdcore/gdCore.h>

#include <gdwtcore/gdFHtmlEditor.h>

#include "WsFileResource.h"
#include "WsApplication.h"
#include "WsContent.h"
#include "WsSearchView.h"
#include "WsTemplate.h"
#include "WsFormConfig.h"
#include "WsErrorPage.h"

using namespace Wt;

WsContent::WsContent(WContainerWidget* parent)
  : WContainerWidget(parent), m_bLogContent(false), m_bAllowHtmlRendering(false), m_pSiteMapView(0), m_curWForm(0)
{
  //    if ( WsLayoutProperties::instance()->get("global", "by_object_stylesheet", "false") == "true" )
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
    wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "wittyshare/Css/WsContent.css");
  addStyleClass("WsContent");
  if (WsLayoutProperties::instance()->get("global", "log_content", "false") == "true")
    m_bLogContent = true;
  m_searchPath = WsLayoutProperties::instance()->get("global", "search_path", "/Search");
  if (WsLayoutProperties::instance()->get("global", "allow_html_rendering", "false") == "true")
    m_bAllowHtmlRendering = true;
  WsUser* pUser           = WsApp->wsUser();
  m_sDocumentRoot         = pUser->getRootPath(); // /var/www/demo_site
  m_httpDocumentRoot      = gdWApp->getParameter("DOCUMENT_ROOT", "/var/www");
  m_sRelativeDocumentRoot = m_sDocumentRoot;
  boost::algorithm::replace_first(m_sRelativeDocumentRoot, m_httpDocumentRoot, ""); // /demo_site
  if ( m_bLogContent )
    wApp->log("notice") << "WsContent::WsContent wittyshare Document Root = " << m_sDocumentRoot
                        << " http server DOCUMENT_ROOT = "                    << m_httpDocumentRoot
                        << " Relative DOCUMENT_ROOT = "                       << m_sRelativeDocumentRoot;
}

void WsContent::clear()
{
  WContainerWidget::clear();
  m_pSiteMapView = 0;
  m_curWForm = 0;
}

void WsContent::load()
{
  WContainerWidget::load();
}

WsContent::~WsContent()
{ }

void WsContent::buildFileUpload(const std::string& path)
{
  clear();
  addWidget(WsApp->WsModules().module("WsModFileUpload")->createContents());
}

void WsContent::doEditPage(std::string path)
{
  std::string newPath = path;
  boost::algorithm::replace_first(newPath, "/Edit", "");
  boost::algorithm::replace_first(newPath, "/SiteMap", "");
  std::string sPathWithoutPrefix = WsApp->WsModules().pathWithoutPrefix(newPath); // ex. /SiteMap
  std::string     sysPath(m_sDocumentRoot + sPathWithoutPrefix);
  WsUser*         pUser       = WsApp->wsUser();
  NodePtr         pNode       = pUser->getAccessRoot()->eatPath(sPathWithoutPrefix);
  if (!pNode.get() ) {
    addWidget(new WsErrorPage(WsErrorPage::Error, path, pUser, "Returned node is null"));
    return;
  }
  clear();
  setOverflow(WContainerWidget::OverflowAuto);
  WVBoxLayout* vbox = new WVBoxLayout();
  setLayout(vbox);
  WsFormConfig* m_formConfig = new WsFormConfig(pNode, WsApp->WsModules());
  vbox->addWidget(m_formConfig, 0);
  vbox->addWidget(WsApp->WsModules().module("WsModEditorUploader")->createContents());
  std::string     strExt(boost::filesystem::extension(sPathWithoutPrefix));
  if ( strExt == ".fhtml" ) {
    if ( !gdcore_isPathFile(sysPath) ) return;
    gdFHtmlEditor* pEditor = new gdFHtmlEditor("", false);
    pEditor->setCurrentPath(sysPath);
    pEditor->readFile();
    m_formConfig->setEditorFhtml(pEditor);
    pEditor->resize(WLength(100, WLength::Percentage), WLength(400));
    vbox->addWidget(pEditor, 1);
  }
}

void WsContent::buildSiteMap()
{
  clear();
  setOverflow(WContainerWidget::OverflowHidden);
  int nTmp = 0;
  nTmp |= gdWFileView::tbUseLeftTree;
  m_pSiteMapView = new WsSiteMapView(gdwtcore::typeOpen, (gdWFileView::tbFlags) nTmp, this);
  m_pSiteMapView->fileSelected().connect(SLOT(this, WsContent::doSiteMapItemSelected));
  resize(WLength(100, WLength::Percentage), WLength(100, WLength::Percentage));
}

void WsContent::siteMapChanged(std::string newPath)
{
  if ( !m_pSiteMapView )
    buildSiteMap();
  m_pSiteMapView->setRelativePath(newPath);  // "/SiteMap/...
}

void WsContent::doSearch(WString sSearch)
{
  clear();
  WsSearchView* srchView = new WsSearchView(sSearch.toUTF8(), this);
  resize(WLength(100, WLength::Percentage), WLength(100, WLength::Percentage));
  addWidget(srchView);
}

void WsContent::viewDirectory(const std::string& path)
{
  WsUser*         pUser        = WsApp->wsUser();
  NodePtr pRootNode    = pUser->getAccessRoot();
  if ( !pRootNode ) {
    wApp->log("notice") <<  "WsContent::viewDirectory() : Path = " << path << ", getAccessRoot() is null";
    return;
  }
  NodePtr pNode        = pRootNode.get()->eatPath(path);
  if ( !pNode.get() ) {
    wApp->log("notice") <<  "WsContent::viewDirectory() : Path = " << path << ", getAccessRoot() eatPath is null";
    return;
  }
  std::string     sInitPage    = pNode.get()->getProperties().get()->get("global", "initial_page", "");
  if ( sInitPage.size() > 0 ) {
    std::string newPage =  pNode.get()->getPath().string() + "/" + sInitPage;
    wApp->log("notice") <<  "WsContent::viewDirectory() : set initial page to " << newPage;
    return wApp->setInternalPath(newPage, true);
  }
  clear();
  if ( !(pUser->isAdministrator() || pUser->isEditor()) )
    if ( WsLayoutProperties::instance()->get("global", "use_content_directory_view", "false") == "false" ) return;
  WsApp->hideImages(false);
  WWidget* w = WsApp->WsModules().module("WsModDirectoryView2")->createContents();
  WsOptions* wOpt = dynamic_cast<WsOptions*>(w);
  wOpt->setOption("rootPath", path);
  if ( 0 ) {
    WVBoxLayout*  vBox = new WVBoxLayout();
    vBox->addWidget(w, 1);
    setLayout(vBox);
    setOverflow(WContainerWidget::OverflowHidden);
  } else {
    addWidget(w);
    setOverflow(WContainerWidget::OverflowAuto);
  }
}

void WsContent::setPath(std::string newPath)
{
  std::string sPathWithoutPrefix = WsApp->WsModules().pathWithoutPrefix(newPath);
  boost::algorithm::replace_all(sPathWithoutPrefix, "&amp;",  "&");
  if ( newPath == "/" ) newPath = WsApp->homePage();
  WsUser*        pUser    = WsApp->wsUser();
  boost::filesystem::path p(newPath);
  std::string             strExt (p.extension().string());
  // TODO : Make this more robust
  if ( strExt == ".itpl" ) {
    sPathWithoutPrefix = p.parent_path().string();
  }
  int perms = pUser->getPermissions(sPathWithoutPrefix);
  if ( perms == ErrorCode::NotLogged ) {
    addWidget(new WsErrorPage(WsErrorPage::UnAuthorized, p.string(), pUser));
    return;
  }
  if ( perms != GlobalConfig::Read && perms != GlobalConfig::ReadWrite) {
    clear();
    if (perms == ErrorCode::NotFound) {
      addWidget(new WsErrorPage(WsErrorPage::NotFound, p.string(), pUser));
    } else {
      addWidget(new WsErrorPage(WsErrorPage::Forbidden, p.string(), pUser));
    }
    return;
  }
  if ( newPath == "/Logo" ) {
    viewDirectory("/");
    return;
  }
  std::string s;
  try {
    s.assign(newPath, 0, m_searchPath.size());
  } catch (string serr) {
    wApp->log("notice") << "WsContent::setPath : assign crash : " << newPath << " error " << serr;
  }
  if ( m_searchPath == s ) {
    std::string query;
    query.assign(newPath, m_searchPath.size(), newPath.size() -  m_searchPath.size());
    boost::algorithm::replace_first(query, "/", "");
    return doSearch(query);
  }
  if ( newPath.compare(0, 8, "/SiteMap") == 0)
    return siteMapChanged(newPath);
  //Edit and upload only available when editor
  if ( newPath.compare(0, 5, "/Edit") == 0)
    if (perms == GlobalConfig::ReadWrite)
      return doEditPage(newPath);
    else {
      addWidget(new WsErrorPage(WsErrorPage::Forbidden, newPath, pUser));
      return;
    }
  if ( newPath.compare(0, 11, "/FileUpload") == 0 )
    if (perms == GlobalConfig::ReadWrite )
      return buildFileUpload(newPath);
    else {
      addWidget(new WsErrorPage(WsErrorPage::Forbidden, newPath, pUser));
      return;
    }
  selectWidget(newPath);
}

void WsContent::selectWidget(std::string path)
{
  std::string             sysPath(m_sDocumentRoot + path);
  boost::filesystem::path p(path);
  std::string             strExt (p.extension().string());
  std::string             strName(p.stem().string());
  std::string             strFileName(strName + strExt);
  std::string             fileContent;
  if ( m_bLogContent )
    wApp->log("notice") << "WsContent::selectWidget :  path = " << path << " name = " << strName << " extension = " << strExt << " system path = " << sysPath;
  // This extension is mainly created for allowing text in a image without a link
  if ( strExt == ".nolink" ) {
    addWidget(new WsErrorPage(WsErrorPage::Error, path, 0, "Extension not allowed", true));
    return;
  }
  // .html
  if ( strExt == ".html" ) {
    if ( m_bLogContent )
      wApp->log("notice") << "WsContent::selectWidget : render a html file : " << sysPath;
    if ( !m_bAllowHtmlRendering ) {
      addWidget(new WsErrorPage(WsErrorPage::Error, path, 0, "Rendering a html file is not allowed"));
      return;
    }
    clear();
    WsApp->hideImages(false);
    WText* pIFrame = new WText();
    pIFrame->setTextFormat(XHTMLUnsafeText);
    pIFrame->setText("<iframe src='" + m_sRelativeDocumentRoot + path + "' height='98%' width='100%' frameborder='0'></iframe>");
    addWidget(pIFrame);
    return;
  }
  // .form
  if ( strExt == ".form" ) {
    if ( m_bLogContent )
      wApp->log("notice") << "WsContent::selectWidget : render a form : " << sysPath;
    clear();
    m_curWForm = new gdWForm(sysPath);
    addWidget(m_curWForm);
    return;
  }
  // .itpl, Inside template : Extension that create a template in the content.
  if ( strExt == ".itpl" ) {
    if ( m_bLogContent )
      wApp->log("notice") << "WsContent::selectWidget : render an inside template : " << sysPath;
    clear();
    WsTemplate* pTemplate = new WsTemplate(strFileName);
    addWidget(pTemplate);
    return;
  }
  // directory
  if ( gdcore_isPathDirectory(sysPath) ) {
    if ( m_bLogContent )
      wApp->log("notice") << "WsContent::selectWidget : render a directory : " << sysPath;
    viewDirectory(path);
    return;
  }
  /*
  Koen : mmm this is a tricky problem. you could do this using an internal path so that you can first let the user authenticate, if necessary, and then you redirect to a static resource with some randomly generated token in the URL so that you can serve the file
  so the permalink is clean, like /cms/folder1/folder2/file.doc
  if your app is deployed at / or /cms, you handle the internal path, authenticate, etc...
  then finally, you create a random ID, and redirect to /resource/folder1/folder2/file.doc?auth=randomid
  and this is handled by a WResource which checks the path and serves the file
  if you prefer I can create a small example ...
  ooops ! must run !
  */
  if ( strExt == ".rss" ) {
    if ( m_bLogContent )
      wApp->log("notice") << "WsContent::selectWidget : render a " << strExt << " file : " << sysPath;
    clear();
    WsApp->hideImages(false);
    addWidget(new WText("Download file : " + path + " ..."));
    return wApp->redirect(m_sRelativeDocumentRoot + path);
  }
  for (int iModule = 0; iModule < WsApp->WsModules().modules().size(); iModule++) {
    WsModule* curModule = WsApp->WsModules().modules()[iModule]->module;
    if ( curModule->isLoaded() ) continue;
    if ( (strExt.size() == 0  && strName == curModule->fileName()) || (strExt.size() > 0 && strExt == curModule->extension(strExt))  ) {
      if ( m_bLogContent )
        if ( strExt == curModule->extension(strExt) )
          wApp->log("notice") << "WsContent::selectWidget : module, render a " << strExt << " file extension : " << sysPath;
        else
          wApp->log("notice") << "WsContent::selectWidget : module, render " << strName << " file name: " << sysPath;
      clear();
      WsApp->hideImages(curModule->hideImages());
      curModule->setSysPath(sysPath);
      curModule->setDiffPath(m_sRelativeDocumentRoot);
      WWidget* w = curModule->createContents();
      if ( w ) {
        if ( asString(curModule->option("useLayout")) == "true" ) {
          WVBoxLayout* vbox = new WVBoxLayout();
          vbox->addWidget(w, 1);
          setLayout(vbox);
        } else {
          addWidget(w);
        }
      } else {
        wApp->log("notice") << "WsContent::selectWidget : module, render " << curModule->moduleName() << " CANNOT call create content: ";
      }
      return;
    }
  }

  if ( strExt == ".pdf" ) {
    if ( m_bLogContent )
      wApp->log("notice") << "WsContent::selectWidget : render a " << strExt << " file : " << sysPath;
    //     if ( !allowedPath(userUID, syspath) ) return;
    clear();
    WsApp->hideImages(false);
    addWidget(new WText("Download file : " + path + " ..."));
    return wApp->redirect(m_sRelativeDocumentRoot + path);
  }
  clear();
  addWidget(new WsErrorPage(WsErrorPage::Error, path, 0, "Unknown extension", false));
}

void WsContent::doSiteMapItemSelected(gdWFileView::signalType sigType, std::string selectedPath)
{
  std::string str = selectedPath;
  if ( sigType == gdWFileView::selected )
    boost::algorithm::replace_first(str, "/SiteMap", "");
  wApp->setInternalPath(str, true);
}

