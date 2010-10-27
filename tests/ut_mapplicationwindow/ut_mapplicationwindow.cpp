/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libmeegotouch.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "ut_mapplicationwindow.h"

#include <mapplication.h>
#include <mapplication_p.h>
#include <mapplicationservice.h>
#include <mapplicationwindow.h>
#include <mapplicationwindow_p.h>
#include <mapplicationpage.h>
#include <minputmethodstate.h>
#include <MComponentData>
#include <MScene>
#include <MSceneManager>
#include <MToolBar>
#include <MNavigationBar>
#include <MApplicationMenu>

#include <MSceneWindow>
#include <MHomeButtonPanel>

#include <QSignalSpy>
#include <QEvent>

// Test class implementation

void Ut_MApplicationWindow::initTestCase()
{
    if(MComponentData::instance() == 0) {
        static int argc = 2;
        static char *argv[ 2 ];
        argv[ 0 ] = (char*)"./ut_mscenewindow";
        argv[ 1 ] = (char*)"-local-theme";
        m_componentData = new MComponentData(argc, argv);
    }
    qRegisterMetaType<MApplicationPage *>("MApplicationPage*");
    qRegisterMetaType< QList<StatusBarTestOperation> >("QList<StatusBarTestOperation>");

    MTheme::loadCSS(qApp->applicationDirPath() + "/ut_mapplicationwindow.css");
}

void Ut_MApplicationWindow::cleanupTestCase()
{
}

void Ut_MApplicationWindow::init()
{
    m_subject = new MApplicationWindow;
}

void Ut_MApplicationWindow::cleanup()
{
    delete m_subject;
    m_subject = 0;
}

void Ut_MApplicationWindow::testConstructorWithoutScene()
{
    QVERIFY(m_subject->scene());
}

void Ut_MApplicationWindow::testConstructorWithScene()
{
    MScene *scene = new MScene;
    MApplicationWindow *window = new MApplicationWindow(scene);
    QCOMPARE((quintptr) window->scene(), (quintptr) scene);
    delete window;
}

void Ut_MApplicationWindow::testIsOnDisplay()
{
    m_subject = MApplication::activeApplicationWindow();

    QPointer<MApplicationPage> page1 = new MApplicationPage;
    QPointer<MApplicationPage> page2 = new MApplicationPage;

    QVERIFY(page1->isOnDisplay() == false);
    QVERIFY(page2->isOnDisplay() == false);

    // Window is not visible => page should not become visible
    m_subject->sceneManager()->appearSceneWindowNow(page2);
    QVERIFY(page1->isOnDisplay() == false);
    QVERIFY(page2->isOnDisplay() == false);

    // To be revisited: how these could be unit-tested
    // (event->viewRect().intersects(sceneBoundingRect())
    // does not return meaningful values in this.

    page1->disappear();
    page1->disappear();
    QVERIFY(page1->isOnDisplay() == false);
    QVERIFY(page2->isOnDisplay() == false);
}

void Ut_MApplicationWindow::testPrestartNoPrestart()
{
    MApplication::setPrestartMode(M::NoPrestart);
    m_subject->show();
    QCOMPARE(m_subject->isVisible(), true);
    m_subject->close();
    QCOMPARE(m_subject->isVisible(), false);
}

void Ut_MApplicationWindow::testPrestartTerminateOnClose()
{
    MApplication::setPrestartMode(M::TerminateOnClose);
    m_subject->show();
    QCOMPARE(m_subject->isVisible(), true);
    m_subject->close();
    QCOMPARE(m_subject->isVisible(), false);
}

void Ut_MApplicationWindow::testPrestartLazyShutdown()
{
    MApplication::setPrestartMode(M::LazyShutdown);
    m_subject->show();
    QCOMPARE(m_subject->isVisible(), true);
    m_subject->close();
    QCOMPARE(m_subject->isVisible(), false);
    QCOMPARE(m_subject->isHidden(), true);
}

void Ut_MApplicationWindow::testWindowActivate()
{
    QVERIFY(MApplication::activeApplicationWindow() == m_subject);

    MApplicationWindow *appWin = new MApplicationWindow;
    QVERIFY(MApplication::activeApplicationWindow() == m_subject);

    QEvent activate(QEvent::WindowActivate);

    qApp->sendEvent(appWin, &activate);
    QVERIFY(MApplication::activeApplicationWindow() == appWin);

    delete appWin;
    QVERIFY(MApplication::activeApplicationWindow() == m_subject);
}

void Ut_MApplicationWindow::testPageChanged()
{
    QSignalSpy spy(m_subject, SIGNAL(pageChanged(MApplicationPage *)));
    MApplicationPage *page = new MApplicationPage;
    MApplicationPage *page2 = new MApplicationPage;
    m_subject->sceneManager()->appearSceneWindowNow(page);
    m_subject->sceneManager()->appearSceneWindowNow(page2);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(0).at(0).value<MApplicationPage *>(), page);
    QCOMPARE(spy.at(1).at(0).value<MApplicationPage *>(), page2);

    delete page;
    delete page2;
}

void Ut_MApplicationWindow::testDeleteOnClose()
{
    qDebug() << "if the test crashes here, "
             << "Qt::WA_DeleteOnClose is broken for MApplicationWindow";
    MApplicationWindow *win = new MApplicationWindow;
    win->setAttribute(Qt::WA_DeleteOnClose, true);
    win->show();
    win->close();
}

void Ut_MApplicationWindow::testSetWindowIconID()
{
    QString iconId("icon-id");

    m_subject->setWindowIconID(iconId);
    QCOMPARE(m_subject->d_func()->navigationBar->viewMenuIconID(), iconId);
}

void Ut_MApplicationWindow::testCurrentPage()
{
    MApplicationPage *page = new MApplicationPage;

    QCOMPARE(m_subject->currentPage(), (MApplicationPage*) 0);
    m_subject->sceneManager()->appearSceneWindowNow(page);
    QCOMPARE(m_subject->currentPage(), page);
    delete page;
}

void Ut_MApplicationWindow::testMenu()
{
    m_subject->d_func()->navigationBar->setArrowIconVisible(true);

    QCOMPARE(m_subject->isMenuOpen(), false);
    m_subject->openMenu();
    QCOMPARE(m_subject->isMenuOpen(), true);
    m_subject->closeMenu();
    QCOMPARE(m_subject->isMenuOpen(), false);
}

void Ut_MApplicationWindow::testIsMenuOpen()
{
    QCOMPARE(m_subject->isMenuOpen(), false);
    m_subject->sceneManager()->appearSceneWindowNow(m_subject->d_func()->menu);
    QCOMPARE(m_subject->isMenuOpen(), true);
    m_subject->sceneManager()->disappearSceneWindowNow(m_subject->d_func()->menu);
    QCOMPARE(m_subject->isMenuOpen(), false);
}

void Ut_MApplicationWindow::testDisplayExitedOnClose()
{
    m_subject->show();
    QSignalSpy spy(m_subject, SIGNAL(displayExited()));
    m_subject->close();
    QCOMPARE(spy.count(), 1);
}

void Ut_MApplicationWindow::testDisplayExitedOnCloseLazyShutdownApp()
{
    MApplication::setPrestartMode(M::LazyShutdown);
    m_subject->show();
    QSignalSpy spy(m_subject, SIGNAL(displayExited()));
    m_subject->close();
    QCOMPARE(spy.count(), 1);
}

void Ut_MApplicationWindow::testPageEscape()
{
    MApplicationPage *page = new MApplicationPage;
    MNavigationBar *navigationBar = m_subject->d_func()->navigationBar;

    page->setEscapeMode(MApplicationPageModel::EscapeManualBack);
    QCOMPARE(navigationBar->escapeButtonMode(), MNavigationBarModel::EscapeButtonClose);

    page->appear(m_subject);
    QCOMPARE(navigationBar->escapeButtonMode(), MNavigationBarModel::EscapeButtonBack);

    page->setEscapeMode(MApplicationPageModel::EscapeCloseWindow);
    QCOMPARE(navigationBar->escapeButtonMode(), MNavigationBarModel::EscapeButtonClose);

    delete page;
}

void Ut_MApplicationWindow::testPageEscapeAuto()
{
    MApplicationPage *firstPage = new MApplicationPage;
    MApplicationPage *secondPage = new MApplicationPage;
    MNavigationBar *navigationBar = m_subject->d_func()->navigationBar;

    QVERIFY(navigationBar != 0);

    m_subject->sceneManager()->appearSceneWindowNow(firstPage);

    QCOMPARE(navigationBar->escapeButtonMode(), MNavigationBarModel::EscapeButtonClose);

    m_subject->sceneManager()->appearSceneWindowNow(secondPage);

    QCOMPARE(navigationBar->escapeButtonMode(), MNavigationBarModel::EscapeButtonBack);

    m_subject->sceneManager()->dismissSceneWindowNow(secondPage);

    QCOMPARE(navigationBar->escapeButtonMode(), MNavigationBarModel::EscapeButtonClose);
}

void Ut_MApplicationWindow::testPageEscapeAutoWhenAddingPageHistory()
{
    MApplicationPage *firstPage = new MApplicationPage;
    MApplicationPage *secondPage = new MApplicationPage;
    MNavigationBar *navigationBar = fetchNavigationBar(m_subject->scene()->items());
    QList<MSceneWindow *> pageHistory;

    QVERIFY(navigationBar != 0);

    m_subject->sceneManager()->appearSceneWindowNow(secondPage);

    QCOMPARE(navigationBar->escapeButtonMode(), MNavigationBarModel::EscapeButtonClose);

    pageHistory.append(firstPage);
    m_subject->sceneManager()->setPageHistory(pageHistory);

    QCOMPARE(navigationBar->escapeButtonMode(), MNavigationBarModel::EscapeButtonBack);
}

void Ut_MApplicationWindow::testPageEscapeAutoWhenClearingPageHistory()
{
    MApplicationPage *firstPage = new MApplicationPage;
    MApplicationPage *secondPage = new MApplicationPage;
    MNavigationBar *navigationBar = fetchNavigationBar(m_subject->scene()->items());
    QList<MSceneWindow *> pageHistory;

    QVERIFY(navigationBar != 0);

    m_subject->sceneManager()->appearSceneWindowNow(firstPage);
    m_subject->sceneManager()->appearSceneWindowNow(secondPage);

    QCOMPARE(navigationBar->escapeButtonMode(), MNavigationBarModel::EscapeButtonBack);

    m_subject->sceneManager()->setPageHistory(pageHistory);

    QCOMPARE(navigationBar->escapeButtonMode(), MNavigationBarModel::EscapeButtonClose);
}

void Ut_MApplicationWindow::testComponentsDisplayMode()
{
    MApplicationPage *page = new MApplicationPage;
    page->appear(m_subject);

    MSceneWindow *homeButtonPanel = m_subject->d_func()->homeButtonPanel;
    MSceneWindow *navigationBar = m_subject->d_func()->navigationBar;

    QCOMPARE(homeButtonPanel->sceneWindowState(), MSceneWindow::Appeared);
    QCOMPARE(navigationBar->sceneWindowState(), MSceneWindow::Appeared);

    page->setComponentsDisplayMode(MApplicationPage::AllComponents, MApplicationPageModel::Hide);

    QCOMPARE(homeButtonPanel->sceneWindowState(), MSceneWindow::Disappeared);
    QCOMPARE(navigationBar->sceneWindowState(), MSceneWindow::Disappeared);

    page->setComponentsDisplayMode(MApplicationPage::HomeButton, MApplicationPageModel::Show);

    QCOMPARE(homeButtonPanel->sceneWindowState(), MSceneWindow::Appeared);
    QCOMPARE(navigationBar->sceneWindowState(), MSceneWindow::Disappeared);

    page->setComponentsDisplayMode(MApplicationPage::HomeButton, MApplicationPageModel::AutoHide);

    QCOMPARE(homeButtonPanel->sceneWindowState(), MSceneWindow::Disappeared);
    QCOMPARE(navigationBar->sceneWindowState(), MSceneWindow::Disappeared);

    page->setComponentsDisplayMode(MApplicationPage::AllComponents, MApplicationPageModel::Show);

    QCOMPARE(homeButtonPanel->sceneWindowState(), MSceneWindow::Appeared);
    QCOMPARE(navigationBar->sceneWindowState(), MSceneWindow::Appeared);
}

void Ut_MApplicationWindow::testStatusBarVisibility_data()
{
    QTest::addColumn< QList<StatusBarTestOperation> >("operations");
    QTest::addColumn<bool>("expectedVisibility");  // true = visible, false = invisible

    typedef QList<StatusBarTestOperation> OpList;

    QTest::newRow("Fullscreen OFF") << (OpList() << MakeNormal) << true;
    QTest::newRow("Fullscreen ON") << (OpList() << MakeFullScreen) << false;
    QTest::newRow("Maximize page area") << (OpList() << MakeNormal << MaximizePageArea) << false;
    QTest::newRow("Restore page area") << (OpList() << MakeNormal << RestorePageArea) << true;
    QTest::newRow("Restore page area while fullscreen1") << (OpList() << MakeFullScreen << RestorePageArea) << false;
    QTest::newRow("Restore page area while fullscreen2") << (OpList() << MakeFullScreen << RestorePageArea << MakeNormal) << true;
    QTest::newRow("Maximize page area while fullscreen1") << (OpList() << MakeFullScreen << MaximizePageArea) << false;
    QTest::newRow("Maximize page area while fullscreen2") << (OpList() << MakeFullScreen << MaximizePageArea << MakeNormal) << false;
}

void Ut_MApplicationWindow::testStatusBarVisibility()
{
    QFETCH(QList<StatusBarTestOperation>, operations);
    QFETCH(bool, expectedVisibility);

    const MSceneWindow *statusBar = 0;

    statusBar = m_subject->d_func()->statusBar;

    if (!statusBar) {
        QSKIP("No status bar used so skipping test.", SkipSingle);
    }

    m_subject->sceneManager()->setOrientationAngle(M::Angle0, MSceneManager::ImmediateTransition);
    QVERIFY(m_subject->orientationAngle() == M::Angle0);

    foreach (StatusBarTestOperation op, operations) {
        switch (op) {
        case MakeFullScreen:
            m_subject->showFullScreen();
            break;
        case MakeNormal:
            m_subject->showNormal();
            break;
        case MaximizePageArea:
            // Status bar hides because along with other decorations.
            m_subject->d_func()->pageAreaMaximized = true;
            QMetaObject::invokeMethod(m_subject, "_q_updateStatusBarVisibility", Qt::DirectConnection);
            break;
        case RestorePageArea:
            m_subject->d_func()->pageAreaMaximized = false;
            QMetaObject::invokeMethod(m_subject, "_q_updateStatusBarVisibility", Qt::DirectConnection);
            break;
        }
    }

    // This relies on status bar being out of display which means no animation
    // and change in QGraphicsItem visibility is immediate.
    QCOMPARE(statusBar->isVisible(), expectedVisibility);
}


void Ut_MApplicationWindow::initToolbarLocationTC(M::OrientationAngle angle, const QString &viewType)
{
    // set orientation
    m_subject->sceneManager()->setOrientationAngle(angle, MSceneManager::ImmediateTransition);

    // set toolbar type
    m_subject->setToolbarViewType(viewType);
}

bool Ut_MApplicationWindow::isToolBarFloating()
{
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents, 10);

    MNavigationBar *navBar = m_subject->d_func()->navigationBar;
    MNavigationBarModel *model = navBar->model();

    return (!model->toolBar() || model->toolBar() != m_subject->d_func()->toolBar);
}

void Ut_MApplicationWindow::testToolBarInLandscapeIsFloating()
{
    initToolbarLocationTC(M::Angle0, MToolBar::defaultType);
    QVERIFY(isToolBarFloating());
}

void Ut_MApplicationWindow::testToolBarInPortraiIsInNavigationBar()
{
    initToolbarLocationTC(M::Angle90, MToolBar::defaultType);
    QVERIFY(isToolBarFloating() == false);
}

void Ut_MApplicationWindow::testToolBarMovesFromFloatingToNavigationBarWhenRotatedToPortrait()
{
    initToolbarLocationTC(M::Angle0, MToolBar::defaultType);
    QVERIFY(isToolBarFloating());

    m_subject->sceneManager()->setOrientationAngle(M::Angle90, MSceneManager::ImmediateTransition);
    QVERIFY(isToolBarFloating() == false);
}

void Ut_MApplicationWindow::testTabBarInLandscapeIsInNavigationBar()
{
    initToolbarLocationTC(M::Angle0, MToolBar::tabType);
    QVERIFY(isToolBarFloating() == false);
}

void Ut_MApplicationWindow::testTabBarInPortraitIsFloating()
{
    initToolbarLocationTC(M::Angle90, MToolBar::tabType);
    QVERIFY(isToolBarFloating());
}

void Ut_MApplicationWindow::testTabBarMovesFromNavigationBarToFloatingWhenRotatedToPortrait()
{
    initToolbarLocationTC(M::Angle0, MToolBar::tabType);
    QVERIFY(isToolBarFloating() == false);

    m_subject->sceneManager()->setOrientationAngle(M::Angle90, MSceneManager::ImmediateTransition);
    QVERIFY(isToolBarFloating());
}

MNavigationBar *Ut_MApplicationWindow::fetchNavigationBar(
        const QList<QGraphicsItem *> &itemsList) const
{
    int i = 0;
    int itemsCount = itemsList.count();
    QGraphicsItem *item;
    QGraphicsWidget *widget;
    MNavigationBar *navigationBar = 0;

    while (navigationBar == 0 && i < itemsCount) {
        item = itemsList.at(i);
        if (item->isWidget()) {
            widget = static_cast<QGraphicsWidget *>(item);
            navigationBar = qobject_cast<MNavigationBar*>(widget);
        }

        ++i;
    }

    return navigationBar;
}

QTEST_MAIN(Ut_MApplicationWindow)
