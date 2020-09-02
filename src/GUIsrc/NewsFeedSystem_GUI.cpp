#include "NewsFeedSystem_GUI.h"
#include "ui_NewsFeedSystem_GUI.h"
#include "keyWordSearch.h"
#include "ui_keyWordSearch.h"
#include "titleSearch.h"
#include "ui_titleSearch.h"

NewsFeedSystem_GUI::NewsFeedSystem_GUI(QDialog *parent) : QMainWindow(parent) {
	ui.setupUi(this);
	connect(ui.keyWord, SIGNAL(clicked(bool)), this, SLOT(useKeyWord()));
	connect(ui.title, SIGNAL(clicked(bool)), this, SLOT(useTitle()));
	connect(ui.quit, SIGNAL(clicked(bool)), this, SLOT(close()));
}

void NewsFeedSystem_GUI::useKeyWord() {
	auto kws = new keyWordSearch();
	kws->show();
}

void NewsFeedSystem_GUI::useTitle() {
	auto ts = new titleSearch();
	ts->show();
}

NewsFeedSystem_GUI::~NewsFeedSystem_GUI() {

}