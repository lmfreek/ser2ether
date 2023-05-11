
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort>
#include <QUdpSocket>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QGridLayout>

namespace Ui { class Widget; }

class Widget : public QWidget

{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);kj
    ~Widget();

    void openSerialPort();
    void closeSerialPort();
    void readSerialData();
    void writeSerialData(const QByteArray &data);
    void readUdpData();
    void writeUdpData(const QByteArray &data);
    void handleError(QSerialPort::SerialPortError error);

private:
    Ui::Widget *ui;

    void createSerialGroupBox();
    void createUdpGroupBox();
    void createLogGroupBox();
    void updateSerialPortInfo();
    void fillPortsParameters();
    void fillPortsInfo();
    void updateUdpPortInfo();
    void updateUdpClose();
    void processError(const QString &s);
    void processWarning(const QString &s);
    void processInfo(const QString &s);
    void initGui();

    QGroupBox *serialGroupBox;
    QLabel *serialPortLabel;
    QComboBox *serialPortComboBox;
    QLabel *baudRateLabel;
    QComboBox *baudRateComboBox;
    QLabel *dataBitsLabel;
    QComboBox *dataBitsComboBox;
    QLabel *parityLabel;
    QComboBox *parityComboBox;
    QLabel *stopBitsLabel;
    QComboBox *stopBitsComboBox;
    QLabel *flowControlLabel;
    QComboBox *flowControlComboBox;
    QPushButton *openSerialButton;
    QPushButton *closeSerialButton;

    QGroupBox *udpGroupBox;
    QLabel *udpPortLabel;
    QLineEdit *udpPortLineEdit;
    QLabel *udpLocalPortLabel;
    QLineEdit *udpLocalPortLineEdit;
    QLabel *destinationIpLabel;
    QLineEdit *destinationIpLineEdit;

    QGroupBox *logGroupBox;
    QTextEdit *logTextEdit;

    QSerialPort *serialPort;
    QUdpSocket *udpSocket;
    QByteArray serialData;
    QByteArray udpData;
};

#endif // WIDGET_H
