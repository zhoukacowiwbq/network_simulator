#ifndef LINKCONFIGDLG_H
#define LINKCONFIGDLG_H

#include <QDialog>
#include <QVector>
#include "TopologyTypes.h"    // Interface, Edge 定义

class QComboBox;
class QSpinBox;
class QDialogButtonBox;
class QLabel;

class LinkConfigDlg : public QDialog {
    Q_OBJECT

public:
    explicit LinkConfigDlg(QWidget *parent = nullptr);

    // 设置节点名称（将显示在标签上）
    void setNodeNames(const QString &srcName, const QString &dstName);

    // 接收两个节点的接口列表
    void setInterfaceLists(const QVector<Interface> &ifs1,
                           const QVector<Interface> &ifs2);

    // 取回用户选择
    QString interface1() const;
    QString interface2() const;
    int     cost()       const;

private:
    QLabel           *labelIf1;
    QLabel           *labelIf2;
    QComboBox        *comboIf1;
    QComboBox        *comboIf2;
    QSpinBox         *spinCost;
    QDialogButtonBox *buttonBox;
};

#endif // LINKCONFIGDLG_H
