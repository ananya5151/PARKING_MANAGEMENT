#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <vector>
#include <queue>

const int NUM_PARKING_SPOTS = 100;
const int TWO_WHEELER_RATE = 15;
const int FOUR_WHEELER_RATE = 30;

struct ParkingSpot {
    bool isOccupied;
    std::string vehicleName;
    std::string vehicleModel;
    std::string vehicleNumber;
    std::string vehicleType;
    int timeInHours;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void parkCar();
    void removeCar();
    void exitApp();

private:
    QLineEdit *nameLineEdit;
    QLineEdit *modelLineEdit;
    QLineEdit *numberLineEdit;
    QLineEdit *timeLineEdit;
    QComboBox *floorComboBox;
    QComboBox *vehicleTypeComboBox;
    QPushButton *parkButton;
    QPushButton *removeButton;
    QPushButton *exitButton;
    QListWidget *parkingListWidget;

    void updateParkingList();
    int findAvailableSpot();
};

#endif // MAINWINDOW_H
