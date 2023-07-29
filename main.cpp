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

    // 测试
    // 可疑点的方位距离测试
    std::vector<TargetInfo> InitTargetInfo(30);

    // 生成60个100~300不重复的距离
    std::mt19937 generator(std::time(0));
    std::uniform_int_distribution<int> distribution(100, 300);
    for (int i = 0; i < 20; ++i) {
        InitTargetInfo[i].dDistance = distribution(generator);
    }
    //------------------------------------------------------------

    // 生成60个0~360不重复的方位
    std::mt19937 generator1(std::time(0));
    std::uniform_int_distribution<int> distribution1(0, 360);
    for (int i = 0; i < 20; ++i) {
        InitTargetInfo[i].dAzimuth = distribution1(generator1);
    }

    // 批号
    for (int i = 0; i < 20; ++i) {
        InitTargetInfo[i].sTargetId = 'A' + i;
    }

    QMainWindow window;

    Radar *radar = new Radar(&window);
    radar->Init();
    radar->SetShowTrack(true);
    radar->SetScanParams(ClockWize,CircleScan,10,45,250);
    radar->SetData(InitTargetInfo);
    radar->Start(100);

    QTimer timer;
    // 设置定时器的间隔，单位为毫秒
    timer.setInterval(100); // 每隔1秒触发一次定时器

    // 连接定时器的timeout()信号到槽函数
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        // 定时器触发时执行的代码
        // 所有的距离-5
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

    // 测试代码结束

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
