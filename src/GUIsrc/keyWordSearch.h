#pragma once

#include <QWidget>
#include <QMessageBox>
#include <QProcess>
#include "ui_keyWordSearch.h"
#include <Qtcore/qtextstream.h>
#include <QtCore/qfile.h>
#include <QtCore/qiodevice.h>
#include <QTextCodec>
#include <fstream>
#include <qdesktopservices.h>
#include <QUrl>

class keyWordSearch : public QDialog
{
	Q_OBJECT

public:
	keyWordSearch(QDialog *parent = Q_NULLPTR);

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
	Ui::keyWordSearch ui;

};
