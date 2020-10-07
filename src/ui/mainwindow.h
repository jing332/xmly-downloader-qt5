#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QTimer>

#include "appsettings.h"
#include "downloadqueuedialog.h"
#include "xmlydownloader.h"

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

  void ApplySettings();

 protected:
  void showEvent(QShowEvent *event);

 private:
  void SetStyleSheet(const QString &filePath);

 private slots:
  void on_descCheckBox_stateChanged(int arg1);
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
  void on_cookieBtn_clicked();
  void on_themeComboBox_currentIndexChanged(int index);

  void OnGetAlbumInfoFinished(int albumID, AlbumInfo *info);
  void OnGetAlbumInfoFailed(const QString &err);
  void OnGetAudioInfoFailed(int albumID, const QString &err);

  void AddAudioInfoItem(const QList<TrackInfo *> &list);
  void Timeout();

 private:
  Ui::MainWindow *ui_;

  QList<TrackInfo *> audioList_;
  QMenu *tableWidgetMenu_;
  QTimer *timer_;
  QThreadPool *pool_;

  AppSettings *appSettings_;

  /*专辑类型(1: 免费, 2: VIP, 3: 付费)*/
  int albumType;
  /*是否升序*/
  bool isAsc_ = true;
  /*是否在文件名前添加序号*/
  bool isAddNum = false;
  /*专辑名称*/
  QString albumName_;
  /*文件后缀名*/
  QString extName = QStringLiteral("m4a");
  /*jvm.[dll/so/dylib]目录*/
  QString jvmDlPath_;
  /*特殊字符的正则表达式*/
  QRegularExpression fileNameReg_ = QRegularExpression("[/\\:*?\"<>|]");
};
#endif  // MAINWINDOW_H
