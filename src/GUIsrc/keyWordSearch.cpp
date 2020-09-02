#include "keyWordSearch.h"

keyWordSearch::keyWordSearch(QDialog *parent) : QDialog(parent) {
	ui.setupUi(this);
	connect(ui.close, SIGNAL(clicked(bool)), this, SLOT(close()));
	connect(ui.search, SIGNAL(clicked(bool)), this, SLOT(entered()));
	connect(ui.ans1, SIGNAL(clicked(bool)), this, SLOT(button1clicked()));
	connect(ui.ans2, SIGNAL(clicked(bool)), this, SLOT(button2clicked()));
	connect(ui.ans3, SIGNAL(clicked(bool)), this, SLOT(button3clicked()));
	connect(ui.ans4, SIGNAL(clicked(bool)), this, SLOT(button4clicked()));
	connect(ui.ans5, SIGNAL(clicked(bool)), this, SLOT(button5clicked()));
}

void keyWordSearch::entered() {
	saveTxt();
	QProcess::execute("./guiQuery.exe");	//阻塞调用
	getTxt();
}

void keyWordSearch::saveTxt() {
	QString filePath = "./query1.txt";
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::critical(NULL, "提示", "无法创建文件");
		return;
	}
	QTextStream out(&file);
	out << ui.lineEdit->text() << endl;
	file.close();
}

void keyWordSearch::getTxt() {
	QTextCodec *codec = QTextCodec::codecForName("GB2312");
	QString filePath = "./result1.txt";
	QFile file(filePath);
	
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(NULL, "提示", "无法打开结果文件");
		return;
	}
	
	QTextStream in(&file);
	in.setCodec(QTextCodec::codecForName("GB2312"));
	for (int i = 0; i < 5; ++i) {
		QByteArray str = file.readLine();
		QString	temp = codec->toUnicode(str);
		if (i == 0) {
			ui.ans1->setText(temp);
		}
		if (i == 1) {
			ui.ans2->setText(temp);
		}
		if (i == 2) {
			ui.ans3->setText(temp);
		}
		if (i == 3) {
			ui.ans4->setText(temp);
		}
		if (i == 4) {
			ui.ans5->setText(temp);
		}
	}
}

void keyWordSearch::button1clicked() {
	QString newsName = ui.ans1->text();
	int pos = newsName.indexOf(",");
	newsName = newsName.mid(0, pos);	//得到对应新闻的id
	QString local("./input/" + newsName + ".html");
	QDesktopServices::openUrl(QUrl::fromLocalFile(local));
}

void keyWordSearch::button2clicked() {
	QString newsName = ui.ans2->text();
	int pos = newsName.indexOf(",");
	newsName = newsName.mid(0, pos);	//得到对应新闻的id
	QString local("./input/" + newsName + ".html");
	QDesktopServices::openUrl(QUrl::fromLocalFile(local));
}

void keyWordSearch::button3clicked() {
	QString newsName = ui.ans3->text();
	int pos = newsName.indexOf(",");
	newsName = newsName.mid(0, pos);	//得到对应新闻的id
	QString local("./input/" + newsName + ".html");
	QDesktopServices::openUrl(QUrl::fromLocalFile(local));
}

void keyWordSearch::button4clicked() {
	QString newsName = ui.ans4->text();
	int pos = newsName.indexOf(",");
	newsName = newsName.mid(0, pos);	//得到对应新闻的id
	QString local("./input/" + newsName + ".html");
	QDesktopServices::openUrl(QUrl::fromLocalFile(local));
}

void keyWordSearch::button5clicked() {
	QString newsName = ui.ans5->text();
	int pos = newsName.indexOf(",");
	newsName = newsName.mid(0, pos);	//得到对应新闻的id
	QString local("./input/" + newsName + ".html");
	QDesktopServices::openUrl(QUrl::fromLocalFile(local));
}