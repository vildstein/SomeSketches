#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTableWidget;
class QFileSystemModel;
class QTreeView;
class QLabel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	virtual ~MainWindow();

private:

	void onDirLoaded(const QString& dir);
	void showHomeDir();
	void goUpDir();


	QDockWidget* createLeftDockWidget();
	QTableWidget* createTableWidget();
	void createToolBar();
	void createStatusBar();

	void updateStatusBar();


	QFileSystemModel* m_fileSystemModel{nullptr};
	QTableWidget* m_tableWidget{nullptr};
	QTreeView* m_treeView{nullptr};
	QLabel* m_statusBarLabel{nullptr};

	QString m_upDir;

	size_t m_foldersCount{0};
	size_t m_filesCount{0};
};
#endif // MAINWINDOW_H
