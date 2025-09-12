#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include "linkconfigdlg.h"

LinkConfigDlg::LinkConfigDlg(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("配置链路"));

    // 动态标签
    labelIf1 = new QLabel(this);
    labelIf2 = new QLabel(this);

    comboIf1 = new QComboBox(this);
    comboIf2 = new QComboBox(this);
    spinCost = new QSpinBox(this);
    spinCost->setRange(1, 100000);
    spinCost->setValue(1);

    buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &LinkConfigDlg::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &LinkConfigDlg::reject);

    // 布局
    auto *form = new QFormLayout;
    form->addRow(labelIf1, comboIf1);
    form->addRow(labelIf2, comboIf2);
    form->addRow(new QLabel(tr("代价 (cost)："), this), spinCost);

    auto *lay = new QVBoxLayout(this);
    lay->addLayout(form);
    lay->addWidget(buttonBox);
}

void LinkConfigDlg::setNodeNames(const QString &srcName, const QString &dstName)
{
    // 在标签上显示节点名称
    labelIf1->setText(tr("%1 的接口：").arg(srcName));
    labelIf2->setText(tr("%1 的接口：").arg(dstName));
}

void LinkConfigDlg::setInterfaceLists(const QVector<Interface> &ifs1,
                                      const QVector<Interface> &ifs2)
{
    comboIf1->clear();
    for (const auto &intf : ifs1)
        comboIf1->addItem(intf.name);

    comboIf2->clear();
    for (const auto &intf : ifs2)
        comboIf2->addItem(intf.name);
}

QString LinkConfigDlg::interface1() const
{
    return comboIf1->currentText();
}

QString LinkConfigDlg::interface2() const
{
    return comboIf2->currentText();
}

int LinkConfigDlg::cost() const
{
    return spinCost->value();
}

