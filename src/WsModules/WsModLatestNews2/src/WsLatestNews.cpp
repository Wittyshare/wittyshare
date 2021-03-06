/*
 *
 *       Filename:  WsLatestNews.cpp
 *
 *    Description:
 *
 *        Created:  04/18/2012 12:34:02 PM
 *
 *         Author: Benoit Daccache, ben.daccache@gmail.com
 *
 */

#include "WsLatestNews.h"


WsLatestNews::WsLatestNews(NodePtr mt):
  WsTreeTraversal(mt)
{
}

vector<NodePtr> WsLatestNews::getLatestNews(int nb)
{
  m_maxSize = nb;
  start();
  return m_vect;
}

int WsLatestNews::beginTraverseDir(NodePtr n)
{
  return ErrorCode::Success;
}

int WsLatestNews::endTraverseDir(NodePtr n)
{
  return ErrorCode::Success;
}

int WsLatestNews::traverseFile(NodePtr n)
{
  WsFileNode* fn = dynamic_cast<WsFileNode*>(n.get());
  if ( fn->getProperties().get()->get("global", "in_view", "true") == "false"   ) return ErrorCode::Success;
  if ( !(
         fn->getPath().extension().string() == ".fhtml" ||
         fn->getPath().extension().string() == ".pdf"   ||
         fn->getPath().extension().string() == ".rpg"   ||
         fn->getPath().extension().string() == ".zip"
       ) )
    return ErrorCode::Success;
  boost::filesystem::path wsRes = fn->getPath();
  bool bRes = false;
  while (wsRes.string() != "/") {
    wsRes = wsRes.parent_path();
    if ( wsRes.filename().string() == "ws.res" ) {
      bRes = true;
      break;
    }
    if ( wsRes.filename().string() == "ws" )     {
      bRes = true;
      break;
    }
    continue;
  }
  if ( bRes ) return ErrorCode::Success;
  /* Vect empty, add the element */
  if (m_vect.size() == 0) {
    m_vect.push_back(n);
    return ErrorCode::Success;
  }
  /* File is a latest news, add is at the right position */
  else {
    vector<NodePtr>::iterator it;
    for (it = m_vect.begin(); it != m_vect.end(); ++it) {
      WsFileNode* temp = dynamic_cast<WsFileNode*>((*it).get());
      if (fn->getPublishDate() > temp->getPublishDate()) {
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

