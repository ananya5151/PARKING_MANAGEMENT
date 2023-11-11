#include "mainwindow.h"
#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QInputDialog>
#include <QRegularExpressionValidator>
#include <stack>

std::vector<ParkingSpot> parkingSpots(NUM_PARKING_SPOTS);
std::queue<int> availableSpots;
std::stack<int> parkedCarStack;

QGridLayout *gridLayout;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Parking Management System");
    setStyleSheet("background-color: #111111;color:#ffffff;");

    QString input = "color: #111111;background-color: white; margin-bottom: 15px;";
    QString label = "color: #ffffff; font-weight: bold;";
    QString comboBox = input;

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setFixedSize(450, 700);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QLabel *nameLabel = new QLabel("Name: ");
    nameLabel->setStyleSheet(label);
    nameLabel->setFont(QFont("Arial", 10, QFont::Bold));

    nameLineEdit = new QLineEdit(this);
    nameLineEdit->setStyleSheet(input);

    QLabel *modelLabel = new QLabel("Vehicle Model: ");
    modelLabel->setStyleSheet(label);
    modelLabel->setFont(QFont("Arial", 10, QFont::Bold));

    modelLineEdit = new QLineEdit(this);
    modelLineEdit->setStyleSheet(input);

    QLabel *numberLabel = new QLabel("Vehicle Number (e.g., AB1234): ");
    numberLabel->setStyleSheet(label);
    numberLabel->setFont(QFont("Arial", 10, QFont::Bold));

    numberLineEdit = new QLineEdit(this);
    numberLineEdit->setStyleSheet(input);

    QRegularExpression regExp("[A-Za-z0-9]*");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regExp, this);
    numberLineEdit->setValidator(validator);

    QLabel *timeLabel = new QLabel("Time (hours): ");
    timeLabel->setStyleSheet(label);
    timeLabel->setFont(QFont("Arial", 10, QFont::Bold));

    timeLineEdit = new QLineEdit(this);
    timeLineEdit->setStyleSheet(input);

    QLabel *floorLabel = new QLabel("Floor: ");
    floorLabel->setStyleSheet(label);
    floorLabel->setFont(QFont("Arial", 10, QFont::Bold));

    floorComboBox = new QComboBox(this);
    floorComboBox->setStyleSheet(comboBox);
    floorComboBox->setFont(QFont("Arial", 10, QFont::Bold));
    floorComboBox->addItem("Top Floor");
    floorComboBox->addItem("Ground Floor");
    floorComboBox->addItem("Basement");

    QLabel *vehicleTypeLabel = new QLabel("Vehicle Type: ");
    vehicleTypeLabel->setStyleSheet(label);
    vehicleTypeLabel->setFont(QFont("Arial", 10, QFont::Bold));

    vehicleTypeComboBox = new QComboBox(this);
    vehicleTypeComboBox->setStyleSheet(comboBox);
    vehicleTypeComboBox->setFont(QFont("Arial", 10, QFont::Bold));
    vehicleTypeComboBox->addItem("Two-Wheeler");
    vehicleTypeComboBox->addItem("Four-Wheeler");

    QHBoxLayout *boxLayout = new QHBoxLayout();

    parkButton = new QPushButton("Park Car", this);
    connect(parkButton, &QPushButton::clicked, this, &MainWindow::parkCar);
    parkButton->setStyleSheet("background-color: lightgreen;");
    parkButton->setFont(QFont("Arial", 10, QFont::Bold));

    removeButton = new QPushButton("Remove Car", this);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeCar);
    removeButton->setEnabled(false);
    removeButton->setStyleSheet("background-color: lightgreen;");
    removeButton->setFont(QFont("Arial", 10, QFont::Bold));

    exitButton = new QPushButton("Exit", this);
    connect(exitButton, &QPushButton::clicked, this, &MainWindow::exitApp);
    exitButton->setStyleSheet("background-color: red;");
    exitButton->setFont(QFont("Arial", 10, QFont::Bold));

    QLabel *availableLabel = new QLabel("Available Parking Spots:", this);
    availableLabel->setStyleSheet(label+"margin-top: 25px;");

    parkingListWidget = new QListWidget(this);
    parkingListWidget->setStyleSheet(input);

    layout->addWidget(nameLabel);
    layout->addWidget(nameLineEdit);

    layout->addWidget(modelLabel);
    layout->addWidget(modelLineEdit);

    layout->addWidget(numberLabel);
    layout->addWidget(numberLineEdit);

    layout->addWidget(timeLabel);
    layout->addWidget(timeLineEdit);

    layout->addWidget(floorLabel);
    layout->addWidget(floorComboBox);

    layout->addWidget(vehicleTypeLabel);
    layout->addWidget(vehicleTypeComboBox);

    boxLayout->addWidget(parkButton);
    boxLayout->addWidget(removeButton);
    boxLayout->addWidget(exitButton);
    layout->addLayout(boxLayout);

    layout->addWidget(availableLabel);
    layout->addWidget(parkingListWidget);


    // Randomly occupy 40 parking spots
    for (int i = 0; i < 40; i++) {
        int randomSpot = QRandomGenerator::global()->bounded(NUM_PARKING_SPOTS);
        parkingSpots[randomSpot].isOccupied = true;
        availableSpots.pop();
    }
    updateParkingList();
}

MainWindow::~MainWindow() {

}

void MainWindow::parkCar() {
    QString name = nameLineEdit->text();
    QString model = modelLineEdit->text();
    QString number = numberLineEdit->text();
    int hours = timeLineEdit->text().toInt();
    QString floor = floorComboBox->currentText();
    QString vehicleType = vehicleTypeComboBox->currentText();

    int rate = 0;

    if (vehicleType == "Two-Wheeler") {
        rate = 15; // Two-Wheeler parking rate
    } else if (vehicleType == "Four-Wheeler") {
        rate = 30; // Four-Wheeler parking rate
    } else {
        QMessageBox::critical(this, "Error", "Invalid vehicle type.");
        return;
    }

    if (!availableSpots.empty()) {
        int spotNumber = findAvailableSpot();
        if (spotNumber != -1) {
            availableSpots.pop();
            parkedCarStack.push(spotNumber);
        } else {
            QMessageBox::critical(this, "Error", "No available parking spots on the " + floor + " floor.");
            return;
        }
        parkingSpots[spotNumber].isOccupied = true;
        parkingSpots[spotNumber].vehicleName = name.toStdString();
        parkingSpots[spotNumber].vehicleModel = model.toStdString();
        parkingSpots[spotNumber].vehicleType = vehicleType.toStdString();
        parkingSpots[spotNumber].vehicleNumber = number.toStdString();
        parkingSpots[spotNumber].timeInHours = hours;

        int charges = hours * rate;

        QString message = "Car parked successfully in spot " + QString::number(spotNumber + 1) +
                          "\nCharges: Rs. " + QString::number(charges) +
                          "\nVehicle Model: " + parkingSpots[spotNumber].vehicleModel.c_str();

        QMessageBox::information(this, "Success", message);

        updateParkingList();
        removeButton->setEnabled(true);
    } else {
        QMessageBox::critical(this, "Error", "No available parking spots on the " + floor + " floor.");
    }
}

void MainWindow::removeCar() {
    bool ok;
    int spotNumber = QInputDialog::getInt(this, "Enter Parking Spot Number", "Spot Number:", 1, 1, NUM_PARKING_SPOTS, 1, &ok);

    if (ok) {
        if (spotNumber >= 1 && spotNumber <= NUM_PARKING_SPOTS && parkingSpots[spotNumber - 1].isOccupied) {
            parkingSpots[spotNumber - 1].isOccupied = false;

            // Calculate charges based on the time the car was parked
            int hoursParked = parkingSpots[spotNumber - 1].timeInHours;
            int rate = 0;

            if (parkingSpots[spotNumber - 1].vehicleType == "Two-Wheeler") {
                rate = TWO_WHEELER_RATE;
            } else if (parkingSpots[spotNumber - 1].vehicleType == "Four-Wheeler") {
                rate = FOUR_WHEELER_RATE;
            }

            int charges = hoursParked * rate;

            availableSpots.push(spotNumber - 1);

            QString message = "Car removed successfully. Vehicle Number: " +
                              QString::fromStdString(parkingSpots[spotNumber - 1].vehicleNumber) +
                              "\nYou may pay ₹" + QString::number(charges) +
                              " for " + QString::number(hoursParked) + " hours as a " +
                              QString::fromStdString(parkingSpots[spotNumber - 1].vehicleModel) + " user.";


            QMessageBox::information(this, "Success", message);
        } else {
            QMessageBox::critical(this, "Error", "Invalid parking spot or spot is empty.");
        }

        updateParkingList();
    }
}

void MainWindow::exitApp() {
    close();
}

void MainWindow::updateParkingList() {
    parkingListWidget->clear();

    for (int i = 0; i < NUM_PARKING_SPOTS; i++) {
        QString spotStatus = parkingSpots[i].isOccupied ? "UNAVAILABLE" : "AVAILABLE";
        QString spotInfo = "Spot " + QString::number(i + 1) + " (" + spotStatus + ")";
        parkingListWidget->addItem(spotInfo);
    }
}

int MainWindow::findAvailableSpot() {
    for (int i = 0; i < NUM_PARKING_SPOTS; i++) {
        if (!parkingSpots[i].isOccupied) {
            return i;
        }
    }
    return -1; // No available spots
}
