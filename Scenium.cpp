#include "Scenium.h"
#include <QDebug>

Scenium::Scenium (QObject *parent) : QGraphicsScene{ parent } {}

void Scenium::mouseMoveEvent (QGraphicsSceneMouseEvent *event)
{
	//	qDebug() << event->buttons() << event->scenePos();
	emit mouseMoved (event->scenePos());
	return QGraphicsScene::mouseMoveEvent (event);
}
