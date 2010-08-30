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

#include "mlabelview.h"
#include "mlabelview_p.h"
#include "mlabelmodel.h"
#include "mlabel.h"
#include "mviewcreator.h"

#include <QTextDocument>
#include <QPixmapCache>
#include <QAbstractTextDocumentLayout>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneResizeEvent>

MLabelViewPrivate::MLabelViewPrivate()
{
    impl = new MLabelViewSimple(this);
}

MLabelViewPrivate::~MLabelViewPrivate()
{
    delete impl;
}

const MLabelModel *MLabelViewPrivate::model() const
{
    Q_Q(const MLabelView);
    return q->model();
}

const MLabelStyle *MLabelViewPrivate::style() const
{
    Q_Q(const MLabelView);
    return q->style().operator ->();
}

const QRectF MLabelViewPrivate::boundingRect() const
{
    Q_Q(const MLabelView);
    return q->boundingRect();
}

bool MLabelViewPrivate::displayAsRichText(QString text, Qt::TextFormat textFormat, int numberOfHighlighters) const
{
    if (textFormat == Qt::RichText) {
        return true;
    } else if (textFormat == Qt::PlainText) {
        return false;
    }

    //Qt::mightBeRichText stops at the first line break
    text.replace("\n", " ");
    
    return Qt::mightBeRichText(text) || (numberOfHighlighters > 0);
}

MLabelView::MLabelView(MLabel *controller) :
    MWidgetView(new MLabelViewPrivate)
{
    Q_D(MLabelView);
    d->controller = controller;
}

MLabelView::~MLabelView()
{
}

void MLabelView::applyStyle()
{
    MWidgetView::applyStyle();
    Q_D(MLabelView);
    d->impl->markDirty();
    d->impl->applyStyle();
    const MLabelStyle* labelStyle = d->style();
    d->paddedSize = size() - QSizeF(labelStyle->paddingLeft() + labelStyle->paddingRight(), labelStyle->paddingTop() + labelStyle->paddingBottom());
    updateGeometry();
}

void MLabelView::drawContents(QPainter *painter, const QStyleOptionGraphicsItem *option) const
{
    Q_D(const MLabelView);
    Q_UNUSED(option);

    //Opacity for the label
    qreal oldOpacity = painter->opacity();
    const MLabelStyle* labelStyle = d->style();
    if (labelStyle->textOpacity() >= 0.0)
        painter->setOpacity(d->controller->effectiveOpacity() * labelStyle->textOpacity());

    //give size adjusted with padding to the actual implementation class
    d->impl->drawContents(painter, d->paddedSize);

    painter->setOpacity(oldOpacity);
}

void MLabelView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    MWidgetView::resizeEvent(event);

    Q_D(MLabelView);
    d->impl->markDirty();
    const MLabelStyle* labelStyle = d->style();
    QSizeF padding(labelStyle->paddingLeft() + labelStyle->paddingRight(),
                   labelStyle->paddingTop() + labelStyle->paddingBottom());
    d->paddedSize = event->newSize() - QSizeF(labelStyle->paddingLeft() + labelStyle->paddingRight(), labelStyle->paddingTop() + labelStyle->paddingBottom());

    event->setOldSize(event->oldSize() - padding);
    event->setNewSize(d->paddedSize);

    d->impl->resizeEvent(event);
}

QFont MLabelView::font() const
{
    return style()->font();
}

QSizeF MLabelView::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_D(const MLabelView);

    QSizeF padding(style()->paddingLeft() + style()->paddingRight(), style()->paddingTop() + style()->paddingBottom());
    return d->impl->sizeHint(which, constraint - padding) + padding;
}

void MLabelView::setupModel()
{
    MWidgetView::setupModel();
    Q_D(MLabelView);

    bool shouldBeRich = d->displayAsRichText(model()->text(), model()->textFormat(), model()->highlighters().size());

    // Check has label type changed since last call to this method. Re-allocate label with correct type.
    if (d->impl->isRich() != shouldBeRich) {
        MLabelViewSimple* oldView = d->impl;
        if (shouldBeRich)
            d->impl = new MLabelViewRich(d);
        else
            d->impl = new MLabelViewSimple(d);
        delete oldView;
    }

    d->impl->setupModel();

    d->impl->markDirty();
}

void MLabelView::updateData(const QList<const char *>& modifications)
{

    MWidgetView::updateData(modifications);

    Q_D(MLabelView);

    if (modifications.contains(MLabelModel::Text) || modifications.contains(MLabelModel::Highlighters) ||
        modifications.contains(MLabelModel::TextFormat)) {
        // Check has label type changed since last call to this method. Re-allocate label with correct type.

        bool shouldBeRich = d->displayAsRichText(model()->text(), model()->textFormat(), model()->highlighters().size());
        bool shouldBeSimple = !shouldBeRich;

        if ((shouldBeRich && !d->impl->isRich()) || (shouldBeSimple && d->impl->isRich())) {
            MLabelViewSimple* oldView = d->impl;
            if (shouldBeRich)
                d->impl = new MLabelViewRich(d);
            else
                d->impl = new MLabelViewSimple(d);

            delete oldView;
            d->impl->setupModel();
        }
    }

    if (d->impl->updateData(modifications))
        updateGeometry();
    d->impl->markDirty();
    update();
}

void MLabelView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(MLabelView);
    d->impl->mousePressEvent(event);
}

void MLabelView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(MLabelView);
    d->impl->mouseReleaseEvent(event);
}

void MLabelView::cancelEvent(MCancelEvent *event)
{
    Q_D(MLabelView);
    d->impl->cancelEvent(event);
}

void MLabelView::longPressEvent(QGraphicsSceneContextMenuEvent *event)
{
    event->ignore();
}

void MLabelView::tapAndHoldGestureEvent(QGestureEvent *event, QTapAndHoldGesture* gesture)
{
    Q_D(MLabelView);
    d->impl->longPressEvent(event,gesture);
}

void MLabelView::orientationChangeEvent(MOrientationChangeEvent *event)
{
    Q_D(MLabelView);
    MWidgetView::orientationChangeEvent(event);
    d->impl->orientationChangeEvent(event);
}

M_REGISTER_VIEW_NEW(MLabelView, MLabel)
