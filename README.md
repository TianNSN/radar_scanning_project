# RadarScanningProject

#### 介绍
使用QPainter开发的雷达P显，目前完成的功能有自适应窗口、扫描方式（圆周扫描、扇形扫描（指定起始角度和结束角度））、扫描方向（顺时针/逆时针）、目标点的批号、航迹线的绘制、方位，量程绘制、鼠标左键点击显示当前点相对于圆心的距离和方位、鼠标滚轮放大缩小。

没有用到其他的第三方库，纯QPainter绘制，拿来即用。


#### 安装教程
在项目中添加头文件Radar.h、源文件Radar.cpp即可

#### 使用说明

1.包含Radar.h，实例化一个Radar*对象，使用接口函数

2.结构体

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

3.接口函数有

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

4.关于函数调用参见main.cpp

#### 效果图

扫描方式：圆周扫描

扫描方向：顺时针

显示目标航迹线，目标批号

![圆周扫描](./resource/%E5%9C%86%E5%91%A8%E6%89%AB%E6%8F%8F.png)

目标消失

![圆周扫描](./resource/%E5%9C%86%E5%91%A8%E6%89%AB%E6%8F%8F(1).png)

扫描方式：扇形扫描

扫描方向：无(扇形扫描没有顺时针或逆时针)

扫描波束宽度：10度

扫描范围：0~90度

![扇形扫描](./resource/%E6%89%87%E5%BD%A2%E6%89%AB%E6%8F%8F.png)

目标消失

![扇形扫描](./resource/%E6%89%87%E5%BD%A2%E6%89%AB%E6%8F%8F(2).png)

鼠标左键点击显示当前位置相对于圆心的距离和方位

![鼠标点击](./resource/%E9%BC%A0%E6%A0%87%E5%B7%A6%E9%94%AE%E7%82%B9%E5%87%BB.png)
