/*
 *
 * Benoit Daccache
 * ben.daccache@gmail.com
*/

#include <Wt/WApplication>
#include <Wt/WPanel>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WTheme>
#include <Wt/WIntValidator>
#include <Wt/WRegExpValidator>

#include <gdcore/gdCore.h>

#include "WsApplication.h"
#include "WsErrorPage.h"
using namespace Wt;


WsErrorPage::WsErrorPage(ErrorCode errorCode, const std::string& path, WsUser* user, const std::string& msg)  : WContainerWidget(0) 
{
    std::string error = "";
    switch(errorCode){
        case Forbidden:
            addWidget(new WText("<h1>FORBIDDEN ACCESS!</h1>"));
            error = "The URL "+ path + " is not allowed for user " + user->getUid();
            break;
        case NotFound:
            addWidget(new WText("<h1>URL NOT FOUND!</h1>"));
            error = "The URL "+ path + " is not found" ;
            break;
        case Error:
            addWidget(new WText("<h1>ERROR!</h1>"));
            error = "Error while loading URL "+ path  ;
            break;
    };

    addWidget(new WText(error));
    addWidget(new WText(msg));
}

WsErrorPage::~WsErrorPage()
{ }


