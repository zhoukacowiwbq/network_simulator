#ifndef CIRCULARCONSOLE_H
#define CIRCULARCONSOLE_H

#include <QGraphicsView>
#include <QVector>
#include <QObject>
#include <QProcess>
class QGraphicsScene;
class ConsoleButton;
class CenterDisplay;

class CircularConsole : public QGraphicsView
{
    Q_OBJECT

public:
    explicit CircularConsole(QWidget *parent = nullptr);

signals:
    void clear();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupScene();
    void updateLayout();

    QGraphicsScene *m_scene;
    CenterDisplay *m_centerDisplay;
    QVector<ConsoleButton*> m_buttons;
    qreal m_innerRadius;
    qreal m_outerRadius;
    QProcess*   m_externalProcess = nullptr;

private slots:
    void onConsoleButtonClicked(ConsoleButton* btn);
};

#endif // CIRCULARCONSOLE_H
