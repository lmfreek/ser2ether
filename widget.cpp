
#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Initialize the GUI
    initGui();

    // Connect the signals and slots
    connect(openSerialButton, SIGNAL(clicked()), this, SLOT(openSerialPort()));
    connect(closeSerialButton, SIGNAL(clicked()), this, SLOT(closeSerialPort()));
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(readSerialData()));
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readUdpData()));
    connect(serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
}

Widget::~Widget()
{
    delete ui;
}

// This slot is called when the user clicks the "Open" button in the GUI
void Widget::openSerialPort()
{
    // If the serial port is already open, do nothing
    if (serialPort->isOpen())
        return;

    // Get the selected serial port name from the combo box
    QString portName = serialPortComboBox->currentText();

    // Set the serial port name
    serialPort->setPortName(portName);

    // Get the selected baud rate from the combo box
    qint32 baudRate = baudRateComboBox->currentText().toInt();

    // Set the baud rate
    serialPort->setBaudRate(baudRate);

    // Get the selected data bits from the combo box
    QSerialPort::DataBits dataBits = static_cast<QSerialPort::DataBits>(
        dataBitsComboBox->itemData(dataBitsComboBox->currentIndex()).toInt());

    // Set the data bits
    serialPort->setDataBits(dataBits);

    // Get the selected parity from the combo box
    QSerialPort::Parity parity = static_cast<QSerialPort::Parity>(
        parityComboBox->itemData(parityComboBox->currentIndex()).toInt());

    // Set the parity
    serialPort->setParity(parity);

    // Get the selected stop bits from the combo box
    QSerialPort::StopBits stopBits = static_cast<QSerialPort::StopBits>(
        stopBitsComboBox->itemData(stopBitsComboBox->currentIndex()).toInt());

    // Set the stop bits
    serialPort->setStopBits(stopBits);

    // Get the selected flow control from the combo box
    QSerialPort::FlowControl flowControl = static_cast<QSerialPort::FlowControl>(
        flowControlComboBox->itemData(flowControlComboBox->currentIndex()).toInt());

    // Set the flow control
    serialPort->setFlowControl(flowControl);

    // Try to open the serial port
    if (serialPort->open(QIODevice::ReadWrite)) {
        // If the serial port was successfully opened, update the GUI
        openSerialButton->setEnabled(false);
        closeSerialButton->setEnabled(true);
        serialPortComboBox->setEnabled(false);
        baudRateComboBox->setEnabled(false);
        dataBitsComboBox->setEnabled(false);
        parityComboBox->setEnabled(false);
        stopBitsComboBox->setEnabled(false);
        flowControlComboBox->setEnabled(false);
        udpLocalPortLineEdit->setEnabled(false);
        udpPortLineEdit->setEnabled(false);
        destinationIpLineEdit->setEnabled(false);
        processInfo(tr("Serial port %1 opened").arg(portName));
    } else {
        // If the serial port could not be opened, display an error message
        processError(tr("Failed to open serial port %1, error: %2")
                         .arg(portName).arg(serialPort->errorString()));
    }
}

// This slot is called when the user clicks the "Close" button in the GUI
void Widget::closeSerialPort()
{
    // If the serial port is not open, do nothing
    if (!serialPort->isOpen())
        return;

    // Close the serial port
    serialPort->close();

    // Update the GUI
    openSerialButton->setEnabled(true);
    closeSerialButton->setEnabled(false);
    serialPortComboBox->setEnabled(true);
    baudRateComboBox->setEnabled(true);
    dataBitsComboBox->setEnabled(true);
    parityComboBox->setEnabled(true);
    stopBitsComboBox->setEnabled(true);
    flowControlComboBox->setEnabled(true);
    udpLocalPortLineEdit->setEnabled(true);
    udpPortLineEdit->setEnabled(true);
    destinationIpLineEdit->setEnabled(true);
    processInfo(tr("Serial port closed"));
}

// This slot is called when data is available on the serial port
void Widget::readSerialData()
{
    // Read the data from the serial port
    serialData = serialPort->readAll();

    // Write the data to the UDP socket
    writeUdpData(serialData);
}

// This function is called to write data to the serial port
void Widget::writeSerialData(const QByteArray &data)
{
    // Write the data to the serial port
    serialPort->write(data);
}

// This slot is called when data is available on the UDP socket
void Widget::readUdpData()
{
    // Read the data from the UDP socket
    udpData.resize(udpSocket->pendingDatagramSize());
    udpSocket->readDatagram(udpData.data(), udpData.size());

    // Write the data to the serial port
    writeSerialData(udpData);
}

// This function is called to write data to the UDP socket
void Widget::writeUdpData(const QByteArray &data)
{
    // Get the destination IP address and port number from the GUI
    QString destinationIp = destinationIpLineEdit->text();
    quint16 udpPort = udpPortLineEdit->text().toUShort();

    // Set the destination IP address and port number for the UDP socket
    udpSocket->writeDatagram(data, QHostAddress(destinationIp), udpPort);
}

// This slot is called when there is an error on the serial port
void Widget::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        // If there is a resource error on the serial port, close it and update the GUI
        processError(tr("Serial port error: %1").arg(serialPort->errorString()));
        closeSerialPort();
    }
}

// This function is called to initialize the GUI
void Widget::initGui()
{
    // Create the serial port object
    serialPort = new QSerialPort(this);
    setWindowTitle("ser2ether 1.0.0");

    // Create the UDP socket object
    udpSocket = new QUdpSocket(this);

    // Create the serial port combo box
    serialPortComboBox = new QComboBox(this);

    // Add the available serial ports to the combo box
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        serialPortComboBox->addItem(serialPortInfo.portName());
    }

    // Create the baud rate combo box
    baudRateComboBox = new QComboBox(this);

    // Add the available baud rates to the combo box
    baudRateComboBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    baudRateComboBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    baudRateComboBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    baudRateComboBox->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    baudRateComboBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    baudRateComboBox->addItem(QStringLiteral("500000"), 500000);
    baudRateComboBox->addItem(QStringLiteral("1000000"), 1000000);
    baudRateComboBox->addItem(QStringLiteral("5000000"), 5000000);

    // Create the data bits combo box
    dataBitsComboBox = new QComboBox(this);

    // Add the available data bits to the combo box
    dataBitsComboBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    dataBitsComboBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    dataBitsComboBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    dataBitsComboBox->addItem(QStringLiteral("8"), QSerialPort::Data8);

    // Create the parity combo box
    parityComboBox = new QComboBox(this);

    // Add the available parity options to the combo box
    parityComboBox->addItem(tr("None"), QSerialPort::NoParity);
    parityComboBox->addItem(tr("Even"), QSerialPort::EvenParity);
    parityComboBox->addItem(tr("Odd"), QSerialPort::OddParity);
    parityComboBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    parityComboBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    // Create the stop bits combo box
    stopBitsComboBox = new QComboBox(this);

    // Add the available stop bits to the combo box
    stopBitsComboBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    stopBitsComboBox->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
    stopBitsComboBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    // Create the flow control combo box
    flowControlComboBox = new QComboBox(this);

    // Add the available flow control options to the combo box
    flowControlComboBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    flowControlComboBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    flowControlComboBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);

    // Create the open serial button
    openSerialButton = new QPushButton(tr("Open"), this);

    // Connect the open serial button to the openSerialPort() slot
    connect(openSerialButton, &QPushButton::clicked, this, &Widget::openSerialPort);
    connect(openSerialButton, &QPushButton::clicked, this, &Widget::updateUdpPortInfo);

    // Create the close serial button
    closeSerialButton = new QPushButton(tr("Close"), this);

    // Connect the close serial button to the closeSerialPort() slot
    connect(closeSerialButton, &QPushButton::clicked, this, &Widget::closeSerialPort);
    connect(closeSerialButton, &QPushButton::clicked, this, &Widget::updateUdpClose);

    // Disable the close serial button initially
    closeSerialButton->setEnabled(false);

    // Create the UDP group box
    udpGroupBox = new QGroupBox(tr("UDP"), this);

    // Create the UDP port label
    udpPortLabel = new QLabel(tr("target Port:"), this);

    // Create the UDP port line edit
    udpPortLineEdit = new QLineEdit(this);

    // Set the default UDP port number
    udpPortLineEdit->setText(QStringLiteral("1234"));

    // Create the local UDP port label
    udpLocalPortLabel = new QLabel(tr("local Port:"), this);

    // Create the UDP port line edit
    udpLocalPortLineEdit = new QLineEdit(this);

    // Set the default UDP port number
    udpLocalPortLineEdit->setText(QStringLiteral("1234"));

    // Create the destination IP label
    destinationIpLabel = new QLabel(tr("Destination IP:"), this);

    // Create the destination IP line edit
    destinationIpLineEdit = new QLineEdit(this);

    // Set the default destination IP address
    destinationIpLineEdit->setText(QStringLiteral("127.0.0.1"));

    // Create the log group box
    logGroupBox = new QGroupBox(tr("Log"), this);

    // Create the log text edit
    logTextEdit = new QTextEdit(this);

    // Set the log text edit to read-only
    logTextEdit->setReadOnly(true);

    // Create the main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create the serial port layout
    QGridLayout *serialPortLayout = new QGridLayout();
    serialPortLayout->addWidget(serialPortComboBox, 0, 0);
    serialPortLayout->addWidget(baudRateComboBox, 0, 1);
    serialPortLayout->addWidget(dataBitsComboBox, 0, 2);
    serialPortLayout->addWidget(parityComboBox, 0, 3);
    serialPortLayout->addWidget(stopBitsComboBox, 0, 4);
    serialPortLayout->addWidget(flowControlComboBox, 0, 5);
    serialPortLayout->addWidget(openSerialButton, 0, 6);
    serialPortLayout->addWidget(closeSerialButton, 0, 7);
    mainLayout->addLayout(serialPortLayout);

    // Create the UDP layout
    QGridLayout *udpLayout = new QGridLayout();
    udpLayout->addWidget(udpLocalPortLabel, 0, 0);
    udpLayout->addWidget(udpLocalPortLineEdit, 0, 1);
    udpLayout->addWidget(udpPortLabel, 0, 2);
    udpLayout->addWidget(udpPortLineEdit, 0, 3);
    udpLayout->addWidget(destinationIpLabel, 0, 4);
    udpLayout->addWidget(destinationIpLineEdit, 0, 5);
    udpGroupBox->setLayout(udpLayout);
    mainLayout->addWidget(udpGroupBox);

    // Create the log layout
    QVBoxLayout *logLayout = new QVBoxLayout();
    logLayout->addWidget(logTextEdit);
    logGroupBox->setLayout(logLayout);
    mainLayout->addWidget(logGroupBox);

    // Set the main layout
    setLayout(mainLayout);

    // Connect the serial port to the readSerialData() slot
    connect(serialPort, &QSerialPort::readyRead, this, &Widget::readSerialData);

    // Connect the UDP socket to the readUdpData() slot
    connect(udpSocket, &QUdpSocket::readyRead, this, &Widget::readUdpData);

    // Connect the serial port to the handleError() slot
    connect(serialPort, &QSerialPort::errorOccurred, this, &Widget::handleError);
}

// Function to create the serial port group box
void Widget::createSerialGroupBox()
{
    // Create the serial port combo box
    serialPortComboBox = new QComboBox(this);

    // Fill the available serial ports in the combo box
    fillPortsInfo();

    // Fill the available serial port parameters in the combo box
    fillPortsParameters();

    // Create the serial port group box
    serialGroupBox = new QGroupBox(tr("Serial Port"), this);

    // Create the layout for the serial port group box
    QGridLayout *serialLayout = new QGridLayout();
    serialLayout->addWidget(serialPortComboBox, 0, 0);
    serialLayout->addWidget(baudRateComboBox, 0, 1);
    serialLayout->addWidget(dataBitsComboBox, 0, 2);
    serialLayout->addWidget(parityComboBox, 0, 3);
    serialLayout->addWidget(stopBitsComboBox, 0, 4);
    serialLayout->addWidget(flowControlComboBox, 0, 5);
    serialLayout->addWidget(openSerialButton, 0, 6);
    serialLayout->addWidget(closeSerialButton, 0, 7);
    serialGroupBox->setLayout(serialLayout);
}

// Function to create the UDP group box
void Widget::createUdpGroupBox()
{
    // Create the UDP group box
    udpGroupBox = new QGroupBox(tr("UDP"), this);

    // Create the UDP port label
    udpPortLabel = new QLabel(tr("Port:"), this);

    // Create the UDP port line edit
    udpPortLineEdit = new QLineEdit(this);

    // Set the default UDP port number
    udpPortLineEdit->setText(QStringLiteral("1234"));

    // Create the destination IP label
    destinationIpLabel = new QLabel(tr("Destination IP:"), this);

    // Create the destination IP line edit
    destinationIpLineEdit = new QLineEdit(this);

    // Set the default destination IP address
    destinationIpLineEdit->setText(QStringLiteral("127.0.0.1"));

    // Create the layout for the UDP group box
    QGridLayout *udpLayout = new QGridLayout();
    udpLayout->addWidget(udpPortLabel, 0, 0);
    udpLayout->addWidget(udpPortLineEdit, 0, 1);
    udpLayout->addWidget(destinationIpLabel, 0, 2);
    udpLayout->addWidget(destinationIpLineEdit, 0, 3);
    udpGroupBox->setLayout(udpLayout);
}

// Function to create the log group box
void Widget::createLogGroupBox()
{
    // Create the log group box
    logGroupBox = new QGroupBox(tr("Log"), this);

    // Create the log text edit
    logTextEdit = new QTextEdit(this);

    // Set the log text edit to read-only
    logTextEdit->setReadOnly(true);

    // Create the layout for the log group box
    QVBoxLayout *logLayout = new QVBoxLayout();
    logLayout->addWidget(logTextEdit);
    logGroupBox->setLayout(logLayout);
}

// Function to update the available serial ports
void Widget::updateSerialPortInfo()
{
    // Clear the current serial port combo box
    serialPortComboBox->clear();

    // Fill the available serial ports in the combo box
    fillPortsInfo();
}

// Function to fill the available serial port parameters in the combo box
void Widget::fillPortsParameters()
{
    // Add the available baud rates to the combo box
    baudRateComboBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    baudRateComboBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    baudRateComboBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    baudRateComboBox->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    baudRateComboBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    // Add the available data bits to the combo box
    dataBitsComboBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    dataBitsComboBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    dataBitsComboBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    dataBitsComboBox->addItem(QStringLiteral("8"), QSerialPort::Data8);

    // Add the available parity options to the combo box
    parityComboBox->addItem(tr("None"), QSerialPort::NoParity);
    parityComboBox->addItem(tr("Even"), QSerialPort::EvenParity);
    parityComboBox->addItem(tr("Odd"), QSerialPort::OddParity);
    parityComboBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    parityComboBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    // Add the available stop bits to the combo box
    stopBitsComboBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    stopBitsComboBox->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
    stopBitsComboBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    // Add the available flow control options to the combo box
    flowControlComboBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    flowControlComboBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    flowControlComboBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

// Function to fill the available serial ports in the combo box
void Widget::fillPortsInfo()
{
    // Clear the current serial port combo box
    serialPortComboBox->clear();

    // Get the available serial ports
    const auto serialPortInfos = QSerialPortInfo::availablePorts();

    // Add the available serial ports to the combo box
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos)
        serialPortComboBox->addItem(serialPortInfo.portName());
}

// Function to update the UDP port information
void Widget::updateUdpPortInfo()
{
    // Get the current UDP port number
    const quint16 udpLocalPort = udpLocalPortLineEdit->text().toInt();

    // Set the UDP socket to listen on the current port
    udpSocket->bind(QHostAddress::AnyIPv4, udpLocalPort);
}

// Function to update the UDP port information
void Widget::updateUdpClose()
{
    // Get the current UDP port number
    // Set the UDP socket to listen on the current port
    udpSocket->close();
}

// Function to process information messages
void Widget::processInfo(const QString &info)
{
    // Append the information message to the log text edit
    logTextEdit->append(tr("[INFO] %1").arg(info));
}

// Function to process error messages
void Widget::processError(const QString &error)
{
    // Append the error message to the log text edit
    logTextEdit->append(tr("[ERROR] %1").arg(error));
}
