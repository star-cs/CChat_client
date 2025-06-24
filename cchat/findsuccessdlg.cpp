#include "findsuccessdlg.h"
#include "ui_findsuccessdlg.h"

#include "applyfriend.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

FindSuccessDlg::FindSuccessDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindSuccessDlg)
{
    ui->setupUi(this);

    setWindowTitle("添加好友");

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    QString app_path = QCoreApplication::applicationDirPath();
    QString pix_path = QDir::toNativeSeparators(app_path +
                            QDir::separator() + "static" +
                            QDir::separator() + "head_1.jpg");

    QPixmap head_pix(pix_path);

    head_pix = head_pix.scaled(ui->head_lb->size(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);

    ui->head_lb->setPixmap(head_pix);
    ui->add_friend_btn->SetState("normal","hover","press");
    this->setModal(true);
}

FindSuccessDlg::~FindSuccessDlg()
{
    qDebug()<<"FindSuccessDlg destruct";
    delete ui;
}

void FindSuccessDlg::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    ui->name_lb->setText(si->_name);
    _si = si;    
    QPixmap head_pix(_si->_icon);
    head_pix = head_pix.scaled(ui->head_lb->size(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);
}

void FindSuccessDlg::on_add_friend_btn_clicked()
{
     //添加好友界面弹出
     this->hide();
     auto applyFriend = new ApplyFriend();
     applyFriend->SetSearchInfo(_si);
     applyFriend->setModal(true);
     applyFriend->show();
}
