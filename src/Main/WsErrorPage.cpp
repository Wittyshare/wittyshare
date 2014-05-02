/*
 *
 * Benoit Daccache
 * ben.daccache@gmail.com
*/

#include <Wt/WApplication>
#include <Wt/WPanel>
#include <Wt/WText>
#include <Wt/WTimer>

#include <gdcore/gdCore.h>

#include "WsApplication.h"
#include "WsErrorPage.h"
using namespace Wt;


WsErrorPage::WsErrorPage(ErrorCode errorCode, const std::string& path, WsUser* user, const std::string& msg, bool redirect)  : WContainerWidget(0)
{
  addStyleClass("WsErrorPage");
  std::string error = "";
  switch (errorCode) {
  case UnAuthorized:
    addWidget(new WText("<h1>Unauthorized ACCESS!</h1>"));
    error = "You need to be logged to access this page " + path;
    if (user != 0)
      error += user->getUid();
    break;
  case Forbidden:
    addWidget(new WText("<h1>FORBIDDEN ACCESS!</h1>"));
    error = "The URL " + path + " is not allowed for user ";
    if (user != 0)
      error += user->getUid();
    break;
  case NotFound:
    addWidget(new WText("<h1>URL NOT FOUND!</h1>"));
    error = "The URL " + path + " was not found on the server" ;
    break;
  case Error:
    addWidget(new WText("<h1>ERROR!</h1>"));
    error = "Error while loading URL " + path  ;
    break;
  };
  addWidget(new WText(error));
  addWidget(new WText(msg));
  if (redirect) {
    Wt::WTimer* timer = new Wt::WTimer();
    timer->setInterval(2000);
    timer->setSingleShot(true);
    timer->timeout().connect(this, &WsErrorPage::doRedirect);
    timer->start();
  }
}

void WsErrorPage::doRedirect()
{
  wApp->redirect("/");
}

WsErrorPage::~WsErrorPage()
{ }


