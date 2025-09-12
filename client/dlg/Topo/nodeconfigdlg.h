/// dlg/NodeConfigDialog.h
#ifndef NODECONFIGDIALOG_H
#define NODECONFIGDIALOG_H

#include <QDialog>
#include <QVector>
#include "TopologyTypes.h"

struct Interface;  // forward declaration; defined in your shared model header
struct Node;       // forward declaration; defined in your shared model header

class QLineEdit;
class QTableWidget;

class NodeConfigDialog : public QDialog {
    Q_OBJECT
public:
    explicit NodeConfigDialog(Node &node, QWidget *parent = nullptr);
    ~NodeConfigDialog() override = default;

private slots:
    void addInterface();
    void accept() override;

private:
    void setupUi();

    Node &m_node;
    QLineEdit *m_nameEdit;
    QLineEdit *m_loopbackEdit;
    QTableWidget *m_interfacesTable;
};

#endif // NODECONFIGDIALOG_H



