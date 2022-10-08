#pragma once
#include <QMainWindow>
#include <QLabel>

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);
private:
	QLabel* mainLabel;
};
