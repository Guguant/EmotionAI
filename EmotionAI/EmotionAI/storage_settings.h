#ifndef STORAGE_SETTINGS_H
#define STORAGE_SETTINGS_H

#include <QDialog>
#include "ui_storage_settings.h"

namespace Ui {
    class StroageSettings;
}

class StorageSettings : public QDialog
{
    Q_OBJECT

public:
    explicit StorageSettings(QWidget *parent = 0);
    virtual ~StorageSettings();

private slots:
    void OkButtonPressed();
    void BrowerSavePathPressed();  /*�޸�ͼ��洢·��.*/
    void BrowerSnipPathPressed();  /*�޸Ľ�ͼ�洢·��.*/

private:
    Ui::StorageSettings *storage_ui_;
};

#endif /*STORAGE_SETTINGS_H*/