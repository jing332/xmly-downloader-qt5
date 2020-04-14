#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QTimer>

#include "cgo.h"
#include "downloadqueuedialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_selectDirBtn_clicked();
  void on_parseBtn_clicked();
  void on_selectAllBtn_clicked();
  void on_unselectBtn_clicked();
  void on_startDownloadBtn_clicked();
  void on_tableWidget_customContextMenuRequested(const QPoint &pos);
  void on_titleLabel_linkActivated(const QString &link);
  void on_mp3RadioBtn_clicked();
  void on_m4aRadioBtn_clicked();

  void getAudiobookInfoFinished(AudioBookInfo *info, int audiobookId);
  void getAudioInfoFinished(const QList<AudioItem *> &audioItemList);
  void getAudioInfoError(const QString reason, int audiobookId, int page,
                         int pageSize);
  void timeout();

 signals:
  void addAudioItemSignal(AudioItem *item);

 private:
  Ui::MainWindow *ui;

  QList<AudioItem *> audioItems;
  QMenu *tableWidgetMenu;
  QTimer *timer;
  QThreadPool *pool;
  QString downloadDir;
  QString suffixName = QStringLiteral(".m4a");
};
#endif  // MAINWINDOW_H
