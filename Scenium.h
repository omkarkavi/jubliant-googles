#ifndef SCENIUM_H
#define SCENIUM_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class Scenium : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit Scenium (QObject *parent = nullptr);

	void mouseMoveEvent (QGraphicsSceneMouseEvent *event);

signals:
	void mouseMoved (QPointF location);
};

#endif // SCENIUM_H
