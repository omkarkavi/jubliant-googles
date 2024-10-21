#include "UpDownLeftRightIum.h"

UpDownLeftRightIum::UpDownLeftRightIum (QObject *parent) : QObject{ parent } {}

bool UpDownLeftRightIum::eventFilter (QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);
		//		qDebug() << "Key Pressed : " << keyEvent;
		switch (keyEvent->key())
		{
		case Qt::Key_Up:
		case Qt::Key_Down:
		case Qt::Key_Right:
		case Qt::Key_Left:
		case Qt::Key_Q:
		case Qt::Key_E:
		case Qt::Key_Space:
			emit keyPressed (keyEvent);
		default:
			break;
		}
	}
	else if (event->type() == QEvent::KeyRelease)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);
		//		qDebug() << "Key Released : " << keyEvent;
		switch (keyEvent->key())
		{
		case Qt::Key_Up:
		case Qt::Key_Down:
		case Qt::Key_Right:
		case Qt::Key_Left:
		case Qt::Key_Q:
		case Qt::Key_E:
		case Qt::Key_Space:
			emit keyReleased (keyEvent);
		default:
			break;
		}
	}
	else if (event->type() == QEvent::GraphicsSceneMousePress)
	{
		QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *> (event);
		{
			//			qDebug() << "MB Pressed" << mouseEvent;
			emit mouseButtonPressed (mouseEvent->button());
		}
	}
	else if (event->type() == QEvent::GraphicsSceneMouseRelease)
	{
		QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *> (event);
		{
			//			qDebug() << "MB Released" << mouseEvent;
			emit mouseButtonReleased (mouseEvent->button());
		}
	}
	else if (event->type() == QEvent::GraphicsSceneMouseMove)
	{
		QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent *> (event);
		//				qDebug() << mouseEvent->scenePos();
		emit mouseMoved (mouseEvent->scenePos());
	}
	else
	{
		//		qDebug() << event;
	}

	return QObject::eventFilter (obj, event);
}
