#include "nodeconfigdlg.h"

/// dlg/NodeConfigDialog.cpp
#include "TopologyTypes.h"  // make sure this header defines Interface and Node

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QHeaderView>
#include <QComboBox>

NodeConfigDialog::NodeConfigDialog(Node &node, QWidget *parent)
    : QDialog(parent)
    , m_node(node)
{
    setupUi();
}

void NodeConfigDialog::setupUi() {
    setWindowTitle(tr("配置节点"));
    setMinimumSize(500, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    auto *nameLabel = new QLabel(tr("节点名称:"), this);
    m_nameEdit = new QLineEdit(m_node.name, this);
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(m_nameEdit);

    auto *loopbackLabel = new QLabel(tr("回环地址:"), this);
    m_loopbackEdit = new QLineEdit(m_node.loopbackAddress, this);
    mainLayout->addWidget(loopbackLabel);
    mainLayout->addWidget(m_loopbackEdit);

    auto *interfacesLabel = new QLabel(tr("接口信息:"), this);
    mainLayout->addWidget(interfacesLabel);

    m_interfacesTable = new QTableWidget(this);
    m_interfacesTable->setColumnCount(4);
    m_interfacesTable->setHorizontalHeaderLabels({ tr("接口名称"), tr("IP地址"), tr("L2模式"), tr("VLAN IDs") });
    m_interfacesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_interfacesTable->setRowCount(m_node.interfaces.size());

    for (int i = 0; i < m_node.interfaces.size(); ++i) {
        const Interface &iface = m_node.interfaces.at(i);
        m_interfacesTable->setItem(i, 0, new QTableWidgetItem(iface.name));
        m_interfacesTable->setItem(i, 1, new QTableWidgetItem(iface.ipAddress));
        m_interfacesTable->setItem(i, 2, new QTableWidgetItem(iface.l2Mode));
        // L2 模式列：用下拉框替换
        QComboBox *cb = new QComboBox(this);
        cb->addItem(QString());         // 空字符串
        cb->addItem("access");
        cb->addItem("trunk");
        // 如果原来有值，就设定当前项
        int idx = cb->findText(iface.l2Mode, Qt::MatchFixedString);
        if (idx >= 0) cb->setCurrentIndex(idx);
        m_interfacesTable->setCellWidget(i, 2, cb);
        
        QString vlanStr;
        for (int j = 0; j < iface.vlanIDs.size(); ++j) {
            vlanStr += QString::number(iface.vlanIDs.at(j));
            if (j < iface.vlanIDs.size() - 1) vlanStr += ",";
        }
        m_interfacesTable->setItem(i, 3, new QTableWidgetItem(vlanStr));
    }
    mainLayout->addWidget(m_interfacesTable);

    auto *addInterfaceBtn = new QPushButton(tr("添加接口"), this);
    mainLayout->addWidget(addInterfaceBtn);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    auto *okBtn = new QPushButton(tr("确认"), this);
    auto *cancelBtn = new QPushButton(tr("取消"), this);
    buttonLayout->addWidget(okBtn);
    buttonLayout->addWidget(cancelBtn);
    mainLayout->addLayout(buttonLayout);

    connect(addInterfaceBtn, &QPushButton::clicked, this, &NodeConfigDialog::addInterface);
    connect(okBtn, &QPushButton::clicked, this, &NodeConfigDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &NodeConfigDialog::reject);
}

void NodeConfigDialog::addInterface() {
    int row = m_interfacesTable->rowCount();
    m_interfacesTable->insertRow(row);
    m_interfacesTable->setItem(row, 0, new QTableWidgetItem(QString("eth%1").arg(row)));
    m_interfacesTable->setItem(row, 1, new QTableWidgetItem(QString()));
    QComboBox *cb = new QComboBox(this);
    cb->addItem(QString());
    cb->addItem("access");
    cb->addItem("trunk");
    cb->setCurrentIndex(0);  // 默认空
    m_interfacesTable->setCellWidget(row, 2, cb);
    m_interfacesTable->setItem(row, 3, new QTableWidgetItem(QString()));
}

void NodeConfigDialog::accept() {
    m_node.name = m_nameEdit->text();
    m_node.loopbackAddress = m_loopbackEdit->text();
    m_node.interfaces.clear();

    for (int i = 0; i < m_interfacesTable->rowCount(); ++i) {
        Interface iface;
        if (auto *item = m_interfacesTable->item(i, 0)) iface.name = item->text();
        if (auto *item = m_interfacesTable->item(i, 1)) iface.ipAddress = item->text();
        if (auto *cb = qobject_cast<QComboBox*>(m_interfacesTable->cellWidget(i, 2))) {
            iface.l2Mode = cb->currentText();
        }
        if (auto *item = m_interfacesTable->item(i, 3)) {
            for (const QString &v : item->text().split(",", Qt::SkipEmptyParts)) {
                iface.vlanIDs.append(v.toInt());
            }
        }
        m_node.interfaces.append(iface);
    }

    QDialog::accept();
}

