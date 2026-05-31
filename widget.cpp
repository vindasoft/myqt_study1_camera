#include "widget.h"
#include "./ui_widget.h"

#include <QCamera>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QImageCapture>
#include <QFileDialog>
#include <QList>
#include <QDebug>
#include <QMediaRecorder>
#include <QCameraFormat>
#include <QStandardPaths>
#include <QMediaFormat>
#include <QAudioInput>
#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , captureSession(new QMediaCaptureSession(this))
{
    ui->setupUi(this);

    // 设置窗口名称及大小
    this->setWindowTitle("Camera");
    this->setFixedSize(650, 550);

    // 获取所有相机列表
    cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        qDebug() << "No cameras found!";
        return;
    }

    // 添加初始化的相机选项：默认选中第0位相机
    for (const auto &item : std::as_const(cameras)) {
        ui->cameraSel->addItem(item.description());
    }
    // 设置默认选中的相机序号：第0位相机
    ui->cameraSel->setCurrentIndex(0);

    // 初始化相机设备
    initializeCamera(0);

    //初始化录像存储模式
    initializeRecordStorageMode(0);

    // 连接信号槽
    // 相机列表
    connect(ui->cameraSel, &QComboBox::currentIndexChanged,
            this, &Widget::onCameraChanged);
    // 相机分辨率列表
    connect(ui->resolutionSel, &QComboBox::currentIndexChanged, this, [this](int idx){
        if (idx >= 0 && idx < mFormats.size()) {
            onCameraResolutionChanged(idx);
        }
    });
    // 开始录像
    connect(ui->startVideoRecordBut, &QPushButton::clicked, this, &Widget::onStartRecord);
    // 暂停录像
    connect(ui->pauseBut, &QPushButton::clicked, this, [this](){
        if (mediaRecorder && mediaRecorder->recorderState() == QMediaRecorder::RecordingState)
            mediaRecorder->pause();
        qDebug() << "Recording pause.";
    });
    // 继续录像
    connect(ui->continueBut, &QPushButton::clicked, this, [this](){
        if (mediaRecorder && mediaRecorder->recorderState() == QMediaRecorder::PausedState)
            mediaRecorder->record();
        qDebug() << "Recording continue.";
    });
    // 停止录像
    connect(ui->stopBut, &QPushButton::clicked, this, [this](){
        if (mediaRecorder && mediaRecorder->recorderState() == QMediaRecorder::RecordingState)
            mediaRecorder->stop();
        qDebug() << "Recording stoped.";
    });

    // 4. 可选：连接信号以监控录像状态和错误
    connect(mediaRecorder, &QMediaRecorder::errorOccurred, this, [](QMediaRecorder::Error error, const QString &errorString){
        qDebug() << "Recording Error:" << errorString;
    });
    connect(mediaRecorder, &QMediaRecorder::recorderStateChanged, this, [](QMediaRecorder::RecorderState state){
        qDebug() << "Recorder State Changed:" << state;
    });
}



// 初始化选中的相机
void Widget::initializeCamera(int index) {
    if (mediaRecorder && mediaRecorder->recorderState() == QMediaRecorder::RecordingState) {
        mediaRecorder->stop();
    }

    // 如果指定的序号大于相机类型下拉框列表长度，直接返回
    if (index >= cameras.size()) return;

    // 清理旧相机
    if (camera) {
        camera->stop();
        delete camera;
        camera = nullptr;
    }

    // 创建新相机对象：根据选择的相机设备类型来创建新相机对象
    QCameraDevice cameraDevice = cameras.at(index);
    qDebug() << "Selected camera:" << cameraDevice.description();
    camera = new QCamera(cameraDevice, this);

    // 设置媒体捕获会话
    captureSession->setCamera(camera);


    // 初始化录像器
    if (mediaRecorder) {
        delete mediaRecorder;
    }
    mediaRecorder = new QMediaRecorder(this);
    captureSession->setRecorder(mediaRecorder);
    // 配置录像器的媒体格式
    QMediaFormat format;
    // 设置容器格式为 MP4，视频编码为 H.264，音频编码为 AAC
    format.setFileFormat(QMediaFormat::MPEG4);
    format.setVideoCodec(QMediaFormat::VideoCodec::H264);
    format.setAudioCodec(QMediaFormat::AudioCodec::AAC);
    if (!format.isSupported(QMediaFormat::Encode)) {
        qWarning() << "H.264+AAC+MP4 not supported, fallback to default.";
        format = QMediaFormat(); // 默认格式
    }
    mediaRecorder->setMediaFormat(format);


    // 添加音频输入
    auto *audioInput = new QAudioInput(this);
    captureSession->setAudioInput(audioInput);


    // 获取相机硬件支持的所有视频格式
    mFormats = cameraDevice.videoFormats();
    removeRepeatedCameraFormats();
    qDebug() << "*******************mFormats.size="<< mFormats.size();

    // 先清空相机分辨率下拉框选项内容，再基于mFormats列表动态填充分辨率下拉框选项内容
    ui->resolutionSel->clear();
    for (const auto &mformat : std::as_const(mFormats)) {
        // 遍历提取分辨率选项
        QSize res = mformat.resolution();
        // qDebug() << res; //摄像头支持分辨率打印
        ui->resolutionSel->addItem(QString("%1*%2").arg(res.width()).arg(res.height()));
    }
    // 默认选中第1条分辨率选项
    ui->resolutionSel->setCurrentIndex(0);

    // 设置取景器：即设置视频输出
    if (!viewfinder) {
        viewfinder = new QVideoWidget(this);
        viewfinder->resize(600, 420);
        viewfinder->move(25, 20);
    }
    captureSession->setVideoOutput(viewfinder);
    viewfinder->show();

    // 初始化图像捕获
    if (imageCapture) {
        delete imageCapture;
    }
    imageCapture = new QImageCapture(this);
    captureSession->setImageCapture(imageCapture);

    // 检查相机状态并启动
    connect(camera, &QCamera::errorOccurred, [=](QCamera::Error error, const QString &errorString) {
        qDebug() << "Camera error:" << errorString;
    });

    camera->start();

    if (camera->isActive()) {
        qDebug() << "Camera started successfully";
    } else {
        qDebug() << "Failed to start camera";
    }

    // 截图并存储图片至本地选择的目录下
    connect(ui->screenShotBut, &QPushButton::clicked, [=](){
        // 初始化图像捕获
        if (imageCapture) {
            delete imageCapture;
        }
        imageCapture = new QImageCapture(this);
        captureSession->setImageCapture(imageCapture);

        QString fileName = QFileDialog::getSaveFileName();
        imageCapture->captureToFile(fileName);
    });
}

//初始化录像内容的存储模式
void Widget::initializeRecordStorageMode(int index) {
    //添加新相机的录像内容的存储模式
    storageModes.clear();
    storageModes = {"增量添加","循环覆盖"};
    for (const auto &storagemode : std::as_const(storageModes)){
        qDebug() << storagemode; //摄像头支持录像内容存储模式打印
        ui->storageModeSel->addItem(storagemode);
    }
    ui->storageModeSel->setCurrentIndex(index);
}

// 切换相机的分辨率
void Widget::setCameraResolution(const QSize &resolution) {
    qDebug() << "***************setCameraResolution selected resolution:" << resolution;
    if (!camera || !camera->isAvailable() || mFormats.isEmpty()) return;

    // 停止相机（可选，某些相机支持热切换，但为确保稳定建议停止）
    bool wasActive = camera->isActive();
    if (wasActive) {
        camera->stop();
    }

    // 定义一个目标分辨率
    QCameraFormat targetFormat;
    // 遍历相机支持的格式，寻找一个与目标分辨率匹配的格式
    for (const auto &mformat : std::as_const(mFormats)) {
        if (mformat.resolution() == resolution) {
            targetFormat = mformat;
            break;
        }
    }

    if (targetFormat.isNull()) {
        // 如果找不到完全匹配的分辨率，可以启用备选逻辑，比如找最接近的。
        // 为简化代码，这里仅输出一条警告。
        qDebug() << "警告: 当前选中的分辨率" << resolution
                 << "相机不支持，请使用默认的分辨率.";
        return;
    }

    // 将选中的硬件格式应用到相机
    camera->setCameraFormat(targetFormat);
    qDebug() << "***************setCameraResolution selected targetFormat resolution:" << targetFormat.resolution();

    // 如果相机已停止，再启动起来
    if (wasActive) {
        qDebug() << "***************setCameraResolution camera wasActive!";
        camera->start();
    }
}

// 去掉重复的相机格式对象
void Widget::removeRepeatedCameraFormats() {
    QList<QCameraFormat> sourceList = mFormats;
    QList<QCameraFormat> resultList;

    // 你的“相等”逻辑在这里定义
    QSet<QString> seen; // 或使用 QSet<QCameraFormat>，但必须重载其 operator==() 和 qHash()

    for (const QCameraFormat& format : sourceList) {
        // 生成一个唯一的键（key），用来判断两个格式是否“相同”
        QString key = QString("%1x%2")
                          .arg(format.resolution().width())
                          .arg(format.resolution().height());
        if (!seen.contains(key)) {
            seen.insert(key);
            resultList.append(format);
        }
    }
    mFormats = resultList;
}


// 辅助函数：生成录像文件的完整路径，实现“增量添加”和“循环覆盖”两种模式
QString Widget::generateRecordingFilePath(const QString &baseDir, const QString &mode) {
    QDir dir(baseDir);
    if (!dir.exists()) dir.mkpath(".");

    if (mode == "循环覆盖") {
        return dir.absoluteFilePath("record_loop.mp4");
    } else { // 默认是"增量添加"
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz");
        return dir.absoluteFilePath(QString("record_%1.mp4").arg(timestamp));
    }
}

void Widget::onStartRecord() {
    qDebug() << "onStartRecord Record start ******************";
    if (mediaRecorder && mediaRecorder->recorderState() == QMediaRecorder::RecordingState) {
        mediaRecorder->stop();
    }

    // initializeCamera( ui->cameraSel->currentIndex());
    if (!mediaRecorder || !camera || !camera->isActive()) {
        qDebug() << "----------------mediaRecorder or camera is not initialized.";
        return;
    }

    // 选择保存目录
    QString saveDir = QFileDialog::getExistingDirectory(this, "选择录像保存目录");
    if (saveDir.isEmpty()) return;

    // 1. 根据存储模式生成不同的文件名
    QString storageMode = ui->storageModeSel->currentText(); // 获取下拉框中的文本
    QString filePath = generateRecordingFilePath(saveDir, storageMode);

    // 2. 设置输出路径并开始录制
    mediaRecorder->setOutputLocation(QUrl::fromLocalFile(filePath));
    mediaRecorder->record();
    qDebug() << "Recording started, file:" << filePath;
}

// 切换相机
void Widget::onCameraChanged(int index) {
    initializeCamera(index);
}

void Widget::onCameraResolutionChanged(int index) {
    setCameraResolution(mFormats[index].resolution());
}

void Widget::onStorageModeChanged(int index) {
    initializeRecordStorageMode(index);
}

Widget::~Widget()
{
    if (mediaRecorder && mediaRecorder->recorderState() == QMediaRecorder::RecordingState) {
        mediaRecorder->stop();
        // 可选：等待几毫秒让文件写入完成
        QEventLoop loop;
        QTimer::singleShot(200, &loop, &QEventLoop::quit);
        loop.exec();
    }

    if (camera) {
        camera->stop();
    }
    delete ui;
}
