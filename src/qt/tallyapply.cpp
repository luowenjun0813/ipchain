#include "tallyapply.h"
#include "ui_tallyapply.h"
#include "optionsmodel.h"
#include "ipchainunits.h"
#include "wallet/coincontrol.h"
#include "guiutil.h"
#include <iostream>
#include "log/log.h"
#include "log/stateinfo.h"
#include "intro.h"

TallyApply::TallyApply(QWidget *parent) :
    QWidget(parent),model(NULL),
    ui(new Ui::TallyApply)
{
    ui->setupUi(this);
    ui->label_errmsg->setText("");
    QRegExp regx("[0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, ui->coinEdit );
    ui->coinEdit->setValidator( validator );
    if(Intro::m_clienttype == "test")
    {
        ui->coinEdit->setText("20000");
    }else
    {
        ui->coinEdit->setText("20000");
    }
    ui->coinEdit->setEnabled(false);
}
TallyApply::~TallyApply()
{
    delete ui;
}
void TallyApply::setModel(WalletModel *_model)
{
    this->model = _model;
}
void TallyApply::processSendCoinsReturn(const WalletModel::SendCoinsReturn &sendCoinsReturn)
{
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::black);
    switch(sendCoinsReturn.status)
    {
    case WalletModel::InvalidAddress:
        LOG_WRITE(LOG_INFO,"tallyapply","InvalidAddress");
        ui->label_errmsg->setText(tr("InvalidAddress"));
        break;
    case WalletModel::InvalidAmount:
        LOG_WRITE(LOG_INFO,"tallyapply","InvalidAmount");
        ui->label_errmsg->setText(tr("InvalidAmount"));
        break;
    case WalletModel::AmountExceedsBalance:
        LOG_WRITE(LOG_INFO,"tallyapply","AmountExceedsBalance");
        ui->label_errmsg->setText(tr("AmountExceedsBalance"));
        break;
    case WalletModel::AmountWithFeeExceedsBalance:
        LOG_WRITE(LOG_INFO,"tallyapply","AmountWithFeeExceedsBalance");
        ui->label_errmsg->setText(tr("AmountWithFeeExceedsBalance"));
        break;
    case WalletModel::DuplicateAddress:
        LOG_WRITE(LOG_INFO,"tallyapply","DuplicateAddress");
        ui->label_errmsg->setText(tr("DuplicateAddress"));
        break;
    case WalletModel::TransactionCreationFailed:
        LOG_WRITE(LOG_INFO,"tallyapply","TransactionCreationFailed");
        ui->label_errmsg->setText(tr("TransactionCreationFailed")+tr(" ")+ m_error.c_str());
        break;
    case WalletModel::TransactionCommitFailed:

        LOG_WRITE(LOG_INFO,"tallyapply","TransactionCommitFailed",m_error.c_str());
        if(m_error == "txn-campaign-JOIN_PUBKEY_IS_TIMEOUT_PUNISHED")
        {
            ui->label_errmsg->setText(tr("TransactionCommitFailed")+tr(" ")+tr("txn-campaign-JOIN_PUBKEY_IS_TIMEOUT_PUNISHED"));
        }else if(m_error == "txn-campaign-EXIT_PUBKEY_IS_DEPOSING")
        {
            ui->label_errmsg->setText(tr("TransactionCommitFailed")+tr(" ")+tr("txn-campaign-EXIT_PUBKEY_IS_DEPOSING"));
        }else
            ui->label_errmsg->setText(tr("TransactionCommitFailed")+tr(" ")+ m_error.c_str());
        break;
    case WalletModel::AbsurdFee:
        LOG_WRITE(LOG_INFO,"tallyapply","AbsurdFee");
        ui->label_errmsg->setText(tr("AbsurdFee"));
        break;
    case WalletModel::PaymentRequestExpired:
        LOG_WRITE(LOG_INFO,"tallyapply","PaymentRequestExpired");
        ui->label_errmsg->setText(tr("PaymentRequestExpired"));
        break;
    case WalletModel::OK:

        STATE_CLEAR();
        STATE_WRITE("STATE = ON");

        LOG_WRITE(LOG_INFO,"tallyapply","OK");
        ui->label_errmsg->setText(tr(""));
    default:
        LOG_WRITE(LOG_INFO,"tallyapply",\
                  QString::number(sendCoinsReturn.status).toStdString().c_str());
        ui->label_errmsg->setText(tr("other error"));
        return;
    }
}
void TallyApply::on_pushButton_apply_pressed()
{


    m_error = "";
    if(!model || !model->getOptionsModel())
    {
        return;
    }
    if(ui->coinEdit->text().isEmpty())
    {
        ui->label_errmsg->setText(tr("money error"));
        LOG_WRITE(LOG_INFO,"tallyapply","money error");
        return;
    }
    if(ui->pushButton_address->text().size()<20)
    {
        ui->label_errmsg->setText(tr("address error"));
        LOG_WRITE(LOG_INFO,"tallyapply","address error");
        return;
    }

    if(!model->m_bFinishedLoading){
        ui->label_errmsg->setText(tr("It's in sync. Please wait a moment."));
        return;
    }
    QList<SendCoinsRecipient> recipients;
    SendCoinsRecipient recipient;
    bool valid = true;

    if (recipient.paymentRequest.IsInitialized()){
        LOG_WRITE(LOG_INFO,"tallyapply","recipient.paymentRequest.IsInitialized()");
        return ;
    }
    QString t = ui->coinEdit->text();
    int64_t y = t.toInt();
    QString remainder_str;
    QString quotient_str = QString::number(y);
    remainder_str = QString::number(y)+"00000000";

    recipient.address = ui->pushButton_address->text();
    recipient.label = "";//biaoqian
    recipient.amount = remainder_str.toDouble();
    recipient.message = "";
    recipient.fSubtractFeeFromAmount = true;
    recipients.append(recipient);

    QString add1 = ui->pushButton_address->text();
    CAmount amount1 = remainder_str.toDouble(); ;
    bool was_locked = model->getEncryptionStatus() == WalletModel::Locked;
    if(!model->CheckPassword())
    {
        LOG_WRITE(LOG_INFO,"tallyapply","psd error");
        ui->label_errmsg->setText(tr("password error"));
        return ;
    }

    // If wallet is still locked, unlock was failed or cancelled, mark context as invalid
    valid = model->getEncryptionStatus() != WalletModel::Locked;
    //ctx(this, valid, was_locked);
    WalletModel::UnlockContext ctx(model, valid, was_locked);
    if(!ctx.isValid())
    {
        // Unlock wallet was cancelled
        ui->label_errmsg->setText(tr("Unlock wallet was cancelled"));
        LOG_WRITE(LOG_INFO,"tallyapply","Unlock wallet was cancelled");
        return;
    }

    // prepare transaction for getting txFee earlier
    WalletModelTransaction currentTransaction(recipients);
    WalletModel::SendCoinsReturn prepareStatus;
    // Always use a CCoinControl instance, use the CoinControlDialog instance if CoinControl has been enabled
    CCoinControl ctrl;
    if (model->getOptionsModel()->getCoinControlFeatures())
    {
        //ctrl = *CoinControlDialog::coinControl;
    }
    else
    {
        ctrl.nConfirmTarget = 1;
    }
    LOG_WRITE(LOG_INFO,"tallyapply prepareBookkeepingTransaction",\
              ui->pushButton_address->text().toStdString().c_str(),\
              remainder_str.toStdString().c_str()
              );
    prepareStatus = model->prepareBookkeepingTransaction(add1,amount1,m_error,currentTransaction, &ctrl);
    if(prepareStatus.status != WalletModel::OK) {
        processSendCoinsReturn(prepareStatus);
        return;
    }
    CAmount txFee = currentTransaction.getTransactionFee();
    // Format confirmation message
    QStringList formatted;
    Q_FOREACH(const SendCoinsRecipient &rcp, currentTransaction.getRecipients())
    {
        QString amount = "<b>" + BitcoinUnits::formatHtmlWithUnit(0, rcp.amount);
        amount.append("</b>");
        // generate monospace address string
        QString address = "<span style='font-family: monospace;'>" + rcp.address;
        address.append("</span>");
        QString recipientElement;
        QString q = QString("%1").arg(rcp.amount);
        if (!rcp.paymentRequest.IsInitialized()) // normal payment
        {
            if(rcp.label.length() > 0) // label with address
            {
                recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.label));
                recipientElement.append(QString(" (%1)").arg(address));
            }
            else // just address
            {
                recipientElement = tr("%1 to %2").arg(amount, address);
            }
        }
        else if(!rcp.authenticatedMerchant.isEmpty()) // authenticated payment request
        {
            recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.authenticatedMerchant));
        }
        else // unauthenticated payment request
        {
            recipientElement = tr("%1 to %2").arg(amount, address);
        }

        formatted.append(recipientElement);
    }

    QString questionString = tr("Are you sure you want to send?");
    questionString.append("<br /><br />%1");

    if(txFee > 0)
    {
        // append fee string if a fee is required
        questionString.append("<hr /><span style='color:#aa0000;'>");
        questionString.append(BitcoinUnits::formatHtmlWithUnit(0, txFee));
        questionString.append("</span> ");
        questionString.append(tr("added as transaction fee"));
        questionString.append(" (" + QString::number((double)currentTransaction.getTransactionSize() / 1000) + " kB)");
    }

    // add total amount in all subdivision units
    questionString.append("<hr />");
    CAmount totalAmount = currentTransaction.getTotalTransactionAmount() + txFee;
    QStringList alternativeUnits;
    Q_FOREACH(BitcoinUnits::Unit u, BitcoinUnits::availableUnits())
    {
         alternativeUnits.append(BitcoinUnits::formatHtmlWithUnit(0, totalAmount));
     }

    questionString.append(tr("Total Amount %1")
                          .arg(BitcoinUnits::formatHtmlWithUnit(0, totalAmount)));


    questionString.append(QString("<span style='font-size:10pt;font-weight:normal;'><br />(=%2)</span>")
                          .arg(alternativeUnits.join(" " + tr("or") + "<br />")));


    WalletModel::SendCoinsReturn sendStatus =model->sendBookkeeping(add1,currentTransaction);
    if (sendStatus.status == WalletModel::OK)
    {
        Q_EMIT next(add1,amount1);
    }
    else{
        m_error =  model->m_sendcoinerror;
    }
    processSendCoinsReturn(sendStatus);

}
void TallyApply::setAddress(QString address)
{
    ui->pushButton_address->setText(address);
    QString strDeposit = model->getDeposit(address);
    ui->coinEdit->setText(strDeposit);
}

void TallyApply::on_pushButton_address_pressed()
{
    Q_EMIT selectaddress();
}
void TallyApply::resetinfo()
{
    ui->coinEdit->setText("20000");
    ui->label_errmsg->setText("");
    ui->pushButton_address->setText(tr("select address"));
}

