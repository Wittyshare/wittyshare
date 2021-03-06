/*
 *
 *       Filename:  WsSearchRSS.cpp
 *
 *    Description:
 *
 *        Created:  04/18/2012 12:34:02 PM
 *
 *         Author: Benoit Daccache, ben.daccache@gmail.com
 *
 */

#include "WsSearchRSS.h"


WsSearchRSS::WsSearchRSS(NodePtr mt):
  WsTreeTraversal(mt)
{
}

vector<NodePtr> WsSearchRSS::getRSS(int nb)
{
  m_maxSize = nb;
  start();
  return m_vect;
}

int WsSearchRSS::beginTraverseDir(NodePtr n)
{
  return ErrorCode::Success;
}

int WsSearchRSS::endTraverseDir(NodePtr n)
{
  return ErrorCode::Success;
}

int WsSearchRSS::traverseFile(NodePtr n)
{
  WsFileNode* fn = dynamic_cast<WsFileNode*>(n.get());
  if ( fn->getProperties().get()->get("global", "in_view", "true") == "false"   ) return ErrorCode::Success;
  // TODO : put this next line as options
  std::string curPath = fn->getPath().string();
  bool bOk = false;
  if ( curPath.compare(0, 17, "/Issues&Positions")          == 0 ) bOk = true;
  if ( curPath.compare(0, 26, "/News&Media/Press releases") == 0 ) bOk = true;
  if ( !bOk ) return ErrorCode::Success;
  if ( !(
         fn->getPath().extension().string() == ".fhtml" ||
         fn->getPath().extension().string() == ".pdf"   ||
         fn->getPath().extension().string() == ".rpg"   ||
         fn->getPath().extension().string() == ".zip"
       ) )
    return ErrorCode::Success;
  /* Vect empty, add the element */
  if (m_vect.size() == 0) {
    m_vect.push_back(n);
    return ErrorCode::Success;
  }
  /* File is a latest news, add is at the right position */
  else {
    vector<NodePtr>::iterator it;
    for (it = m_vect.begin(); it != m_vect.end(); ++it) {
      if (fn->getPublishDate() > dynamic_cast<WsFileNode*>((*it).get())->getPublishDate()) {
        m_vect.insert(it, n);
        if (m_vect.size() > m_maxSize)
          m_vect.pop_back();
        return ErrorCode::Success;
      }
    }
    if (m_vect.size() < m_maxSize)
      m_vect.push_back(n);
    return ErrorCode::Success;
  }
}

