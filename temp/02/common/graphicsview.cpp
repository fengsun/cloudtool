﻿#include "graphicsview.h"

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
    this->setScene(&m_scene);
    this->setInteractive(true);
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setRubberBandSelectionMode(Qt::ContainsItemShape);
    this->setAcceptDrops(true);
}

void GraphicsView::addImage(const cv::Mat &imagein)
{
    QGraphicsPixmapItem *item= new QGraphicsPixmapItem();
    QImage image=this->Mat2QImage(imagein);
    item->setPixmap(QPixmap::fromImage(image));
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    item->setAcceptedMouseButtons(Qt::LeftButton);
    m_scene.clear();
    m_scene.addItem(item);
    if(image.height()>this->height()){
        this->fitInView(item,Qt::KeepAspectRatio);
    }
}

void GraphicsView::removeImage()
{
   m_scene.clear();
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        double angleDeltaY = event->angleDelta().y();
        double zoomFactor = qPow(1.0015, angleDeltaY);
        scale(zoomFactor, zoomFactor);
        if(angleDeltaY > 0)
        {
            this->centerOn(sceneMousePos);
            sceneMousePos = this->mapToScene(event->pos());
        }
        this->viewport()->update();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        QMenu menu;
        QAction *clearAllAction = menu.addAction("Clear All");
        connect(clearAllAction,
                SIGNAL(triggered(bool)),
                this,
                SLOT(clearAll(bool)));

        QAction *clearSelectedAction = menu.addAction("Clear Selected");
        connect(clearSelectedAction,
                SIGNAL(triggered(bool)),
                this,
                SLOT(clearSelected(bool)));

        QAction *noEffectAction = menu.addAction("No Effect");
        connect(noEffectAction,
                SIGNAL(triggered(bool)),
                this,
                SLOT(noEffect(bool)));

        QAction *blurEffectAction = menu.addAction("Blur Effect");
        connect(blurEffectAction,
                SIGNAL(triggered(bool)),
                this,
                SLOT(blurEffect(bool)));

        QAction *dropShadEffectAction = menu.addAction("Drop Shadow Effect");
        connect(dropShadEffectAction,
                SIGNAL(triggered(bool)),
                this,
                SLOT(dropShadowEffect(bool)));

        QAction *colorizeEffectAction = menu.addAction("Colorize Effect");
        connect(colorizeEffectAction,
                SIGNAL(triggered(bool)),
                this,
                SLOT(colorizeEffect(bool)));

        QAction *customEffectAction = menu.addAction("Custom Effect");
        connect(customEffectAction,
                SIGNAL(triggered(bool)),
                this,
                SLOT(customEffect(bool)));
        menu.exec(event->globalPos());
        event->accept();
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
    else
        event->ignore();
}

void GraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
    else
        event->ignore();
}

void GraphicsView::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData=event->mimeData();
    if(mimeData->hasUrls()) {
        QStringList path;
        QList<QUrl> urlList=mimeData->urls();
        for(int i=0;i<urlList.size();i++) {
            QString filepath=urlList.at(i).toLocalFile();
            if(!filepath.isEmpty())
                path.push_back(filepath);
        }
        emit  dropFilePath(path);
    }
}

cv::Mat GraphicsView::QImage2Mat(const QImage &image, bool inCloneImageData)
{
    switch ( image.format() )
      {
         // 8-bit, 4 channel
         case QImage::Format_RGB32:
         {
            cv::Mat mat( image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine() );
            return (inCloneImageData ? mat.clone() : mat);
         }

         // 8-bit, 3 channel
         case QImage::Format_RGB888:
         {
            if ( !inCloneImageData ) {
               qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";
            }
            QImage swapped = image.rgbSwapped();
            return cv::Mat( swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();
         }

         // 8-bit, 1 channel
         case QImage::Format_Indexed8:
         {
            cv::Mat  mat( image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.bits()), image.bytesPerLine() );

            return (inCloneImageData ? mat.clone() : mat);
         }
            break;
      }

      return cv::Mat();

}

QImage GraphicsView::Mat2QImage(const cv::Mat &mat)
{
    switch ( mat.type() )
      {
         // 8-bit, 4 channel
         case CV_8UC4:
         {
            QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32 );
            return image;
         }

         // 8-bit, 3 channel
         case CV_8UC3:
         {
            QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888 );
            return image.rgbSwapped();
         }

         // 8-bit, 1 channel
         case CV_8UC1:
         {
            static QVector<QRgb>  sColorTable;
            // only create our color table once
            if ( sColorTable.isEmpty() )
            {
               for ( int i = 0; i < 256; ++i )
                  sColorTable.push_back( qRgb( i, i, i ) );
            }
            QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8 );
            image.setColorTable( sColorTable );
            return image;
         }

         default:
            qDebug("Image format is not supported: depth=%d and %d channels\n", mat.depth(), mat.channels());
            break;
      }
      return QImage();
}

void GraphicsView::clearAll(bool)
{
    scene()->clear();
}

void GraphicsView::clearSelected(bool)
{
    while(scene()->selectedItems().count() > 0)
    {
        delete scene()->selectedItems().at(0);
        scene()->selectedItems().removeAt(0);
    }
}

void GraphicsView::noEffect(bool)
{
    foreach(QGraphicsItem *item, scene()->selectedItems())
    {
        item->setGraphicsEffect(Q_NULLPTR);
    }
}

void GraphicsView::blurEffect(bool)
{
    foreach(QGraphicsItem *item, scene()->selectedItems())
    {
        item->setGraphicsEffect(new QGraphicsBlurEffect(this));
    }
}

void GraphicsView::dropShadowEffect(bool)
{
    foreach(QGraphicsItem *item, scene()->selectedItems())
    {
        item->setGraphicsEffect(new QGraphicsDropShadowEffect(this));
    }
}

void GraphicsView::colorizeEffect(bool)
{
    foreach(QGraphicsItem *item, scene()->selectedItems())
    {
        item->setGraphicsEffect(new QGraphicsColorizeEffect(this));
    }
}

void GraphicsView::customEffect(bool)
{
//    foreach(QGraphicsItem *item, scene()->selectedItems())
//    {
        //item->setGraphicsEffect(new QCustomGraphicsEffect(this));
//    }
}