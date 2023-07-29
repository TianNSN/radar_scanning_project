#include "Radar.h"

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QTimer>
#include <cmath>
#include <random>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QMainWindow window;

    // -------------------------------测试开始------------------------------
    // 可疑点的方位距离测试
    std::vector<TargetInfo> InitTargetInfo(20);
    // 生成20个100~300不重复的距离
    std::mt19937 generator(std::time(0));
    std::uniform_int_distribution<int> distribution(100, 300);
    for (int i = 0; i < 20; ++i) {
        InitTargetInfo[i].dDistance = distribution(generator);
    }
    //------------------------------------------------------------
    // 生成20个0~360不重复的方位
    std::mt19937 generator1(std::time(0));
    std::uniform_int_distribution<int> distribution1(0, 360);
    for (int i = 0; i < 20; ++i) {
        InitTargetInfo[i].dAzimuth = distribution1(generator1);
    }
    // 批号
    for (int i = 0; i < 20; ++i) {
        InitTargetInfo[i].sTargetId = 'A' + i;
    }

    Radar *radar = new Radar(&window);
    // 初始化雷达界面数据
    radar->Init();
    // 设置是否显示航迹线，即目标移动轨迹
    radar->SetShowTrack(true);
    // 设置扫描方式、扫描方向、扫描波束宽度（扇形扫描的宽度）、扇形扫描的起始角度、结束角度
    radar->SetScanParams(ClockWize,CircleScan,10,45,250);
    // 添加目标数据，程序根据目标的距离和方位自动计算并绘制坐标
    radar->SetData(InitTargetInfo);
    // 雷达扫描定时器启动
    radar->Start(100);

    // 该定时器模拟目标移动
    QTimer timer;
    // 设置定时器的间隔，单位为毫秒
    timer.setInterval(100); // 每隔1秒触发一次定时器

    // 连接定时器的timeout()信号到槽函数
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        // 定时器触发时执行的代码
        // 所有的距离+1，即目标距离圆心+1，直到超出雷达范围
        for (auto it = InitTargetInfo.begin(); it != InitTargetInfo.end();) {
            if(it->dDistance == 0)
            {
                it = InitTargetInfo.erase(it);
            }
            else
            {
                it->dDistance += 1;
                ++it;
            }
        }
        radar->SetData(InitTargetInfo);
    });

    // 启动定时器
    timer.start();
    // ----------------------------------------测试结束-------------------



    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(radar);

    // 创建QWidget并将布局设置到QWidget上
    QWidget *centralWidget = new QWidget(&window);
    centralWidget->setLayout(layout);

    // 设置centralWidget为QMainWindow的centralWidget
    window.setCentralWidget(centralWidget);

    // 显示主窗口
    window.show();

    return a.exec();
}
