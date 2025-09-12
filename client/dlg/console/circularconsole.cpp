#include "circularconsole.h"
#include "consolebutton.h"
#include "centerdisplay.h"
#include "Constants.h"
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QtMath>

CircularConsole::CircularConsole(QWidget *parent) : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    setupScene();
}

void CircularConsole::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    updateLayout();
}

void CircularConsole::onConsoleButtonClicked(ConsoleButton* btn) {
    QString name = btn->buttonText();
    if (name == "Clear") {
        // TODO: 调用开始捕包的逻辑
        emit clear();
    }
    else if (name == "Load") {

    }
    else if (name == "Save") {

    }
    else if (name == "Start") {
        if (!m_externalProcess) {
            // 程序路径
            QString program = "/home/zhou/sourcecode/tcpip_stack/tcpstack.exe";
            // 如果有命令行参数就放到下面
            QStringList args;
            args << "--" << "bash" << "-i" << "-c"
                 << QString("%1; exec bash").arg(program);


            m_externalProcess = new QProcess(this);
            m_externalProcess->setWorkingDirectory("/home/zhou/sourcecode/tcpip_stack");
            m_externalProcess->start("gnome-terminal", args);
            if (!m_externalProcess->waitForStarted()) {
                qWarning() << "启动失败：" << m_externalProcess->errorString();
                delete m_externalProcess;
                m_externalProcess = nullptr;
            }
        }
    }
    else if (name == "Stop") {
        if (m_externalProcess) {
            // 先试优雅退出
            m_externalProcess->terminate();
            if (!m_externalProcess->waitForFinished(3000)) {
                // 超时就强制 kill
                m_externalProcess->kill();
                m_externalProcess->waitForFinished();
            }
            delete m_externalProcess;
            m_externalProcess = nullptr;
        }
    }
    
}

void CircularConsole::setupScene()
{
    m_centerDisplay = new CenterDisplay(100);  // Radius will be updated in updateLayout
    m_scene->addItem(m_centerDisplay);

    QStringList buttonNames = {
    "Start", "Stop", "Restart", "Save",
    "Load", "Clear", "", ""
    };

    for (int i = 0; i < Constants::BUTTON_COUNT; ++i) {
        ConsoleButton *button = new ConsoleButton(0, 0, 0, 0);  // Values will be updated in updateLayout
        if (i < buttonNames.size())  
            button->setButtonText(buttonNames[i]);

        m_scene->addItem(button);
        m_buttons.append(button);

        connect(button, &ConsoleButton::clicked,
            this, &CircularConsole::onConsoleButtonClicked);
    }

    updateLayout();
}

void CircularConsole::updateLayout()
{
    qreal size = qMin(width(), height());
    setSceneRect(-size/2, -size/2, size, size);

    m_outerRadius = size * Constants::OUTER_RADIUS_FACTOR;
    m_innerRadius = size * Constants::INNER_RADIUS_FACTOR;

    m_centerDisplay->setPos(0, 0);
    m_centerDisplay->setRadius(m_innerRadius * Constants::CENTER_DISPLAY_FACTOR);

    qreal totalAngle = 2 * M_PI;
    qreal buttonAngle = totalAngle / Constants::BUTTON_COUNT;
    qreal startAngle = -M_PI / 2 - buttonAngle / 2;

    for (int i = 0; i < Constants::BUTTON_COUNT; ++i) {
        qreal angle = startAngle + i * buttonAngle;
        qreal spanAngle = buttonAngle * (1 - Constants::BUTTON_SPACING);

        m_buttons[i]->setPos(0, 0);
        m_buttons[i]->setAngles(angle, spanAngle);
        m_buttons[i]->setRadii(m_innerRadius, m_outerRadius);

        qDebug() << "Button" << i << ": Angle =" << angle << "SpanAngle =" << spanAngle
                 << "InnerRadius =" << m_innerRadius << "OuterRadius =" << m_outerRadius;
    }

    // 确保场景更新
    m_scene->update();
}
