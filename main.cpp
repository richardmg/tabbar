#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

#define SUPERCLASS QWidget

class TestWidget : public SUPERCLASS
{
    Q_OBJECT

public:
    QTabBar *m_tabbar;
    QTabWidget *m_tabwidget;
    TestWidget(QWidget *parent = 0) : SUPERCLASS(parent)
    {
        m_tabbar = new QTabBar(this);
        m_tabbar->setMovable(true);
        m_tabbar->move(50, 50);
        m_tabbar->resize(300, m_tabbar->size().height());

        m_tabwidget = new QTabWidget(this);
        m_tabwidget->setMovable(true);
        m_tabwidget->move(50, 200);
        m_tabwidget->resize(300, 100);

        for (int i = 0; i < 20; ++i) {
            m_tabbar->addTab(QLatin1String("Tab") + QString::number(i));
            m_tabwidget->addTab(new QWidget(), QLatin1String("Tab") + QString::number(i));
        }

//        QPushButton *b = new QPushButton("hitme", this);
//        m_tabbar->setTabButton(2, QTabBar::LeftSide, b);

    }

public slots:
    void kaboom()
    {
        QMessageBox::information(this, "Hi", "KABOOM!");
    }
};

#include "main.moc"

int main(int argc, char **argv){
    QApplication app(argc, argv);
    app.setStyle(QLatin1String("fusion"));
    TestWidget wid1;
    wid1.resize(800, 600);
    wid1.show();

    return app.exec();
}

