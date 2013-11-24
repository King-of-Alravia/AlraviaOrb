#include "sendorbsdialog.h"
#include "ui_sendorbsdialog.h"

#include "walletmodel.h"
#include "alraviaorbunits.h"
#include "addressbookpage.h"
#include "optionsmodel.h"
#include "sendorbsentry.h"
#include "guiutil.h"
#include "askpassphrasedialog.h"
#include "base58.h"

#include <QMessageBox>
#include <QTextDocument>
#include <QScrollBar>

SendOrbsDialog::SendOrbsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendOrbsDialog),
    model(0)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC // Icons on push buttons are very uncommon on Mac
    ui->addButton->setIcon(QIcon());
    ui->clearButton->setIcon(QIcon());
    ui->sendButton->setIcon(QIcon());
#endif

    addEntry();

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addEntry()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clear()));

    fNewRecipientAllowed = true;
}

void SendOrbsDialog::setModel(WalletModel *model)
{
    this->model = model;

    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendOrbsEntry *entry = qobject_cast<SendOrbsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            entry->setModel(model);
        }
    }
    if(model && model->getOptionsModel())
    {
        setBalance(model->getBalance(), model->getUnconfirmedBalance(), model->getImmatureBalance());
        connect(model, SIGNAL(balanceChanged(qint64, qint64, qint64)), this, SLOT(setBalance(qint64, qint64, qint64)));
        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
    }
}

SendOrbsDialog::~SendOrbsDialog()
{
    delete ui;
}

void SendOrbsDialog::on_sendButton_clicked()
{
    QList<SendOrbsRecipient> recipients;
    bool valid = true;

    if(!model)
        return;

    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendOrbsEntry *entry = qobject_cast<SendOrbsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            if(entry->validate())
            {
                recipients.append(entry->getValue());
            }
            else
            {
                valid = false;
            }
        }
    }

    if(!valid || recipients.isEmpty())
    {
        return;
    }

    // Format confirmation message
    QStringList formatted;
    foreach(const SendOrbsRecipient &rcp, recipients)
    {
#if QT_VERSION < 0x050000
        formatted.append(tr("<b>%1</b> to %2 (%3)").arg(AlraviaOrbUnits::formatWithUnit(AlraviaOrbUnits::ALO, rcp.amount), Qt::escape(rcp.label), rcp.address));
#else
		formatted.append(tr("<b>%1</b> to %2 (%3)").arg(AlraviaOrbUnits::formatWithUnit(AlraviaOrbUnits::ALO, rcp.amount), rcp.label.toHtmlEscaped(), rcp.address));
#endif
    }

    fNewRecipientAllowed = false;

    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm send Orbs"),
                          tr("Are you sure you want to send %1?").arg(formatted.join(tr(" and "))),
          QMessageBox::Yes|QMessageBox::Cancel,
          QMessageBox::Cancel);

    if(retval != QMessageBox::Yes)
    {
        fNewRecipientAllowed = true;
        return;
    }

    WalletModel::UnlockContext ctx(model->requestUnlock());
    if(!ctx.isValid())
    {
        // Unlock wallet was cancelled
        fNewRecipientAllowed = true;
        return;
    }

    WalletModel::SendOrbsReturn sendstatus = model->sendOrbs(recipients);
    switch(sendstatus.status)
    {
    case WalletModel::InvalidAddress:
        QMessageBox::warning(this, tr("Send Orbs"),
            tr("The recipient address is not valid, please recheck."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::InvalidAmount:
        QMessageBox::warning(this, tr("Send Orbs"),
            tr("The amount to pay must be larger than 0."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::AmountExceedsBalance:
        QMessageBox::warning(this, tr("Send Orbs"),
            tr("The amount exceeds your balance."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::AmountWithFeeExceedsBalance:
        QMessageBox::warning(this, tr("Send Orbs"),
            tr("The total exceeds your balance when the %1 transaction fee is included.").
            arg(AlraviaOrbUnits::formatWithUnit(AlraviaOrbUnits::ALO, sendstatus.fee)),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::DuplicateAddress:
        QMessageBox::warning(this, tr("Send Orbs"),
            tr("Duplicate address found, can only send to each address once per send operation."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::TransactionCreationFailed:
        QMessageBox::warning(this, tr("Send Orbs"),
            tr("Error: Transaction creation failed!"),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::TransactionCommitFailed:
        QMessageBox::warning(this, tr("Send Orbs"),
            tr("Error: The transaction was rejected. This might happen if some of the orbs in your wallet were already spent, such as if you used a copy of wallet.dat and orbs were spent in the copy but not marked as spent here."),
            QMessageBox::Ok, QMessageBox::Ok);
        break;
    case WalletModel::Aborted: // User aborted, nothing to do
        break;
    case WalletModel::OK:
        accept();
        break;
    }
    fNewRecipientAllowed = true;
}

void SendOrbsDialog::clear()
{
    // Remove entries until only one left
    while(ui->entries->count())
    {
        delete ui->entries->takeAt(0)->widget();
    }
    addEntry();

    updateRemoveEnabled();

    ui->sendButton->setDefault(true);
}

void SendOrbsDialog::reject()
{
    clear();
}

void SendOrbsDialog::accept()
{
    clear();
}

SendOrbsEntry *SendOrbsDialog::addEntry()
{
    SendOrbsEntry *entry = new SendOrbsEntry(this);
    entry->setModel(model);
    ui->entries->addWidget(entry);
    connect(entry, SIGNAL(removeEntry(SendOrbsEntry*)), this, SLOT(removeEntry(SendOrbsEntry*)));

    updateRemoveEnabled();

    // Focus the field, so that entry can start immediately
    entry->clear();
    entry->setFocus();
    ui->scrollAreaWidgetContents->resize(ui->scrollAreaWidgetContents->sizeHint());
    qApp->processEvents();
    QScrollBar* bar = ui->scrollArea->verticalScrollBar();
    if(bar)
        bar->setSliderPosition(bar->maximum());
    return entry;
}

void SendOrbsDialog::updateRemoveEnabled()
{
    // Remove buttons are enabled as soon as there is more than one send-entry
    bool enabled = (ui->entries->count() > 1);
    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendOrbsEntry *entry = qobject_cast<SendOrbsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            entry->setRemoveEnabled(enabled);
        }
    }
    setupTabChain(0);
}

void SendOrbsDialog::removeEntry(SendOrbsEntry* entry)
{
    delete entry;
    updateRemoveEnabled();
}

QWidget *SendOrbsDialog::setupTabChain(QWidget *prev)
{
    for(int i = 0; i < ui->entries->count(); ++i)
    {
        SendOrbsEntry *entry = qobject_cast<SendOrbsEntry*>(ui->entries->itemAt(i)->widget());
        if(entry)
        {
            prev = entry->setupTabChain(prev);
        }
    }
    QWidget::setTabOrder(prev, ui->addButton);
    QWidget::setTabOrder(ui->addButton, ui->sendButton);
    return ui->sendButton;
}

void SendOrbsDialog::setAddress(const QString &address)
{
    SendOrbsEntry *entry = 0;
    // Replace the first entry if it is still unused
    if(ui->entries->count() == 1)
    {
        SendOrbsEntry *first = qobject_cast<SendOrbsEntry*>(ui->entries->itemAt(0)->widget());
        if(first->isClear())
        {
            entry = first;
        }
    }
    if(!entry)
    {
        entry = addEntry();
    }

    entry->setAddress(address);
}

void SendOrbsDialog::pasteEntry(const SendOrbsRecipient &rv)
{
    if(!fNewRecipientAllowed)
        return;

    SendOrbsEntry *entry = 0;
    // Replace the first entry if it is still unused
    if(ui->entries->count() == 1)
    {
        SendOrbsEntry *first = qobject_cast<SendOrbsEntry*>(ui->entries->itemAt(0)->widget());
        if(first->isClear())
        {
            entry = first;
        }
    }
    if(!entry)
    {
        entry = addEntry();
    }

    entry->setValue(rv);
}

bool SendOrbsDialog::handleURI(const QString &uri)
{
    SendOrbsRecipient rv;
    // URI has to be valid
    if (GUIUtil::parseAlraviaOrbURI(uri, &rv))
    {
        CAlraviaOrbAddress address(rv.address.toStdString());
        if (!address.IsValid())
            return false;
        pasteEntry(rv);
        return true;
    }

    return false;
}

void SendOrbsDialog::setBalance(qint64 balance, qint64 unconfirmedBalance, qint64 immatureBalance)
{
    Q_UNUSED(unconfirmedBalance);
    Q_UNUSED(immatureBalance);
    if(!model || !model->getOptionsModel())
        return;

    int unit = model->getOptionsModel()->getDisplayUnit();
    ui->labelBalance->setText(AlraviaOrbUnits::formatWithUnit(unit, balance));
}

void SendOrbsDialog::updateDisplayUnit()
{
    if(model && model->getOptionsModel())
    {
        // Update labelBalance with the current balance and the current unit
        ui->labelBalance->setText(AlraviaOrbUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), model->getBalance()));
    }
}
