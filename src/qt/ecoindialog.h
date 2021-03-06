#ifndef ECOINDIALOG_H
#define ECOINDIALOG_H

#include <QWidget>
#include "walletmodel.h"
#include "upgradewidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "wallet/wallet.h"
using namespace std;

class WalletModel;
namespace Ui {
class ecoindialog;
}
class ecoincreatedialog;
class eCoinSenddialog;
class ecoinsendaffrimdialog;
class eCoinSelectAddress;
class eCoinSendresultDialog;
class upgradewidget;
class EcoinAddressDialog;

using namespace std;
class ECoinDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ECoinDialog(QWidget *parent = 0);
    ~ECoinDialog();
    void setModel(WalletModel *_model);
    void showEvent(QShowEvent *);
    // int addnewoneipcmessage(QString name,QString add,int num);
    int addnewoneipcmessage(QString name);
    static void updatalist();
    static bool m_bNeedUpdateLater;
private   Q_SLOTS:
    void setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance,
                    const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance);

    void gotoeCoinSelectAddressPage(int e);
    void gotoeCoinCreatePage(QString);
    void gotoeCoinAddressPage(QString);
    void gotoCreateeCoinResultPage();
    void gotoSendeCoinResultPage(QString);
    void goback(QString);
    void gotoSendeCoinPage(QString name);
    void updateDisplayUnit();
    void gobacktoePage();
    void gotoSendAffrimDialogPage(QString,QString);
    void on_createEcoinBtn_pressed();
    void on_chooseadd_pressed();
    void GetOldAddress();
    void updataLater();

private:
    QVBoxLayout  *pvboxlayoutall;
    Ui::ecoindialog *ui;
    WalletModel *model;
    std::map<std::string, uint64_t> m_eCoinlist;
    typedef std::map<std::string, uint64_t> map_t;
    ecoincreatedialog *ecoincreateDialogPage;
    eCoinSenddialog *eCoinSenddialogPage;
    ecoinsendaffrimdialog *eCoinSendaffrimdialogPage;
    eCoinSelectAddress *pCoinSelectAddressPage;
    eCoinSendresultDialog *  eCoinSendresultDialogPage;
    QString m_address;
    QLabel* pOldSelectIpcButtons;
    EcoinAddressDialog* m_pEcoinAddressDialog;
private Q_SLOTS:
    void changelabel(QLabel*);
};

#endif // ECOINDIALOG_H
