#pragma once

#include <QDialog>
#include "ui_titleSearch.h"
#include <QWidget>
#include <QMessageBox>
#include <QProcess>
#include <Qtcore/qtextstream.h>
#include <QtCore/qfile.h>
#include <QtCore/qiodevice.h>
#include <QTextCodec>
#include <fstream>
#include <qdesktopservices.h>
#include <QUrl>

class titleSearch : public QDialog
{
	Q_OBJECT

public:
	titleSearch(QDialog *parent = Q_NULLPTR);
	~titleSearch();

	void saveTxt();
	void getTxt();

public slots:
	void entered();
	void button1clicked();
	void button2clicked();
	void button3clicked();
	void button4clicked();
	void button5clicked();

private:
	Ui::titleSearch ui;

};
