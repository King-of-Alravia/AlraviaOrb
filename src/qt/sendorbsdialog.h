#ifndef SENDORBSDIALOG_H
#define SENDORBSDIALOG_H

#include <QDialog>

namespace Ui {
    class SendOrbsDialog;
}
class WalletModel;
class SendOrbsEntry;
class SendOrbsRecipient;

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

/** Dialog for sending alraviaorbs */
class SendOrbsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendOrbsDialog(QWidget *parent = 0);
    ~SendOrbsDialog();

    void setModel(WalletModel *model);

    /** Set up the tab chain manually, as Qt messes up the tab chain by default in some cases (issue https://bugreports.qt-project.org/browse/QTBUG-10907).
     */
    QWidget *setupTabChain(QWidget *prev);

    void setAddress(const QString &address);
    void pasteEntry(const SendOrbsRecipient &rv);
    bool handleURI(const QString &uri);

public slots:
    void clear();
    void reject();
    void accept();
    SendOrbsEntry *addEntry();
    void updateRemoveEnabled();
    void setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance);

private:
    Ui::SendOrbsDialog *ui;
    WalletModel *model;
    bool fNewRecipientAllowed;

private slots:
    void on_sendButton_clicked();
    void removeEntry(SendOrbsEntry* entry);
    void updateDisplayUnit();
};

#endif // SENDORBSDIALOG_H
