#ifndef RADAR_H
#define RADAR_H

#include <QWidget>
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QPoint>
#include <QtMath>
#include <QMouseEvent>
#include <QLabel>
#include <QToolTip>
#include <cmath>
#include <QWheelEvent>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <QColor>

//枚举，扫描方式
enum ScanModeEnum{
    /// 圆周扫描
    CircleScan = 0,
    // 扇形扫描
    SectorScan
};
//枚举，扫描方向
enum ScanDirectionEnum{
    ClockWize = 0,                    //顺时针扫描
    AntiClockWize                   //逆时针扫描
};

//目标信息
struct  TargetInfo
{
    std::string sTargetId;          //批号
    std::string sTOA ;              //到达时间
    double dAzimuth ;               //方位
    double dPitch  ;                //俯仰
    double dDistance;               //距离
    double dFrequency ;             //射频
    double dPulseWidth ;            //脉宽
    double dPulseAmplitude ;       //脉幅（0.5-4。5）
    double dPRI ;                   //脉冲重复周期
    double dRCS ;                   //目标反射面积
    int  nAttr;                     //目标属性，0：我方;1:敌方；2：中立；3：不明

};

class Radar : public QWidget
{
    Q_OBJECT

public:
    Radar(QWidget *parent = nullptr);
    ~Radar();

    //----------------------------------------------------
    //  函数名称：Start
    //  作用：开启雷达扫描定时器
    //  参数：参数一：int类型，表示定时器间隔，默认为500ms
    //-----------------------------------------------------
    void Start(int nScanPeriod = 500);

    //----------------------------------------------------
    //  函数名称：Stop
    //  作用：关闭雷达扫描定时器
    //  参数：无参数
    //-----------------------------------------------------
    void Stop();

    //----------------------------------------------------
    //  函数名称：Init
    //  作用：初始化雷达扫描基本数据
    //  参数：参数一：int类型，表示最大探测距离，即雷达扫描图半径，默认300
    //        参数二：int类型，表示等分数，即雷达扫描图圈数，默认5圈
    //        参数三：int类型，扫描周期，默认500毫秒更新一次
    //        参数四：int类型，扫描波束宽度，默认为10度
    //        参数五：bool类型，是否显示轨迹，true表示显示轨迹
    //-----------------------------------------------------
    void Init(int nMaxDetectionDistance = 300,int nEqualParts = 5,int nScanPeriod = 500,
              int nBeamWidth = 10,bool bShowTrack = false);

    //----------------------------------------------------
    //  函数名称：SetShowTrack
    //  作用：设置是否显示航迹(一个目标的各个坐标点之间的连线成为航迹)
    //  参数：参数一：bool类型，是否显示轨迹，true表示显示轨迹
    //-----------------------------------------------------
    void SetShowTrack(bool bShowTrack = false);

    //----------------------------------------------------
    //  函数名称：SetScanPeriod
    //  作用：设置扫描周期，停止-设置扫描周期-开始
    //  参数：参数一：int类型，默认为500ms
    //-----------------------------------------------------
    void SetScanPeriod(int nScanPeriod = 500);

    //----------------------------------------------------
    //  函数名称：SetScanParams
    //  作用：设置扫描参数 停止-设置扫描参数-开始
    //  参数：参数一：ScanDirectionEnum类型，扫描方式
    //        参数二：ScanModeEnum类型，扫描方向
    //        参数三：int类型，扫描波束宽度，默认为10度
    //        参数四：int类型，起始角，默认从0度开始
    //        参数五：int类型，结束角，默认360度结束
    //-----------------------------------------------------
    void SetScanParams(ScanDirectionEnum scanDirection = ClockWize,ScanModeEnum scanMode = CircleScan,
                       int nBeamWidth = 10,int nStartAngle = 0,int nEndAngle = 360);

    //----------------------------------------------------
    //  函数名称：SetData
    //  作用：设置单点坐标数据
    //  参数：参数一：TargetInfo类型
    //-----------------------------------------------------
    void SetData(TargetInfo ti);

    //----------------------------------------------------
    //  函数名称：SetData
    //  作用：设置多点坐标数据
    //  参数：参数一：std::vector<TargetInfo>类型
    //-----------------------------------------------------
    void SetData(std::vector<TargetInfo>& ti);

    //----------------------------------------------------
    //  函数名称：SetColor
    //  作用：设置航迹线颜色
    //  参数：颜色的rgb数值
    //-----------------------------------------------------
    void SetColor(int r, int g, int b);

public slots:

private slots:

    void timerTimeOut();

    void mousePressEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event) override;

private:

    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent *event);

    //----------------------------------------------------
    //  函数名称：drawAzimuth
    //  作用：绘制雷达扫描图方位
    //  参数：参数一：QPainter对象，类型：QPainter
    //        参数二、三：在指定坐标(x, y)上绘制方位文本，类型：int
    //        参数四：绘制的方位数据，类型：int
    //-----------------------------------------------------
    void drawAzimuth(QPainter *painter, int x, int y, int azimuth);

    //----------------------------------------------------
    //  函数名称：drawRange
    //  作用：绘制雷达扫描图量程
    //  参数：参数一：QPainter对象，类型：QPainter
    //        参数二、三：在指定坐标(x, y)上绘制方位文本，类型：int
    //        参数四：绘制的量程数据，类型：int
    //-----------------------------------------------------
    void drawRange(QPainter *painter, int x, int y, int range, int range1);

    //----------------------------------------------------
    //  函数名称：drawPoints
    //  作用：绘制目标
    //  参数：参数一：QPainter对象，类型：QPainter
    //        参数二：目标的坐标
    //-----------------------------------------------------
    void drawPoints(QPainter* painter, QVector<QPoint> &Record);

    //----------------------------------------------------
    //  函数名称：drawTarget
    //  作用：绘制目标
    //  参数：参数一：QPainter对象，类型：QPainter
    //        参数二：目标的信息
    //-----------------------------------------------------
    void drawTarget(QPainter* painter, TargetInfo ti);

private:

    // 圆心坐标
    int CoorX;
    int CoorY;
    // 实际半径
    int pRadius;
    // 圈数
    int pNumTurns;
    //  扫描周期
    int pScanPeriod;
    // 扫描速率
    int ScanRate;
    // 扫描波束宽度
    int pBeamWidth;
    // 显示轨迹
    bool pShowTrack;
    // 缩放因子
    int ScaleFactor;
    // 最小边长
    int minSide;
    // 窗口的半径
    int MeasureRadius;
    // 单点坐标数据
    TargetInfo pTargetInfo;
    // 多点坐标数据
    std::vector<TargetInfo> pTargetInfos;
    // 定时器对象
    QTimer *timer;
    // 扫描线的初始角度
    int pStartAngle;
    // 扫描线的结束角度
    int pEndAngle;
    // 绘制目标的直径大小
    double cir_size;
    // 记录目标的轨迹点
    std::unordered_map<std::string, QVector<QPoint>>Record;
    // 扫描方式
    ScanModeEnum pScanMode;
    // 扫描方向
    ScanDirectionEnum  pScanDirection;
    int _start;
    int _end;
    bool flag;

    // 航迹线颜色r、g、b
    int r;
    int g;
    int b;
};
#endif // RADAR_H
