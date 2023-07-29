#include "Radar.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QRectF>

Radar::Radar(QWidget *parent)
    : QWidget(parent)
{
    timer = new QTimer();

    // 目标点默认半径为5
    cir_size = 5;
    flag = false;

    // 监听窗口大小变化的事件
    installEventFilter(this);

    // 默认扫描波束宽度为圆周，即360度
    pBeamWidth = 360;
    ScaleFactor = 1;
    ScanRate = 1;

    // 默认目标点颜色为蓝色
    r = 0;
    g = 0;
    b = 255;

    //强制设置背景颜色为黑色
    setPalette(Qt::black); setAutoFillBackground(true);
    //用槽函数控制雷达扫描效果。
    connect(timer,SIGNAL(timeout()),this,SLOT(timerTimeOut()));

    // 设置自适应窗口
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);
}

Radar::~Radar()
{
}

void Radar::Start(int nScanPeriod)
{
    timer->start(nScanPeriod);
}

void Radar::Stop()
{
    flag = false;
    timer->stop();
}

void Radar::Init(int nMaxDetectionDistance, int nEqualParts, int nScanPeriod, int nBeamWidth, bool bShowTrack)
{
    this->pRadius = nMaxDetectionDistance;
    this->pNumTurns = nEqualParts;
    this->pScanPeriod = nScanPeriod;
    this->pBeamWidth = nBeamWidth;
    this->pShowTrack = bShowTrack;
}

void Radar::SetShowTrack(bool bShowTrack)
{
    this->pShowTrack = bShowTrack;
}

void Radar::SetScanPeriod(int nScanPeriod)
{
    timer->stop();
    this->pScanPeriod = nScanPeriod;
    timer->start(nScanPeriod);
}

void Radar::SetScanParams(ScanDirectionEnum scanDirection, ScanModeEnum scanMode,
                           int nBeamWidth, int nStartAngle, int nEndAngle)
{
    timer->stop();
    this->pScanDirection = scanDirection;
    this->pScanMode = scanMode;
    this->pBeamWidth = nBeamWidth;
    this->pStartAngle = nStartAngle;
    this->pEndAngle = nEndAngle;

    _start = pStartAngle;
    _end = pEndAngle - pBeamWidth;
    timer->start(pScanPeriod);
}

void Radar::SetData(TargetInfo ti)
{
    this->pTargetInfo = ti;
}

void Radar::SetData(std::vector<TargetInfo> &ti)
{
    this->pTargetInfos = ti;
}

void Radar::SetColor(int r, int g, int b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

void Radar::timerTimeOut()
{
    if(pScanMode == CircleScan)
    {
        if(pScanDirection == ClockWize)
        {
            if(!flag)
            {
                pStartAngle = 90;
                flag = true;
            }

            pBeamWidth = 360;

            //每次旋转ScanRate度
            pStartAngle -= ScanRate;

            if(pStartAngle <= -360)
            {
                pStartAngle = 0;
            }
        }
        else
        {
            if(!flag)
            {
                pStartAngle = 90;
                flag = true;
            }

            pBeamWidth = 360;

            //每次旋转ScanRate度
            pStartAngle += ScanRate;

            if(pStartAngle >= 360)
            {
                pStartAngle = 0;
            }
        }
    }
    else if(pScanMode == SectorScan)
    {
        //每次旋转ScanRate度
        pStartAngle -= ScanRate;

        if(pStartAngle < _start || pStartAngle > _end)
        {
            ScanRate *= -1;
        }
    }

    update();
}

void Radar::paintEvent(QPaintEvent *event)//此函数自动执行，实现扫描动画效果
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPen pen;
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(qRgba(128, 255, 0,250)));

    // 圆心
    CoorX = this->width() / 2;
    CoorY = this->height() / 2;

    // 获取窗口的宽度和高度
    int width = this->width();
    int height = this->height();

    // 计算最小边长
    minSide = qMin(width, height) - 50;

    // 求量程
    int range = pRadius / pNumTurns;
    MeasureRadius = minSide / 2;
    int range1 = MeasureRadius / pNumTurns;

    // 从外到内画圆
    QPoint center(CoorX,CoorY);
    for(int i = 0;i < pNumTurns;i++)
    {
        painter.drawEllipse(center, ScaleFactor * (MeasureRadius - range1 * i),
                                   ScaleFactor * (MeasureRadius - range1 * i));
    }

    // 画量程
    drawRange(&painter, CoorX, CoorY, range, range1);

    /** 画外圈刻度值，将坐标系移动到圆心 **/
    painter.translate(CoorX, CoorY);
    painter.setPen(QPen(qRgba(255, 255, 0,250)));
    painter.setFont(QFont("Calibri",10));
    for(int i=1;i<=360;i++){
        painter.rotate(1);
        painter.drawLine(0, ScaleFactor * (MeasureRadius * -1), 0, ScaleFactor * (MeasureRadius * -1 + 5));
    }

    for(int i=1;i<=72;i++){
        painter.rotate(5);
        painter.drawLine(0, ScaleFactor * (MeasureRadius * -1), 0, ScaleFactor * (MeasureRadius * -1 + 8));
    }

    for(int i=1;i<=36;i++){
        painter.setRenderHint(QPainter::Antialiasing);
        painter.rotate(10);
        painter.drawLine(0, ScaleFactor * (MeasureRadius * -1),0, ScaleFactor * (MeasureRadius * -1 + 12));
    }

    for(int i = 1;i <= 4;i++){

        // 外圈刻度值
        painter.drawLine(0, 0, 0, ScaleFactor * (MeasureRadius * -1));

        // 绘制方位
        drawAzimuth(&painter, -10, ScaleFactor * (MeasureRadius * -1 - 10), (i - 1) * 90);

        // 顺时针旋转90度
        painter.rotate(90);
    }

    painter.translate(-CoorX,-CoorY);//恢复坐标系
    painter.setPen(QPen(Qt::NoPen));//去掉外框线

    // 圆周扫描
    if(pScanMode == CircleScan)
    {
        /** 绘制扫描效果 **/
        if(pScanDirection == ClockWize)
        {
            QConicalGradient gradient(CoorX, CoorY, pStartAngle);
            gradient.setColorAt(0.1, QColor(128, 255, 0, 150));
            gradient.setColorAt(0.9, QColor(0, 255, 0, 0));//尾部
            painter.setBrush(gradient);
        }
        else if(pScanDirection == AntiClockWize)
        {
            QConicalGradient gradient(CoorX, CoorY, pStartAngle);
            gradient.setColorAt(0.9, QColor(128, 255, 0, 150));
            gradient.setColorAt(0.1, QColor(0, 255, 0, 0));//尾部
            painter.setBrush(gradient);
        }

        painter.drawPie(CoorX-ScaleFactor * MeasureRadius, CoorY-ScaleFactor * MeasureRadius,
                    ScaleFactor * MeasureRadius * 2, ScaleFactor * MeasureRadius * 2,
                    -pStartAngle * 16, pBeamWidth * 16);
    }// 扇形扫描
    else if(pScanMode == SectorScan)
    {
        painter.setBrush(QBrush(QColor(128, 255, 0, 150)));

        painter.save();
        QTransform transform;
        transform.translate(CoorX, CoorY);  // 平移原点位置
        painter.setTransform(transform);  // 应用变换矩阵
        painter.rotate(-90);
        painter.drawPie(-ScaleFactor * MeasureRadius, -ScaleFactor * MeasureRadius,
                        ScaleFactor * MeasureRadius * 2, ScaleFactor * MeasureRadius * 2,
                        -pStartAngle * 16, -pBeamWidth * 16);
        painter.restore();
    }

    //模拟画出可疑点
    painter.translate(CoorX, CoorY);
    painter.setBrush(QBrush(QColor(50, 255, 200, 200)));

    if(pTargetInfo.sTargetId.size() != 0)
    {
        drawTarget(&painter, pTargetInfo);
    }

    // 绘制多点坐标数据
    for(auto target : pTargetInfos)
    {
        drawTarget(&painter, target);
    }
}


void Radar::resizeEvent(QResizeEvent *event)
{
    // 窗口大小改变时重新绘制
    Q_UNUSED(event);
    update();
}

void Radar::wheelEvent(QWheelEvent *event)
{
    int angleDelta = event->angleDelta().y();
    qreal scaleFactorDelta = 1;

    if (angleDelta > 0) {
        // 滚轮向前滚动，放大图形
        ScaleFactor += scaleFactorDelta;
    } else {
        // 滚轮向后滚动，缩小图形
        ScaleFactor -= scaleFactorDelta;
    }

    // 限制放大缩小比例的范围
    if (ScaleFactor < 1)
    {
        ScaleFactor = 1;
    }
    else if (ScaleFactor > 5)
    {
        ScaleFactor = 5;
    }

    // 更新小部件进行重绘
    update();
}

void Radar::drawAzimuth(QPainter *painter, int x, int y, int azimuth)
{
    if(azimuth == 180)
    {
        painter->save();
        painter->rotate(180);

        QFont font = QFont("Arial",16);

        QString str = QString::number(azimuth);

        painter->setFont(font);
        painter->drawText(x - 10, (y - 10) * -1, str);
        painter->restore();
    }
    else
    {
        QFont font = QFont("Arial",16);

        QString str = QString::number(azimuth);

        painter->setFont(font);
        painter->drawText(x, y, str);
    }
}

// 绘制量程
void Radar::drawRange(QPainter* painter, int x, int y, int range, int range1)
{
    for(int i = 1;i <= pNumTurns;i++)
    {
        painter->drawText(x, y + ScaleFactor * range1 * i - 3, QString::number(range * i));
        painter->drawText(x, y - ScaleFactor * range1 * i + 13, QString::number(range * i));
        painter->drawText(x + ScaleFactor * range1 * i - 20,y, QString::number(range * i));
        painter->drawText(x - ScaleFactor * range1 * i, y, QString::number(range * i));
    }
}

void Radar::drawPoints(QPainter* painter, QVector<QPoint> &Record)
{
    // 获取窗口的宽度和高度
    int width = this->width();
    int height = this->height();

    // 计算最小边长
    minSide = qMin(width, height) - 50;
    MeasureRadius = minSide / 2;

    QVector<QPoint>p;
    for(auto point : Record)
    {
        int x = point.x() * MeasureRadius / pRadius * ScaleFactor;
        int y = point.y() * MeasureRadius / pRadius * ScaleFactor;

        QPoint z(x, y);
        p.emplace_back(z);
    }

    painter->drawPoints(p.data(), p.size());
}

void Radar::drawTarget(QPainter* painter, TargetInfo ti)
{
    if(ti.dDistance > pRadius)
    {
        return;
    }

    QColor color(r, g, b);
    painter->setPen(QPen(color));

    double angle = qDegreesToRadians(-ti.dAzimuth + 90); // 将逆时针转换为顺时针，并调整方向，使y轴正方向为0度，x轴正方向为90度

    double x1 = CoorX + ti.dDistance * cos(angle);
    double y1 = CoorY - ti.dDistance * sin(angle);

    int x2 = x1 - CoorX;
    int y2 = -1 * (y1 - CoorY);

    int x = x2 * MeasureRadius / pRadius;
    int y = y2 * MeasureRadius / pRadius;

    QPoint p(x2, -1 * y2);

    if(Record.find(ti.sTargetId) != Record.end())
    {
        Record[ti.sTargetId].append(p);
    }
    else
    {
        QVector<QPoint> temp;
        temp.append(p);
        Record[ti.sTargetId] = temp;
    }

    painter->drawEllipse(x * ScaleFactor,-1 * y * ScaleFactor, cir_size, cir_size);

    if(pShowTrack)
        drawPoints(painter, Record[ti.sTargetId]);

    // 画出批号
    QFont font = QFont("Arial",11);
    painter->setFont(font);
    painter->setPen(QPen(Qt::red));

    painter->drawText(x * ScaleFactor,-1 * y * ScaleFactor, QString::fromStdString(ti.sTargetId));
}

void Radar::mousePressEvent(QMouseEvent *event)
{
    //如果鼠标左键按下
    if (event->type() == QEvent::MouseButtonPress) {

         int x = event->x();
         int y = event->y();

         // 计算最小边长
         minSide = qMin(width(), height()) - 50;
         MeasureRadius = minSide / 2;

         // 计算距离
         // 圆心坐标(CoorX, CoorY)
         int distance = sqrt(pow(x - CoorX, 2) + pow(y - CoorY, 2))
                        * pRadius / MeasureRadius;

         // 计算方位
         double azimuth = atan2(y - CoorY, x - CoorX) * 180 / M_PI;
         //坐标转换
         QPoint point = this->mapToGlobal(event->pos());
         point.setX(point.x() - 20);
         point.setY(point.y() - 50);

         if(static_cast<int>(azimuth) < -90 && static_cast<int>(azimuth) > -180)
         {
             QToolTip::showText(point, QString("距离: %1\n方位: %2").arg(distance / ScaleFactor).arg(azimuth + 450));
         }
         else
         {
             QToolTip::showText(point, QString("距离: %1\n方位: %2").arg(distance / ScaleFactor).arg(azimuth + 90));
         }
    }
}
