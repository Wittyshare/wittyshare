/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <stdio.h>

#include <Wt/WTheme>
#include <Wt/WLabel>
#include <Wt/WRegExpValidator>

#include "WsApplication.h"
#include "WsContentButtonsBar.h"

#include <ConfigManager/WsLayoutProperties.h>

using namespace Wt;

WsContentButtonsBar::WsContentButtonsBar(WContainerWidget* parent)
  : gdToolbar(parent), m_pDialog(0), m_pLE_Name(0), m_pButOk(0), m_bDebug(true)
{
  if ( WString::tr("byObjectStyleSheet").narrow() == "true" )
    wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "wittyshare/Css/WsContentButtonBar.css");
  addStyleClass("WsContentButtonsBar");
  WsApp->setContentButtonsBar(this);
  WsUser*           pUser   = WsApp->wsUser();
  gdToolbarItem*    pTbItem = 0;
  if ( pUser->isAdministrator() || pUser->isEditor() ) {
    pTbItem = addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/edit.png", "", "Edit current node !");
    pTbItem->setUrl("/Edit");
    pTbItem->clicked().connect(SLOT(this, WsContentButtonsBar::doMenuEditPage));
    pTbItem = addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder_add.png", "", "Create a new folder");
    pTbItem->clicked().connect(SLOT(this, WsContentButtonsBar::doFolderNew));
    pTbItem = addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/file_add.png", "", "Create a new file");
    pTbItem->clicked().connect(SLOT(this, WsContentButtonsBar::doFileNew));
    pTbItem = addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/folder_delete.png", "", "Delete current node");
    pTbItem->clicked().connect(SLOT(this, WsContentButtonsBar::doDeleteNode));
    pTbItem = addToolbarItem(wApp->theme()->resourcesUrl() + "gdwtcore/Icons/file_upload.png", "", "Upload a file");
    pTbItem->setUrl("/FileUpload");
    pTbItem->clicked().connect(SLOT(this, WsContentButtonsBar::doFileUpload));
  }
  for (int iModule = 0; iModule < WsApp->WsModules().modules().size(); ++iModule) {
    WsModule* curModule = WsApp->WsModules().modules()[iModule]->module;
    curModule->createContentsMenuBar(this);
  }
}

WsContentButtonsBar::~WsContentButtonsBar()
{
  // for (int iModule = 0; iModule < WsApp->WsModules().modules().size(); ++iModule)
  //  {
  //   WsModule* curModule = WsApp->WsModules().modules()[iModule]->module;
  //   curModule->destroyContentsMenuBar();
  //  }
}

void WsContentButtonsBar::doMenuEditPage(gdToolbarItem* pTbItem, WMouseEvent ev)
{
  std::string str =  pTbItem->url() + wApp->internalPath();
  std::string id = "";
  int ret = WsApp->wsUser()->isLocked(wApp->internalPath(), id);
  if (ret != 1) {
    WMessageBox::show("Warning", "The file is currently locked by \"" + id + "\". Please try again later.", Ok);
    return;
  }
  setNewInternalPath(pTbItem->url(),  WsApp->WsModules().pathWithoutPrefix(wApp->internalPath()), true);
}

void WsContentButtonsBar::doFolderNew(gdToolbarItem* pTbItem, WMouseEvent ev)
{
  m_pDialog = new WDialog("New Folder");
  m_pDialog->finished().connect(SLOT(this, WsContentButtonsBar::doEndFolderNew));
  Wt::WRegExpValidator* validator = new Wt::WRegExpValidator("[a-zA-Z0-9._& ]{2,48}");
  m_pLE_Name = new Wt::WLineEdit(m_pDialog->contents());
  m_pLE_Name->setValidator(validator);
  m_pLE_Name->keyWentUp().connect(SLOT(this, WsContentButtonsBar::doFolderNameChanged));
  m_pLE_Name->enterPressed().connect(SLOT(this, WsContentButtonsBar::doFolderEnterPressed));
  m_pLE_Name->setTextSize(16);
  m_pLE_Name->setFocus(true);
  WLabel* label = new Wt::WLabel(" : Name of the new Folder ", m_pDialog->contents());
  label->setBuddy(m_pLE_Name);
  m_pButOk = new Wt::WPushButton("OK", m_pDialog->footer());
  m_pButOk->clicked().connect(m_pDialog, &Wt::WDialog::accept);
  m_pButOk->disable();
  WPushButton* cancel = new Wt::WPushButton("Cancel", m_pDialog->footer());
  cancel->clicked().connect(m_pDialog, &Wt::WDialog::reject);
  m_pDialog->show();
}

void WsContentButtonsBar::doFileNew(gdToolbarItem* pTbItem, WMouseEvent ev)
{
  m_pDialog = new WDialog("New File");
  m_pDialog->finished().connect(SLOT(this, WsContentButtonsBar::doEndFileNew));
  Wt::WRegExpValidator* validator = new Wt::WRegExpValidator("[a-zA-Z0-9._& ]{2,48}");
  m_pLE_Name = new Wt::WLineEdit(m_pDialog->contents());
  m_pLE_Name->setValidator(validator);
  m_pLE_Name->keyWentUp().connect(SLOT(this, WsContentButtonsBar::doFileNameChanged));
  m_pLE_Name->enterPressed().connect(SLOT(this, WsContentButtonsBar::doFileEnterPressed));
  m_pLE_Name->setTextSize(16);
  m_pLE_Name->setFocus(true);
  WLabel* label = new Wt::WLabel(" : Name of the new File ", m_pDialog->contents());
  label->setBuddy(m_pLE_Name);
  m_pButOk = new Wt::WPushButton("OK", m_pDialog->footer());
  m_pButOk->clicked().connect(m_pDialog, &Wt::WDialog::accept);
  m_pButOk->disable();
  WPushButton* cancel = new Wt::WPushButton("Cancel", m_pDialog->footer());
  cancel->clicked().connect(m_pDialog, &Wt::WDialog::reject);
  m_pDialog->show();
}

void WsContentButtonsBar::doFolderEnterPressed()
{
  if ( m_pLE_Name->validate() )
    m_pDialog->accept();
}

void WsContentButtonsBar::doFileEnterPressed()
{
  if ( m_pLE_Name->validate() )
    m_pDialog->accept();
}

void WsContentButtonsBar::doFolderNameChanged()
{
  m_pButOk->setDisabled(m_pLE_Name->validate() != Wt::WValidator::Valid);
}

void WsContentButtonsBar::doFileNameChanged()
{
  m_pButOk->setDisabled(m_pLE_Name->validate() != Wt::WValidator::Valid);
}

void WsContentButtonsBar::doEndFolderNew()
{
  if ( m_pDialog->result() == Wt::WDialog::Accepted ) {
    std::string newName = m_pLE_Name->text().toUTF8();
    boost::algorithm::replace_all(newName, "&amp;",  "&");
    std::string currentPath = WsApp->WsModules().pathWithoutPrefix(wApp->internalPath());
    boost::algorithm::replace_all(currentPath, "&amp;",  "&");
    /* Remove useless / */
    if (currentPath == "/")
      currentPath = "";

    WsUser* pUser = WsApp->wsUser();
    //Test if path is a file or a directory
    NodePtr pNode =  pUser->getAccessRoot()->eatPath(currentPath);
    if(!pNode.get()) return; //TODO ?
    
    if(!pNode.get()->isDirectory())
        currentPath = path(currentPath).parent_path().string();

    std::string path2Create = currentPath + "/" + newName;
    // TODO : test internalPath is directory and if no, replace file name by newName
    pUser->createNode(path2Create, WsUser::Directory);
    setNewInternalPath("/Edit" , path2Create, true);
  }
  delete m_pDialog;
  m_pButOk   = 0;
  m_pLE_Name = 0;
  m_pDialog  = 0;
}

void WsContentButtonsBar::doEndFileNew()
{
  if ( m_pDialog->result() == Wt::WDialog::Accepted ) {
    std::string newName = m_pLE_Name->text().toUTF8();
    boost::algorithm::replace_all(newName, "&amp;",  "&");
    boost::filesystem::path pathName(newName);
    if ( pathName.extension().string() == "" ) newName += ".fhtml";
    std::string currentPath = WsApp->WsModules().pathWithoutPrefix(wApp->internalPath());
    boost::algorithm::replace_all(currentPath, "&amp;",  "&");
    WsUser* pUser = WsApp->wsUser();
    //Test if path is a file or a directory
    NodePtr pNode =  pUser->getAccessRoot()->eatPath(currentPath);
    if(!pNode.get()) return; //TODO ?
    
    if(!pNode.get()->isDirectory())
        currentPath = path(currentPath).parent_path().string();

    std::string path2Create = currentPath + "/" + newName;
    boost::algorithm::replace_all(path2Create, "//",  "/");
    // TODO : test internalPath is directory and if no, replace file name by newName
    pUser->createNode(path2Create, WsUser::File);
    setNewInternalPath("/Edit" , path2Create, true);
  }
  delete m_pDialog;
  m_pButOk   = 0;
  m_pLE_Name = 0;
  m_pDialog  = 0;
}

void WsContentButtonsBar::doDeleteNode(gdToolbarItem* pTbItem, Wt::WMouseEvent ev)
{
  WsUser* pUser = WsApp->wsUser();
  //std::string newPath = wApp->internalPath();
  std::string newPath = WsApp->WsModules().pathWithoutPrefix(wApp->internalPath());
  boost::algorithm::replace_all(newPath, "&amp;",  "&");
  WMessageBox* pBox = new WMessageBox("Confirm", "Delete : " + newPath, Wt::Information, Wt::Yes | Wt::No);
  pBox->buttonClicked().connect(boost::bind(&WsContentButtonsBar::doMBoxRespons, this, pBox));
  pBox->show();
}

void WsContentButtonsBar::doMBoxRespons(WMessageBox* pBox)
{
  int res = pBox->buttonResult();
  delete pBox;
  if ( res != Wt::Yes ) return;
  WsUser* pUser = WsApp->wsUser();
  //std::string newPath = wApp->internalPath();
  std::string newPath = WsApp->WsModules().pathWithoutPrefix(wApp->internalPath());
  boost::algorithm::replace_all(newPath, "&amp;",  "&");
  pUser->deleteNode(newPath);
  wApp->setInternalPath(path(newPath).parent_path().string(), true);
}

void WsContentButtonsBar::doFileUpload(gdToolbarItem* pTbItem, WMouseEvent ev)
{
  WsUser*           pUser                 = WsApp->wsUser();
  std::string       sDocumentRoot         = pUser->getRootPath(); // /var/www/demo_site
  std::string       fullPath              = sDocumentRoot + wApp->internalPath();
  std::string       currentPath           = wApp->internalPath();
  boost::algorithm::replace_all(fullPath, "&amp;",  "&");
    //Test if path is a file or a directory
  
  NodePtr pNode =  pUser->getAccessRoot()->eatPath(wApp->internalPath());
  if(!pNode.get()) return; //TODO ?
    
  if(!pNode.get()->isDirectory())
      currentPath = path(currentPath).parent_path().string();

  if ( pUser->isAdministrator() || pUser->isEditor() ) {
    setNewInternalPath(pTbItem->url() , currentPath, true);
  }
}

void WsContentButtonsBar::setNewInternalPath(std::string sub, std::string url, bool refresh)
{
  if (sub == wApp->internalPath()) {
    sub = WsApp->WsModules().pathWithoutPrefix(sub);
  } else {
    boost::algorithm::replace_all(url, sub, "");
  }
  std::string newPath = sub + url;
  wApp->setInternalPath(newPath, refresh);
}
