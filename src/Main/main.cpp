/*
 * Copyright (C) 2006-Today Guy Deleeuw
 *
 * See the LICENSE file for terms of use.
 */
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include <User/WsUser.h>
#include <Logger/WsCrashLogger.h>

#include "WsApplication.h"

#include <signal.h>



using namespace Wt;

WApplication* createApplication(void* pTree, const WEnvironment& env)
{
  return new WsApplication(env);
}


int main(int argc, char** argv)
{
  signal(SIGSEGV,  &WsCrashLogger::handleSignal);
  signal( SIGABRT, &WsCrashLogger::handleSignal);
  signal( SIGILL,  &WsCrashLogger::handleSignal);
  signal( SIGFPE,  &WsCrashLogger::handleSignal);
  boost::posix_time::ptime now;
  std::string d2out;
  now = boost::posix_time::microsec_clock::local_time(); // current *LOCAL TIMEZONE* time/date
  d2out = boost::posix_time::to_iso_extended_string(now);
  d2out[d2out.find('T')] = ' ';
  bool isServer = argc < 2 || strcmp(argv[1], "client") != 0;
  if ( isServer ) {
    std::cerr << "[" << d2out << "] [notice]" << " main - in server." << std::endl;
  } else {
    std::cerr << "[" << d2out << "] [notice]" << " main - in client." << std::endl;
  }
  int ret =  WRun(argc, argv, boost::bind(createApplication, (void*) 0, _1));
  return ret;
}

