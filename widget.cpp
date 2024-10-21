#include "widget.h"
#include <QtMath>
#include "ui_widget.h"

int xl = -400, xr = 400, yu = -400, yd = 400;

Widget::Widget (QWidget *parent) : QWidget (parent), ui (new Ui::Widget)
{
	ui->setupUi (this);
	aThingyToMove = new QGraphicsItemGroup;
	directionWidget = new QGraphicsItemGroup;

    upArrow = new QGraphicsSimpleTextItem ("↑"); // Proper up arrow is added
    //upArrow->setBrush (Qt::yellow);
    upArrow->setPen(QPen(Qt::black));
	downArrow = new QGraphicsSimpleTextItem ("↓");
    downArrow->setPen (QPen (Qt::black));
	leftArrow = new QGraphicsSimpleTextItem ("←");
    leftArrow->setPen (QPen (Qt::black));
	rightArrow = new QGraphicsSimpleTextItem ("→");
    rightArrow->setPen (QPen (Qt::black));

	directionWidget->addToGroup (upArrow);
	directionWidget->addToGroup (downArrow);
	directionWidget->addToGroup (rightArrow);
	directionWidget->addToGroup (leftArrow);
	QGraphicsEllipseItem *hitBoxItem = new QGraphicsEllipseItem (-5, -10, 10, 20);
	hitBoxItem->setBrush (QBrush (Qt::blue));
	aThingyToMove->addToGroup (hitBoxItem);
	pointingLine = new QGraphicsLineItem (QLineF (0, 0, 0, -2000));
	pointingLine->setPen (QPen (Qt::darkGray));
	pointingLine->setTransform (QTransform().translate (0, -15));

	upArrow->setTransform (QTransform().translate (0 - upArrow->boundingRect().width() / 2, -15 - upArrow->boundingRect().height() / 2));
	downArrow->setTransform (QTransform().translate (0 - downArrow->boundingRect().width() / 2, 15 - downArrow->boundingRect().height() / 2));
	rightArrow->setTransform (QTransform().translate (15 - rightArrow->boundingRect().width() / 2, 0 - rightArrow->boundingRect().height() / 2));
	leftArrow->setTransform (QTransform().translate (-15 - leftArrow->boundingRect().width() / 2, 0 - leftArrow->boundingRect().height() / 2));
	qDebug() << aThingyToMove->transformOriginPoint();
	qDebug() << aThingyToMove->boundingRect().center();
	//	aThingyToMove->setTransformOriginPoint (aThingyToMove->boundingRect().center());
	aThingyToMove->addToGroup (pointingLine);
	aThingyToMove->setData (0, QStringLiteral ("Main Sprite"));

	scene = new Scenium;
	filter = new UpDownLeftRightIum;
	ui->graphicsView->setScene (scene);
	ui->graphicsView->setMouseTracking (true);
	ui->graphicsView->installEventFilter (filter);
	scene->installEventFilter (filter);
	addEnemies();
    addFriends();

	scene->addItem (aThingyToMove);
	scene->addItem (directionWidget);
	//	boundingBoxRect = new QGraphicsRectItem;
	//	boundingBoxRect->setRect (pointingLine->sceneBoundingRect());
	//	boundingBoxRect->setPen (QPen (Qt::darkBlue));
	//	scene->addItem (boundingBoxRect);
	aThingyToMove->setPos (0, 0);
	QGraphicsLineItem *lI1 = scene->addLine (QLineF (0, -100, 0, 100), QPen (Qt::cyan));
	lI1->setData (0, QStringLiteral ("Y Axis"));
	QGraphicsLineItem *lI2 = scene->addLine (QLineF (-100, 0, 100, 0), QPen (Qt::cyan));
	lI2->setData (0, QStringLiteral ("X Axis"));
	QGraphicsRectItem *sceneRect = scene->addRect (QRectF (-400, -400, 800, 800), QPen (Qt::darkRed));
	sceneRect->setData (0, "Scene Bounds");
	ui->graphicsView->setSceneRect (sceneRect->rect());
	ui->graphicsView->centerOn (sceneRect->rect().center());
	connect (&spriteMovementTimer, &QTimer::timeout, this, &Widget::spriteMovement);
	spriteMovementTimer.start (20);
	connect (filter, &UpDownLeftRightIum::keyPressed, this,
		[this] (QKeyEvent *event)
		{
		switch (event->key())
		{
		case Qt::Key_Up:
			currentMovement.up = true;
			break;
		case Qt::Key_Down:
			currentMovement.down = true;
			break;
		case Qt::Key_Right:
			currentMovement.right = true;
			break;
		case Qt::Key_Left:
			currentMovement.left = true;
			break;
		case Qt::Key_E:
			currentMovement.clock = true;
			break;
		case Qt::Key_Q:
			currentMovement.anticlock = true;
			break;
		case Qt::Key_Space:
			currentMovement.poofingOn = true;
			pointingLine->setPen (QPen (Qt::red));
			break;
		default:
			break;
		}
                //qDebug() << "current movement set: " << currentMovement.up << currentMovement.down << currentMovement.right << currentMovement.left;
	});
	connect (filter, &UpDownLeftRightIum::keyReleased, this,
		[this] (QKeyEvent *event)
		{
		switch (event->key())
		{
		case Qt::Key_Up:
			currentMovement.up = false;
			break;
		case Qt::Key_Down:
			currentMovement.down = false;
			break;
		case Qt::Key_Right:
			currentMovement.right = false;
			break;
		case Qt::Key_Left:
			currentMovement.left = false;
			break;
		case Qt::Key_E:
			currentMovement.clock = false;
			break;
		case Qt::Key_Q:
			currentMovement.anticlock = false;
			break;
		case Qt::Key_Space:
			currentMovement.poofingOn = false;
			pointingLine->setPen (QPen (Qt::darkGray));
			break;
		default:
			break;
		}
	});
	connect (filter, &UpDownLeftRightIum::mouseButtonPressed, this,
		[this] (Qt::MouseButton button)
		{
		switch (button)
		{
		case Qt::LeftButton:
			currentMovement.poofingOn = true;
			pointingLine->setPen (QPen (Qt::red));
			break;
		default:
			break;
		}
	});
	connect (filter, &UpDownLeftRightIum::mouseButtonReleased, this,
		[this] (Qt::MouseButton button)
		{
		switch (button)
		{
		case Qt::LeftButton:
			currentMovement.poofingOn = false;
            pointingLine->setPen (QPen (Qt::darkGray));
			break;
		default:
			break;
		}
	});
	connect (scene, &Scenium::mouseMoved, this,
		[this] (QPointF location)
		{
		currentMovement.mousePointerLocation = location;
		ui->textBrowser_2->setText ("X : " + QString::number (currentMovement.mousePointerLocation.rx()) + "\n"
			+ "Y : " + QString::number (currentMovement.mousePointerLocation.ry()));
	});



}

Widget::~Widget() { delete ui; }

void Widget::spriteMovement()
{
	qreal movementDirection{};
	directionWidget->setRotation (0);
	bool didSth = false;
    //QTransform initPos = aThingyToMove->sceneTransform();

        qDebug() << "current movement get : " << currentMovement.up << currentMovement.down << currentMovement.right << currentMovement.left;
	if (currentMovement.up)
	{
                qDebug() << "UP";
		//		aThingyToMove->setTransform (aThingyToMove->sceneTransform().translate (0, -1.5));
		//		aThingyToMove->setPos (aThingyToMove->pos() + QPointF (0, -1.5));
        upArrow->setPen(QPen(Qt::green));
		didSth = true;
	}
	else
	{
        upArrow->setPen(QPen(Qt::gray));
    }
	if (currentMovement.down)
	{
                qDebug() << "DOWN";
		//		aThingyToMove->setTransform (aThingyToMove->sceneTransform().translate (0, 0.7));
		//		aThingyToMove->setPos (aThingyToMove->pos() + QPointF (0, 0.7));
        downArrow->setPen(QPen(Qt::green));
		didSth = true;
	}
	else
	{
        downArrow->setPen(QPen(Qt::gray));
	}
	if (currentMovement.right)
	{
                qDebug() << "RIGHT";
		//		aThingyToMove->setTransform (aThingyToMove->sceneTransform().translate (0.8, 0));
		//		aThingyToMove->setPos (aThingyToMove->pos() + QPointF (0.8, 0));
        rightArrow->setPen(QPen(Qt::green));
		didSth = true;
	}
	else
	{
        rightArrow->setPen(QPen(Qt::gray));
	}
	if (currentMovement.left)
	{
                qDebug() << "LEFT";
		//		aThingyToMove->setTransform (aThingyToMove->sceneTransform().translate (-0.8, 0));
        leftArrow->setPen(QPen(Qt::green));
		didSth = true;
	}
	else
	{
        leftArrow->setPen(QPen(Qt::gray));
	}

	if (!(currentMovement.up && currentMovement.down))
	{
		if (currentMovement.up)
		{
			movementDirection = 0;
			if (!(currentMovement.left && currentMovement.right))
			{
				if (currentMovement.left)
				{
					movementDirection = 315;
				}
				else if (currentMovement.right)
				{
					movementDirection = 45;
				}
			}
		}
		else if (currentMovement.down)
		{
			movementDirection = 180;
			if (!(currentMovement.left && currentMovement.right))
			{
				if (currentMovement.left)
				{
					movementDirection = 225;
				}
				else if (currentMovement.right)
				{
					movementDirection = 135;
				}
			}
		}
		else if (!(currentMovement.left && currentMovement.right))
		{
			if (currentMovement.left)
			{
				movementDirection = 270;
			}
			else if (currentMovement.right)
			{
				movementDirection = 90;
			}
		}
	}

	qreal direction = QLineF (aThingyToMove->scenePos(), currentMovement.mousePointerLocation).angle();
	aThingyToMove->setRotation (-direction + 90);

	if (didSth)
	{
		aThingyToMove->setPos (aThingyToMove->scenePos() + calcDeltaMovement (aThingyToMove->rotation(), movementDirection));
	}
	directionWidget->setPos (aThingyToMove->scenePos());

	static QPointF prevMousePointerPos;
	if (prevMousePointerPos != currentMovement.mousePointerLocation)
	{
		didSth = true;
		prevMousePointerPos = currentMovement.mousePointerLocation;
	}

	if (true)
	{
		//		qDebug() << "SpriteMovement... Initial Position: " << initPos;
		//		qDebug() << "_________________________________________SpriteMovement... Final Position: " << aThingyToMove->sceneTransform() << "||"
		//				 << aThingyToMove->rotation();
		//		qDebug() << aThingyToMove->collidingItems (Qt::IntersectsItemShape);
		QList<QGraphicsItem *> collidingThingies = pointingLine->collidingItems (Qt::IntersectsItemShape);
		QString output = "_____________________________\n";
		output += "Mouse At: " + QString::number (currentMovement.mousePointerLocation.rx()) + ", "
			+ QString::number (currentMovement.mousePointerLocation.ry()) + "\n" + "Direction : " + QString::number (direction) + "\n";
		if (!collidingThingies.isEmpty())
		{
			for (auto g : collidingThingies)
			{
				//				qDebug() << g->data (0) << g->data (1) << g->data (2) << g->data (3) << g->data (4) << g->data (5);
				output.append (g->data (0).toString() + "\n");
				poofTheEnemy (g);
			}
		}
		ui->textBrowser->setText (output);
	}
	//	Q_UNUSED (didSth);
}

void Widget::spriteMovement_M2()
{
	bool didSth = false;
    //	QTransform initPos = aThingyToMove->sceneTransform();

    //	qDebug() << "current movement get : " << currentMovement.up << currentMovement.down << currentMovement.right << currentMovement.left;
	if (currentMovement.up)
	{
                qDebug() << "UP";
		QPointF speedPoint (0, -1.5);
		QPointF finalPosPoint (speedPoint.rx() * qCos (qDegreesToRadians (-aThingyToMove->rotation()))
				+ speedPoint.ry() * qSin (qDegreesToRadians (-aThingyToMove->rotation())),
			speedPoint.rx() * qSin (qDegreesToRadians (-aThingyToMove->rotation()))
				+ speedPoint.ry() * qCos (qDegreesToRadians (-aThingyToMove->rotation())));
		aThingyToMove->setPos (aThingyToMove->pos() + finalPosPoint);
        upArrow->setPen(QPen(Qt::green));
		didSth = true;
	}
	else
	{
        upArrow->setPen(QPen(Qt::gray));
	}
	if (currentMovement.down)
	{
                qDebug() << "DOWN";
		QPointF speedPoint (0, 0.7);
		QPointF finalPosPoint (speedPoint.rx() * qCos (qDegreesToRadians (-aThingyToMove->rotation()))
				+ speedPoint.ry() * qSin (qDegreesToRadians (-aThingyToMove->rotation())),
			speedPoint.rx() * qSin (qDegreesToRadians (-aThingyToMove->rotation()))
				+ speedPoint.ry() * qCos (qDegreesToRadians (-aThingyToMove->rotation())));
		aThingyToMove->setPos (aThingyToMove->pos() + finalPosPoint);
        downArrow->setPen(QPen(Qt::green));
		didSth = true;
	}
	else
	{
        downArrow->setPen(QPen(Qt::gray));
	}
	if (currentMovement.right)
	{
                qDebug() << "RIGHT";
		QPointF speedPoint (0.8, 0);
		QPointF finalPosPoint (speedPoint.rx() * qCos (qDegreesToRadians (aThingyToMove->rotation()))
				+ speedPoint.ry() * qSin (qDegreesToRadians (aThingyToMove->rotation())),
			speedPoint.rx() * qSin (qDegreesToRadians (aThingyToMove->rotation()))
				+ speedPoint.ry() * qCos (qDegreesToRadians (aThingyToMove->rotation())));
		aThingyToMove->setPos (aThingyToMove->pos() + finalPosPoint);
        rightArrow->setPen(QPen(Qt::green));
		didSth = true;
	}
	else
	{
        rightArrow->setPen(QPen(Qt::gray));
	}
	if (currentMovement.left)
	{
                qDebug() << "LEFT";
		QPointF speedPoint (-0.8, 0);
		QPointF finalPosPoint (speedPoint.rx() * qCos (qDegreesToRadians (aThingyToMove->rotation()))
				+ speedPoint.ry() * qSin (qDegreesToRadians (aThingyToMove->rotation())),
			speedPoint.rx() * qSin (qDegreesToRadians (aThingyToMove->rotation()))
				+ speedPoint.ry() * qCos (qDegreesToRadians (aThingyToMove->rotation())));
		aThingyToMove->setPos (aThingyToMove->pos() + finalPosPoint);
        leftArrow->setPen(QPen(Qt::green));
		didSth = true;
	}
	else
	{
        leftArrow->setPen(QPen(Qt::gray));
	}
	if (currentMovement.clock)
	{
        //		qDebug() << "CLOCK";
        //		aThingyToMove->setTransform (aThingyToMove->sceneTransform().rotate (1));
		aThingyToMove->setRotation (aThingyToMove->rotation() + 1);
        //	aThingyToMove->sceneTransform();
        //				aThingyToMove->setTransform (aThingyToMove->transform().rotate (1));

        //		qDebug() << aThingyToMove->transformOriginPoint() << aThingyToMove->sceneTransform();

        //		leftArrow->setBrush (Qt::green);
		didSth = true;
	}
	else
	{
        //		leftArrow->setBrush (Qt::gray);
	}
	if (currentMovement.anticlock)
	{
        //		qDebug() << "ANTICLOCK";
        //		aThingyToMove->setTransform (aThingyToMove->sceneTransform().rotate (-1));
		aThingyToMove->setRotation (aThingyToMove->rotation() - 1);
        //		aThingyToMove->setTransform (aThingyToMove->transform().rotate (-1));

        //		leftArrow->setBrush (Qt::green);
		didSth = true;
	}
	else
	{
		//		leftArrow->setBrush (Qt::gray);
	}
	directionWidget->setRotation (aThingyToMove->rotation());
	directionWidget->setPos (aThingyToMove->scenePos());

	if (true)
	{
		//		qDebug() << "SpriteMovement... Initial Position: " << initPos;
		//		qDebug() << "_________________________________________SpriteMovement... Final Position: " << aThingyToMove->sceneTransform() << "||"
		//				 << aThingyToMove->rotation();
		//		qDebug() << aThingyToMove->collidingItems (Qt::IntersectsItemShape);
		QList<QGraphicsItem *> collidingThingies = pointingLine->collidingItems (Qt::IntersectsItemShape);
		QString output = "_____________________________\n";
		if (!collidingThingies.isEmpty())
		{
			for (auto g : collidingThingies)
			{
				//				qDebug() << g->data (0) << g->data (1) << g->data (2) << g->data (3) << g->data (4) << g->data (5);
				output.append (g->data (0).toString() + "\n");
				poofTheEnemy (g);
			}
		}
		ui->textBrowser->setText (output);
	}
	Q_UNUSED (didSth);
}

bool Widget::poofTheEnemy (QGraphicsItem *potentialEnemy)
{
	if (potentialEnemy->data (0) == "Enemy" && potentialEnemy->data (1) == "Poofable" && currentMovement.poofingOn)
	{
        potentialEnemy->setVisible (false);
        //scene->removeItem(potentialEnemy);



		ui->sB_D3DZC0unter->setValue (ui->sB_D3DZC0unter->value() + 1);

		return true;
	}
	return false;
}

void Widget::addEnemies()
{
	for (int i = 0; i < 10; i++)
	{
		int x = QRandomGenerator::global()->bounded (xl, xr);
		int y = QRandomGenerator::global()->bounded (yu, yd);

        QGraphicsEllipseItem *a = new QGraphicsEllipseItem (x, y, 10, 10);
        a->setPen (QPen (Qt::black));
        a->setBrush (QBrush (Qt::red));
        a->setData (0, QStringLiteral ("Enemy"));
        a->setData (1, QStringLiteral ("Poofable"));
        scene->addItem (a);
        enemyList << a;

	}
    enemyRespawnTimer.start (5000);
	connect (&enemyRespawnTimer, &QTimer::timeout, this,
		[this] ()
		{
        // int n = QRandomGenerator::global()->bounded (10);

  //       enemyList[n]->setVisible (true);
        int x = QRandomGenerator::global()->bounded (xl, xr);   // Changed the logic to add enemies, earlier code was just hiding and showing the enemies as respawning
        int y = QRandomGenerator::global()->bounded (yu, yd);   // Now the code will remove the enemy from the scene and after respawning timer is set will create a new enemy object

        QGraphicsEllipseItem *a = new QGraphicsEllipseItem (x, y, 10, 10);
        a->setPen (QPen (Qt::black));
        a->setBrush (QBrush (Qt::red));
        a->setData (0, QStringLiteral ("Enemy"));
        a->setData (1, QStringLiteral ("Poofable"));
        scene->addItem (a);
        enemyList << a;
	});
}




void Widget::addFriends()
{
    for (int i = 0; i < 10; i++)
    {
        int x = QRandomGenerator::global()->bounded (xl, xr);
        int y = QRandomGenerator::global()->bounded (yu, yd);


        QGraphicsEllipseItem *a = new QGraphicsEllipseItem (x, y, 10, 10);
        a->setPen (QPen (Qt::black));
        a->setBrush (QBrush (Qt::black));
        a->setData (0, QStringLiteral ("Friend"));
        a->setData (1, QStringLiteral ("Safe"));
        scene->addItem (a);
        friendList << a;


    }

}



qreal possiblePowerDeltaC (qreal angle0Delta, qreal angle90Delta, qreal relativeMovementDirection)
{
	qreal fullPower = qSqrt ((angle0Delta * angle0Delta) + (angle90Delta * angle90Delta));
	qreal fullPowerAngle = qAtan (angle90Delta / angle0Delta);
    if ((relativeMovementDirection < fullPowerAngle + 0.001) && (relativeMovementDirection > fullPowerAngle - 0.001))
	{
		// Success
		// => fullPowerDelta in movementDirection
		return fullPower;
	}
	else if (relativeMovementDirection < fullPowerAngle)
	{
		qreal rightMultiplier = qTan (relativeMovementDirection) * angle0Delta;
		qreal possiblePowerDelta = qSqrt ((angle0Delta * angle0Delta) + (angle90Delta * rightMultiplier * angle90Delta * rightMultiplier));
		// => possiblePowerDelta in movementDirection
		return possiblePowerDelta;
	}
	else if (relativeMovementDirection > fullPowerAngle)
	{
		qreal forwardMultiplier = angle90Delta / qTan (relativeMovementDirection);
		qreal possiblePowerDelta = qSqrt ((angle0Delta * forwardMultiplier * angle0Delta * forwardMultiplier) + (angle90Delta * angle90Delta));
		// => possiblePowerDelta in movementDirection
		return possiblePowerDelta;
	}
	else
	{
		Q_ASSERT (false);
	}
	return qreal{};
}

QPointF deltaPointInMovementDirection (qreal absoluteDelta, qreal movementDirection)
{
	//	movementDirection -> viewPortMovementDirection
	//	^(0)				^(-90)
	//	|					|
	//	|					|
	//	|					|
	//	|					|
	//	------->(+90)		--------------> (0)
	qreal viewPortMovementDirection = movementDirection - 90;
	qreal deltaX = absoluteDelta * qSin (viewPortMovementDirection);
	qreal deltaY = absoluteDelta * qCos (viewPortMovementDirection);
	return QPointF (deltaX, deltaY);
}

QPointF Widget::calcDeltaMovement (qreal rotatedAngle, qreal movementDirection)
{
	qreal relMoveDir = movementDirection - rotatedAngle;
	//	Q_ASSERT (movementDirection > 0);
	//	Q_ASSERT (movementDirection < 360);
	//	Q_ASSERT (rotatedAngle > 0);
	//	Q_ASSERT (rotatedAngle < 360);
	if (relMoveDir < 0)
	{
		relMoveDir = relMoveDir + 360;
	}
	if (relMoveDir > 360)
	{
		relMoveDir = relMoveDir - 360;
	}
	qreal forwardDelta = 1.5, backwardDelta = 0.7, rightDelta = 0.8, leftDelta = 0.8;

	/// The following is much cheaper than linear programming
	if ((relMoveDir < 0.001) && (relMoveDir > (-0.001 + 360)))
	{
		// Success
		// Return Forward
		// => forwardDelta in movementDirection
		return deltaPointInMovementDirection (forwardDelta, movementDirection);
	}
	else if ((relMoveDir < 90.001) && (relMoveDir > (90 - 0.001)))
	{
		// Success
		// Return Right
		// => rightDelta in movementDirection
		return deltaPointInMovementDirection (rightDelta, movementDirection);
	}
	else if ((relMoveDir < 180.001) && (relMoveDir > (180 - 0.001)))
	{
		// Success
		// Return Backward
		// => backwardDelta in movementDirection
		return deltaPointInMovementDirection (backwardDelta, movementDirection);
	}
	else if ((relMoveDir < 270.001) && (relMoveDir > (270 - 0.001)))
	{
		// Success
		// Return Left
		// => leftDelta in movementDirection
		return deltaPointInMovementDirection (leftDelta, movementDirection);
	}
	else if (relMoveDir < 90)
	{
		//	|0
		//	|
		//	|
		//	|
		//	-------(+90)
		return deltaPointInMovementDirection (possiblePowerDeltaC (forwardDelta, rightDelta, relMoveDir), movementDirection);
		/* Example usage of possiblePowerDeltaC
		qreal fullPower = qSqrt ((forwardDelta * forwardDelta) + (rightDelta * rightDelta));
		qreal fullPowerAngle = qAtan (rightDelta / forwardDelta);
		if ((relMoveDir < fullPowerAngle + 0.001) && (relMoveDir > fullPowerAngle - 0.001))
		{
			// Success
			// => fullPowerDelta in rotatedAngle
		}
		else if (relMoveDir < fullPowerAngle)
		{
			qreal rightMultiplier = qTan (relMoveDir) * forwardDelta;
			qreal possiblePowerDelta = qSqrt ((forwardDelta * forwardDelta) + (rightDelta * rightMultiplier * rightDelta * rightMultiplier));
			// => possiblePowerDelta in rotatedAngle
		}
		else if (relMoveDir > fullPowerAngle)
		{
			qreal forwardMultiplier = rightDelta / qTan (relMoveDir);
			qreal possiblePowerDelta = qSqrt ((forwardDelta * forwardMultiplier * forwardDelta * forwardMultiplier) + (rightDelta * rightDelta));
			// => possiblePowerDelta in rotatedAngle
		}
*/
	}
	else if (relMoveDir < 180)
	{
		//	__________ (0)
		//	|
		//	|
		//	|
		//	|(+90)
		return deltaPointInMovementDirection (possiblePowerDeltaC (rightDelta, backwardDelta, relMoveDir), movementDirection);
	}
	else if (relMoveDir < 270)
	{
		//	(+90)____________
		//					|
		//					|
		//					|
		//					|(0)
		return deltaPointInMovementDirection (possiblePowerDeltaC (backwardDelta, leftDelta, relMoveDir), movementDirection);
	}
	else if (relMoveDir < 360)
	{
		//				|(+90)
		//				|
		//				|
		//				|
		//	(0)----------
		return deltaPointInMovementDirection (possiblePowerDeltaC (leftDelta, forwardDelta, relMoveDir), movementDirection);
	}
	else
	{
		qDebug() << "Rel Move Dir == " << relMoveDir;
		Q_ASSERT (false);
		return QPointF{};
	}
}

void Widget::on_pB_SwitchMovementMethod_clicked()
{
	static bool a = false;
	if (a)
	{
		upArrow->setText ("↑");
		connect (&spriteMovementTimer, &QTimer::timeout, this, &Widget::spriteMovement);
		disconnect (&spriteMovementTimer, &QTimer::timeout, this, &Widget::spriteMovement_M2);
		a = false;
	}
	else
	{
        upArrow->setText ("↑");
		connect (&spriteMovementTimer, &QTimer::timeout, this, &Widget::spriteMovement_M2);
		disconnect (&spriteMovementTimer, &QTimer::timeout, this, &Widget::spriteMovement);
		a = true;
	}
}

void Widget::on_pB_Reset_clicked() { aThingyToMove->setTransform (QTransform{}); }

void Widget::on_pB_Shear_clicked() { aThingyToMove->setTransform (aThingyToMove->sceneTransform().shear (1, 0)); }

void Widget::on_pB_BoundBoxRectToggle_clicked() {}
