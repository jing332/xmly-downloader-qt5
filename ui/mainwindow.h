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

 protected:
  void showEvent(QShowEvent *event) override;
  void closeEvent(QCloseEvent *event) override;

 private:
  void SetStyleSheet(const QString &filePath);
  int GetIntWidth(int n);
  void ReadConfig();

 signals:
  void AddAudioItem(AudioItem *item);

 private slots:
  void on_selectDirBtn_clicked();
  void on_parseBtn_clicked();
  void on_selectAllBtn_clicked();
  void on_unselectBtn_clicked();
  void on_startDownloadBtn_clicked();
  void on_downloadDirLabel_linkActivated(const QString &link);
  void on_tableWidget_customContextMenuRequested(const QPoint &pos);
  void on_titleLabel_linkActivated(const QString &link);
  void on_addNumCheckBox_clicked();
  void on_mp3RadioBtn_clicked();
  void on_m4aRadioBtn_clicked();

  void OnGetAlbumInfoFinished(AlbumInfo *info, int audiobookId);
  void OnGetAudioInfoFinished(const QList<AudioItem *> &audioItemList);
  void OnGetAudioInfoError(const QString reason, int audiobookId, int page,
                           int pageSize);
  void Timeout();

  void on_cookieBtn_clicked();

  void on_themeComboBox_currentIndexChanged(int index);

 private:
  Ui::MainWindow *ui_;

  QList<AudioItem *> audioItems_;
  QMenu *tableWidgetMenu_;
  QTimer *timer_;
  QThreadPool *pool_;

  /*是否在文件名前添加序号*/
  bool isAddNum = false;
  /*专辑名称*/
  QString albumName_;
  /*下载路径*/
  QString downloadDir_;
  /*Cookie 非免费专辑必填*/
  QString cookie_;

  /*文件后缀名*/
  QString suffixName_ = QStringLiteral(".m4a");
  /*特殊字符的正则表达式*/
  QRegularExpression fileNameReg_ = QRegularExpression("[\\:*?\"<>|]");
};
#endif  // MAINWINDOW_H
