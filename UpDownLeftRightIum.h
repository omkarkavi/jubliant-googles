#ifndef UPDOWNLEFTRIGHTIUM_H
#define UPDOWNLEFTRIGHTIUM_H

#include <QEvent>
#include <QKeyEvent>
#include <QObject>
#include <QGraphicsSceneMouseEvent>

class UpDownLeftRightIum : public QObject
{
	Q_OBJECT
public:
	explicit UpDownLeftRightIum (QObject *parent = nullptr);

	bool eventFilter (QObject *obj, QEvent *event) override;

signals:
	void keyPressed (QKeyEvent *event);
	void keyReleased (QKeyEvent *event);
	void mouseButtonPressed (Qt::MouseButton button);
	void mouseButtonReleased (Qt::MouseButton button);
	void mouseMoved (QPointF location);
};

#endif // UPDOWNLEFTRIGHTIUM_H
