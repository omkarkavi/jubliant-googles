#ifndef WIDGET_H
#define WIDGET_H

#include <QRandomGenerator>
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsRectItem>
#include <QWidget>
#include <QTimer>
#include "UpDownLeftRightIum.h"
#include "Scenium.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = nullptr);
	~Widget();

private slots:

	void on_pB_SwitchMovementMethod_clicked ();

	void on_pB_Reset_clicked ();

	void on_pB_Shear_clicked ();

	void on_pB_BoundBoxRectToggle_clicked ();

private:
	Ui::Widget *ui;
	QGraphicsItemGroup *aThingyToMove, *directionWidget;
	QGraphicsSimpleTextItem *upArrow, *downArrow, *leftArrow, *rightArrow;
	Scenium *scene;
	QGraphicsLineItem *pointingLine;
	//	QGraphicsRectItem *boundingBoxRect;
	QList<QGraphicsEllipseItem *> enemyList;
    QList<QGraphicsEllipseItem *> friendList;

	struct SPRITE_MOVEMENT
	{
		bool up{};
		bool down{};
		bool right{};
		bool left{};
		bool clock{};
		bool anticlock{};
		QPointF mousePointerLocation{};
		bool poofingOn{};
	} currentMovement{};

	void spriteMovement ();
	void spriteMovement_M2 ();

	QTimer spriteMovementTimer, enemyRespawnTimer;

	UpDownLeftRightIum *filter;

	bool poofTheEnemy (QGraphicsItem *potentialEnemy);

    void addEnemies();
    void addFriends();

	QPointF calcDeltaMovement (qreal rotatedAngle, qreal movementDirection);
};
#endif // WIDGET_H
