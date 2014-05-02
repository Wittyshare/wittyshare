/*
 * Benoit Daccache
*/

#ifndef _WsErrorPage_H_
#define _WsErrorPage_H_


#include <Wt/WContainerWidget>
#include <User/WsUser.h>

class WsErrorPage : public Wt::WContainerWidget {
public :
  enum ErrorCode {
    UnAuthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    Error = 500
  };

  WsErrorPage(ErrorCode errorCode, const std::string& path, WsUser* user = 0, const std::string& msg = "", bool redirect = false);
  ~WsErrorPage();

private slots:
  void doRedirect();
};

#endif

