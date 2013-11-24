#ifndef SENDORBSENTRY_H
#define SENDORBSENTRY_H

#include <QFrame>

namespace Ui {
    class SendOrbsEntry;
}
class WalletModel;
class SendOrbsRecipient;

/** A single entry in the dialog for sending bitcoins. */
class SendOrbsEntry : public QFrame
{
    Q_OBJECT

public:
    explicit SendOrbsEntry(QWidget *parent = 0);
    ~SendOrbsEntry();

    void setModel(WalletModel *model);
    bool validate();
    SendOrbsRecipient getValue();

    /** Return whether the entry is still empty and unedited */
    bool isClear();

    void setValue(const SendOrbsRecipient &value);
    void setAddress(const QString &address);

    /** Set up the tab chain manually, as Qt messes up the tab chain by default in some cases (issue https://bugreports.qt-project.org/browse/QTBUG-10907).
     */
    QWidget *setupTabChain(QWidget *prev);

    void setFocus();

public slots:
    void setRemoveEnabled(bool enabled);
    void clear();

signals:
    void removeEntry(SendOrbsEntry *entry);

private slots:
    void on_deleteButton_clicked();
    void on_payTo_textChanged(const QString &address);
    void on_addressBookButton_clicked();
    void on_pasteButton_clicked();
    void updateDisplayUnit();

private:
    Ui::SendOrbsEntry *ui;
    WalletModel *model;
};

#endif // SENDCOINSENTRY_H
