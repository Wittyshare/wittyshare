/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
*/

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/convenience.hpp>

#include <Wt/WLogger>
#include <Wt/WContainerWidget>
#include <Wt/WEnvironment>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WOverlayLoadingIndicator>
#include <Wt/WTheme>

#include "WsApplication.h"
#include "Main/WsFunctionnalities.h"

#include <gdcore/gdCore.h>


#include "Main/WsTemplate.h"
#include "Main/WsTopBanner.h"
#include "Main/WsBottomBanner.h"
#include "Main/WsFunctionnalities.h"
#include "WsErrorPage.h"
#include <ConfigManager/WsLayoutProperties.h>
#include <Utils/WsUtils.h>

using namespace Wt;

WsApplication::WsApplication(const WEnvironment& env)
  : gdApplication(env), m_pUser(0), m_pButtonsBar(0), m_pContent(0), m_pImages(0)
{
  std::string sUid = gdWApp->getUserID();
  std::string sPwd;
  wApp->log("notice") << "WsApplication::WsApplication - Start load user with uid : " << sUid;
  std::string cip = environment().clientAddress();
  wApp->log("notice") << "WsApplication::WsApplication - Start load user with client IP : " << cip << " env ip " << environment().clientAddress();
  // TODO : Provisoire, si on est sur le reseau local on authentifie via ldap, sinon on considère que on est guest
  bool isPublicSite = WsGlobalProperties::instance()->get("global", "public_site", "false") == "true" ? true : false;
  // On recherche le theme par dans la configuration, si rien n'est configuré on prend le theme polised par defaut.
  wApp->setCssTheme(WsLayoutProperties::instance()->get("global", "css_theme", "polished"));
  wApp->useStyleSheet(wApp->theme()->resourcesUrl() + "wittyshare/Css/wittyshare.css");
  if (boost::filesystem::exists(wApp->docRoot() + "/wt/resources/js/functions.js"))
    wApp->require(wApp->resourcesUrl() + "js/functions.js");
  if ( isPublicSite ) {
    bool enableLoginWindow = WsGlobalProperties::instance()->get("global", "login_window", "true") == "true" ? true : false;
    if (enableLoginWindow) {
      if (WsUtils::ipValid(cip)) { //IP is 192.168.*.* or 127.0.*.*
        m_logon = new gdWLogon(sUid, sPwd, root());
        m_logon->logonValidated().connect(SLOT(this, WsApplication::doEndDialogLogon));
        m_logon->load();
      } else
        doEndDialogLogon("guest", "");
    } else
      doEndDialogLogon("guest", "");
  } else // TODO : Improve the solution
    doEndDialogLogon("", "");
}

void WsApplication::notify(const Wt::WEvent& event)
{
  try {
    WApplication::notify(event);
  } catch (std::exception& e) {
    wApp->log("ERROR") << "WsApplication::notify() : Exception caught" ;
  }
}

void WsApplication::doEndDialogLogon(std::string sUid, std::string pPassword)
{
  m_pUser = new WsUser(sUid, pPassword, environment().clientAddress());
  if ( !m_pUser || m_pUser->load() == ErrorCode::Failure ) {
    wApp->log("ERROR") << "WsApplication::doEndDialogLogon - Cannot load user with uid : " << sUid;
    if ( m_pUser )
      delete m_pUser;
    m_logon->setError("Invalid username or password");
    m_pUser = 0;
    return;
  }
  googleAnalyticsLogger("/");
  root()->addStyleClass("wsMainWin");
  // Chargement du fichier de resources .xml
  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->resourcesUrl() + "wittyshare/Transl/wittyshare");
  // Chargement du fichier de configuration .xml
  wApp->messageResourceBundle().use(wApp->docRoot() + wApp->theme()->resourcesUrl() + "wittyshare/Conf/wittyshare_config");
  wApp->setTitle(WsLayoutProperties::instance()->get("global", "win_title", ""));
  WApplication::instance()->setLoadingIndicator(new WOverlayLoadingIndicator());
  m_ml.load(); // Load all modules
  std::string    oldInternalPath = wApp->internalPath();
  boost::algorithm::replace_all(oldInternalPath, "&amp;",  "&");
  oldInternalPath = m_ml.checkPath(oldInternalPath);
  std::string truePath(WsModules().pathWithoutPrefix(oldInternalPath));
  if ( oldInternalPath != "/" && oldInternalPath.size() > 0 ) {
    // TODO : Boucler par module
    int perms = m_pUser->getPermissions(truePath);
    if ( perms == ErrorCode::NotLogged ) {
      root()->clear();
      wApp->root()->addWidget(new WsErrorPage(WsErrorPage::UnAuthorized, truePath, m_pUser, "", true));
      return;
    }
    if ( perms == ErrorCode::NotFound) {
      root()->clear();
      wApp->root()->addWidget(new WsErrorPage(WsErrorPage::NotFound, truePath, m_pUser, "", true));
      return;
    }
    if ( perms != GlobalConfig::Read && perms != GlobalConfig::ReadWrite)  {
      root()->clear();
      wApp->root()->addWidget(new WsErrorPage(WsErrorPage::Forbidden, truePath, m_pUser, "", true));
      return;
    }
  } else {
    oldInternalPath = ""; // reset sais + pq
  }
  m_sHomePage = m_pUser->getHomePage();
  if ( WsLayoutProperties::instance()->get("global", "use_template", "true") == "true") {
    if ( oldInternalPath.size() > 1 ) {
      setTemplate(oldInternalPath);
    } else {
      setTemplate(m_sHomePage);
    }
  } else {
    WVBoxLayout* vbox = new WVBoxLayout();
    vbox->setContentsMargins(0, 0, 0, 0);
    WsContent* pContent = new WsContent();
    m_pContent = pContent;
    if ( oldInternalPath.size() > 0 )
      m_pContent->setPath(oldInternalPath);
    else
      m_pContent->setPath(m_sHomePage);
    if ( WsLayoutProperties::instance()->get("global", "use_topbanner", "true") == "true" ) {
      WsTopBanner* pTB = new WsTopBanner();
      vbox->addWidget(pTB, 0);
    }
    /*
         if ( WsLayoutProperties::instance()->get("global", "use_menubar", "true") == "true" )
           {
            LOG(DEBUG)<<"WsApplication :: Use menu bar";
            WsMenuBar* pBar = new WsMenuBar();
            vbox->addWidget(pBar, 0);
           }
    */
    if ( WsLayoutProperties::instance()->get("global", "use_fonctionnalities", "true") == "true" ) {
      vbox->addWidget(new WsFunctionnalities());
    }
    if (WsLayoutProperties::instance()->get("global", "use_ws_images", "false") == "true") {
      m_pImages = WsApp->WsModules().module("WsModImages2")->createContents();
      WHBoxLayout* hbox = new WHBoxLayout();
      hbox->addWidget(m_pImages, 0);
      hbox->addWidget(pContent, 1);
      vbox->addLayout(hbox, 1);
    } else
      vbox->addWidget(pContent, 1);
    if (WsLayoutProperties::instance()->get("global", "use_bottombanner", "true") == "true") {
      vbox->addWidget(new WsBottomBanner(WsLayoutProperties::instance()->get("global", "bottombanner_text", "")), 0);
    }
    root()->setLayout(vbox);
  }
  wApp->internalPathChanged().connect(SLOT(this, WsApplication::doPathChanged));
  wApp->internalPathChanged().connect(SLOT(this, WsApplication::googleAnalyticsLogger));
}

WsApplication::~WsApplication()
{
  if ( m_pUser )
    delete m_pUser;
  m_pUser = 0;
}


void WsApplication::doPathChanged(std::string newPath)
{
  setTemplate(newPath);
}

void WsApplication::hideImages(bool bHide)
{
  if ( m_pImages ) m_pImages->setHidden(bHide);
}

WsUser* WsApplication::wsUser()
{
  return m_pUser;
}

WsModulesLoader& WsApplication::WsModules()
{
  return m_ml;
}

void WsApplication::setTemplate(const std::string& sPageOrig)
{
  std::string sPage = sPageOrig;
  boost::algorithm::replace_all(sPage, "&amp;",  "&");
  if ( WsLayoutProperties::instance()->get("global", "use_template", "true") == "true" ) {
    std::string     truePath(WsModules().pathWithoutPrefix(sPage));
    NodePtr rootNode = m_pUser->getAccessRoot();
    if ( !rootNode.get() ) {
      root()->addWidget(new WsErrorPage(WsErrorPage::Error, sPage, m_pUser, "WsApplication::setTemplate() root node is null"));
      return;
    }
    NodePtr curNode = rootNode.get()->eatPath(truePath);
    if ( !curNode.get() ) {
      boost::filesystem::path p(truePath);
      truePath = p.parent_path().string();
      curNode = rootNode.get()->eatPath(truePath);
    }
    // Boucle sur parent, jusqu'a obtention du template
    std::string sTemplate;
    std::string sPath;
    NodePtr pNode = curNode;
    while (pNode.get()) {
      sTemplate = pNode.get()->getProperties().get()->get("global", "template", "");
      if ( sTemplate.size() > 0 ) break;
      pNode = pNode.get()->getParent();
    }
    if ( sTemplate.size() < 1 ) sTemplate = "root.tpl"; // ce template doit exister
    if ( sPage.compare(0,  5, "/Edit" ) == 0 )    sTemplate = "stdEditPage.tpl"; // cui-ci aussi
    if ( sPage.compare(0,  8, "/SiteMap" ) == 0 ) sTemplate = "stdContact.tpl"; // cui-ci aussi
    if ( sPage.compare(0,  5, "/Logo" ) == 0 )    sTemplate = "GhFullWidth.tpl"; // cui-ci aussi
    if ( sPage.compare(0,  7, "/Search" ) == 0 )  sTemplate = "stdContact.tpl"; // cui-ci aussi
    if ( pNode.get() ) {
      sPath = sPage;
    } else {
      sPath = m_sHomePage;
    }

    std::string initPage = curNode.get()->getProperties().get()->get("global", "initial_page", "");
    if(initPage.size() > 0){
        NodePtr initNode = rootNode.get()->eatPath(truePath+"/"+initPage);
        if(initNode.get()){
            sTemplate = initNode.get()->getProperties().get()->get("global", "template", sTemplate);
        }
    }


    if ( sTemplate != m_sTemplate ) {
      m_sTemplate = sTemplate;
      root()->clear();
      m_pButtonsBar = 0;
      m_pContent = 0; // le template doit prevoir la creation du content
      m_pImages = 0;
      WsTemplate* pTemplate = new WsTemplate(sTemplate, sPath);
      bool bUseLayout = WsLayoutProperties::instance()->get("global", "useLayout", "false") == "true" ? true : false;
      if ( bUseLayout ) {
        WVBoxLayout* vbox = new WVBoxLayout();
        vbox->setContentsMargins(0, 0, 0, 0);
        vbox->addWidget(pTemplate, 1);
        root()->setLayout(vbox);
      } else {
        root()->addWidget(pTemplate);
      }
      return;
    }
  }
  if ( m_pContent )
    m_pContent->setPath(sPage);
}

void WsApplication::setContent(WsContent* pContent)
{
  //FIXME double free here
  //if ( m_pContent )
  //  delete m_pContent;
  m_pContent = pContent;
  m_pContent->setMaximumSize(WLength::Auto, WLength(10000));
}

void WsApplication::setContentButtonsBar(WsContentButtonsBar* pButtonsBar)
{
  m_pButtonsBar = pButtonsBar;
}

WsContentButtonsBar* WsApplication::contentButtonsBar()
{
  return m_pButtonsBar;
}

const std::string& WsApplication::homePage()
{
  return m_sHomePage;
}

void WsApplication::googleAnalyticsLogger(std::string newPath)
{
  bool enableAnalytics = WsGlobalProperties::instance()->get("analytics", "enable", "false") == "true" ? true : false;
  if (!enableAnalytics) return ;
  std::string UA = WsGlobalProperties::instance()->get("analytics", "UA", "") ;
  std::string domain = WsGlobalProperties::instance()->get("analytics", "domain", "none") ;
  std::string googleCmd = "(function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){\
                             (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),\
                             m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)\
                             })(window,document,'script','//www.google-analytics.com/analytics.js','ga');\
                             ga('create', '" + UA + "', '" + domain + "');\
                             ga('send', 'pageview');";
  doJavaScript(googleCmd);
}
