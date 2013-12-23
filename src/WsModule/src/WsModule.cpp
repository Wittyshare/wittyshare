/*
* Copyright (C) 2006-Today Guy Deleeuw
*
* See the LICENSE file for terms of use.
*/

#include <iostream>

#include <Wt/WLogger>

#include <gdcore/gdCore.h>

#include <Main/WsApplication.h>

#include "WsModule.h"



WsEditorWidget::WsEditorWidget(WContainerWidget* parent)
  : WContainerWidget(parent)
{ }

WsEditorWidget::~WsEditorWidget()
{ }

void WsEditorWidget::load()
{
  WContainerWidget::load();
}

bool WsEditorWidget::saveEditor()
{
  return true;
}







WsContentButtonsBar* WsModule::contentButtonsBar()
{
  WsApplication::wsInstance()->contentButtonsBar();
}

void WsModule::destroyContentsMenuBar()
{ }

void WsModule::setLoaded()
{
  m_bLoaded = true;
}

bool WsModule::isLoaded()
{
  return m_bLoaded;
}

