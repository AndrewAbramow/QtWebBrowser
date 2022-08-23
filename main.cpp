#include "mainwindow.h"

#include <QApplication>
#include <QtGui/QtGui>
#include <QWebEngineView>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>

class BrowserTabWidget : public QWidget
{
    Q_OBJECT

signals:
    void tabLoadingFinished(QString tabTitle);
};

BrowserTabWidget* createNewBrowserWindow()
{
    auto *browserWindow = new BrowserTabWidget;
    auto *urlLineEdit = new QLineEdit;
    auto *webView = new QWebEngineView;
    auto *vbox = new QVBoxLayout(browserWindow);

    vbox->addWidget(urlLineEdit);
    vbox->addWidget(webView);

    QObject::connect(urlLineEdit, &QLineEdit::returnPressed,[webView, urlLineEdit]()
    {
        auto urlToOpen = urlLineEdit->text();
        webView->load(urlToOpen);
    });
    QObject::connect(webView, &QWebEngineView::loadFinished,[webView, browserWindow](bool ok)
    {
        if (ok) {
            emit browserWindow->tabLoadingFinished(webView->title());
        }
    });
    return browserWindow;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTabWidget tabs;

    QPushButton newTabButt("+");
    tabs.setTabsClosable(true);
    tabs.setCornerWidget(&newTabButt,Qt::BottomLeftCorner);

    QObject::connect(&newTabButt, &QPushButton::clicked,[&tabs] ()
    {
        auto newTab = createNewBrowserWindow();
        QObject::connect(newTab,&BrowserTabWidget::tabLoadingFinished, [&tabs](QString newTitle)
        {
            tabs.setTabText(tabs.currentIndex(), newTitle);
            tabs.setWindowTitle(newTitle);
        });
        tabs.addTab(newTab, "tab " + QString::number(tabs.count()+1));
    });

    newTabButt.click();

    QObject::connect(&tabs,&QTabWidget::tabCloseRequested, [&tabs](int indexToRemove)
    {
        if (tabs.count()>1)
        {
            auto widgetToDelete = tabs.widget(indexToRemove);
            delete widgetToDelete;
        }
    });

    QObject::connect(&tabs, &QTabWidget::currentChanged, [&tabs] (int currentIndex)
    {
        tabs.setWindowTitle(tabs.tabText(currentIndex));
    });
    tabs.show();
    return a.exec();
}

#include <main.moc>
