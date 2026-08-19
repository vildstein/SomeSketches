#include "mainwindow.h"

#include <QDockWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>

#include <QDir>
#include <QFileSystemModel>
#include <QTreeView>

//#include <QTreeWidget>
//#include <QTreeWidgetItem>
//#include <QTableView>


#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{

	auto leftDock = createLeftDockWidget();
	addDockWidget(Qt::LeftDockWidgetArea, leftDock);

	m_tableWidget = createTableWidget();
	setCentralWidget(m_tableWidget);

	createToolBar();
	createStatusBar();

	showHomeDir();

	//connect(m_fileSystemModel, &QFileSystemModel::directoryLoaded, this, &MainWindow::onDirLoaded);
}

void MainWindow::onDirLoaded(const QString& dir)
{
	qInfo() << dir;

	QDir currentDir(dir);
	auto dirList = currentDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

	if (currentDir.cdUp()) {
		m_upDir = currentDir.absolutePath();
	}

	qInfo() << dirList;

	int row{0};
	int column{0};

	m_tableWidget->insertRow(row);
	m_tableWidget->insertColumn(column);

	for (auto it = dirList.cbegin(); it != dirList.cend(); ++it) {

		if (it->isFile()) {
			++m_filesCount;
		}

		if (it->isDir() && !(it->isRelative()) ) {
			++m_foldersCount;
		}

		auto itemName = it->completeBaseName();
		auto widItem = new QTableWidgetItem(itemName);
		m_tableWidget->setItem(row, column, widItem);

		++row;
		m_tableWidget->insertRow(row);


		//++column;
		//if (column > 4) {
		//	++row;
		//	m_tableWidget->insertRow(row);
		//	column = 0;
		//}
	}

	updateStatusBar();
}

void MainWindow::showHomeDir()
{
	onDirLoaded(QDir::homePath());

	auto homeIndex = m_fileSystemModel->index(QDir::homePath());

	if (homeIndex.isValid()) {
		m_treeView->expand(homeIndex);
		m_treeView->scrollTo(homeIndex);
		m_treeView->resizeColumnToContents(0);
	}

}

void MainWindow::goUpDir()
{
	auto upDirIndex = m_fileSystemModel->index(m_upDir);

	if (upDirIndex.isValid()) {
		m_treeView->expand(upDirIndex);
		m_treeView->scrollTo(upDirIndex);
		m_treeView->resizeColumnToContents(0);
	}

	m_tableWidget->clear();

	onDirLoaded(m_upDir);
}

MainWindow::~MainWindow() = default;

QDockWidget* MainWindow::createLeftDockWidget()
{
	m_treeView = new QTreeView;

	m_fileSystemModel = new QFileSystemModel;

	m_treeView->setModel(m_fileSystemModel);
	m_treeView->setColumnHidden(1, true);
	m_treeView->setColumnHidden(2, true);
	m_treeView->setColumnHidden(3, true);
	m_fileSystemModel->setRootPath(QDir::homePath());

	m_treeView->setHeaderHidden(true);

	//auto homeIndex = m_fileSystemModel->index(QDir::homePath());

	//if (homeIndex.isValid()) {
	//	treeView->expand(homeIndex);
	//	treeView->scrollTo(homeIndex);
	//}

	auto dockWidget = new QDockWidget(this);
	dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);

	dockWidget->setWidget(m_treeView);

	return dockWidget;
}

QTableWidget* MainWindow::createTableWidget()
{
	auto tableView = new QTableWidget;
	tableView->setShowGrid(false);
	tableView->verticalHeader()->setVisible(false);
	tableView->horizontalHeader()->setVisible(false);

	//tableView->setModel(m_fileSystemModel);
	//tableView->setRootIndex(m_fileSystemModel->index(QDir::homePath()));

	return tableView;
}

void MainWindow::createToolBar()
{
	auto goUpDirAction = new QAction(tr("Up"));
	connect(goUpDirAction, &QAction::triggered, this, &MainWindow::goUpDir);

	auto dirToolBar = addToolBar(tr("Up"));
	dirToolBar->addAction(goUpDirAction);
}

void MainWindow::createStatusBar()
{
	m_statusBarLabel = new QLabel;
	m_statusBarLabel->setAlignment(Qt::AlignLeft);
	statusBar()->addWidget(m_statusBarLabel);
}

void MainWindow::updateStatusBar()
{
	auto status = QString(tr("The directory contains %1 folders and %2 files.")).arg(m_foldersCount).arg(m_filesCount);
	m_statusBarLabel->setText(status);

	m_foldersCount = 0;
	m_filesCount = 0;
}

