#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NewsFeedSystem_GUI.h"

class NewsFeedSystem_GUI : public QMainWindow
{
	Q_OBJECT

public:
	NewsFeedSystem_GUI(QDialog *parent = Q_NULLPTR);
	~NewsFeedSystem_GUI();

public slots:
	void useKeyWord();
	void useTitle();

private:
	Ui::NewsFeedSystem_GUIClass ui;
};
