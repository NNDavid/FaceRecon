#include "MainWindow.h"
#include <QVBoxLayout>
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	auto* mainLayout = new QVBoxLayout(this);
	mainLabel = new QLabel("Hello World!", this);
	mainLayout->addWidget(mainLabel);
	this->setLayout(mainLayout);
}