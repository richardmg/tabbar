#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

static int g_margins = 0;
static int g_fadeoutWidth = 20;
static int g_buttonWidth = 20;

class CustomStyle : public QProxyStyle
{
public:
    QRect subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const
    {
        switch (sr) {
        case SE_TabBarScrollLeftButton:
            if (const QStyleOptionTabBarBase *base = qstyleoption_cast<const QStyleOptionTabBarBase *>(opt)) {
                // Return the rect of the left scroll button
                const int buttonWidth = qMax(pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget), QApplication::globalStrut().width());
                return QStyle::visualRect(widget->layoutDirection(), base->rect, QRect(0, 0, buttonWidth, base->rect.height() - 1));
            }
            break;
        case SE_TabBarScrollRightButton:
            if (const QStyleOptionTabBarBase *base = qstyleoption_cast<const QStyleOptionTabBarBase *>(opt)) {
                // Return the rect of the right scroll button
                const int buttonWidth = qMax(pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget), QApplication::globalStrut().width());
                return QStyle::visualRect(widget->layoutDirection(), base->rect, QRect(base->rect.width() - buttonWidth, 0, buttonWidth, base->rect.height()));
            }
            break;
        case SE_TabBarScrollRect:
            if (const QStyleOptionTabBarBase *base = qstyleoption_cast<const QStyleOptionTabBarBase *>(opt)) {
                // Return the rect between the scroll buttons. A smaller rect will add more margins between
                // buttons and tabs. If a tab is outside the scroll rect, the scroll buttons will be enabled.
                const int leftEdge = subElementRect(SE_TabBarScrollLeftButton, base, widget).right() + g_margins;
                const int rightEdge = subElementRect(SE_TabBarScrollRightButton, base, widget).x() - g_margins;
                return QRect(leftEdge, 0, rightEdge - leftEdge, 0);
            }
            break;
        case SE_TabBarTearIndicatorLeft:
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
                // Return the rect of the fade out area on the left side of the tabbar.
                const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget);
                return QRect(buttonWidth, 0, g_fadeoutWidth, tab->rect.height() - 2);
            }
            break;
        case SE_TabBarTearIndicatorRight:
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
                // Return the rect of the fade out area on the right side of the tabbar.
                const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget);
                return QRect(tab->rect.width() - buttonWidth - g_fadeoutWidth, 0, g_fadeoutWidth, tab->rect.height() - 2);
            }
            break;
        default:
            break;
        }
        return QProxyStyle::subElementRect(sr, opt, widget);
    }

    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget) const
    {
        switch (pe) {
        case PE_IndicatorTabTearLeft:
            // Fade out tab on left side:
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
                QLinearGradient gradient(opt->rect.topLeft(), opt->rect.topRight());
                gradient.setColorAt(0, tab->palette.window().color());
                gradient.setColorAt(1, Qt::transparent);
                p->fillRect(opt->rect, gradient);
            }
            break;
        case PE_IndicatorTabTearRight:
            // Fade out tab on right side:
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
                QLinearGradient gradient(opt->rect.topLeft(), opt->rect.topRight());
                gradient.setColorAt(0, Qt::transparent);
                gradient.setColorAt(1, tab->palette.window().color());
                p->fillRect(opt->rect, gradient);
            }
            break;
        default:
            return QProxyStyle::drawPrimitive(pe, opt, p, widget);
        }
    }

    int pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *widget) const
    {
        switch (m) {
        case PM_TabBarScrollButtonWidth:
            return g_buttonWidth;
        default:
            break;
        }
        return QProxyStyle::pixelMetric(m, opt, widget);
    }
};

class TestWidget : public QWidget
{
    Q_OBJECT

public:
    TestWidget(QWidget *parent = 0) : QWidget(parent)
    {
        QVBoxLayout *mainLayout = new QVBoxLayout;
        QHBoxLayout *subLayout = new QHBoxLayout;
        setLayout(mainLayout);

        QTabWidget *tabwidget1 = new QTabWidget;
        tabwidget1->setMovable(true);
        for (int i = 0; i < 20; ++i)
            tabwidget1->addTab(new QWidget(), QStringLiteral("Tab") + QString::number(i));
        mainLayout->addWidget(tabwidget1);

        subLayout->addWidget(new QLabel(QStringLiteral("Button width:")));
        QSpinBox *spinBox = new QSpinBox;
        spinBox->setValue(g_buttonWidth);
        connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(setButtonWidth(int)));
        subLayout->addWidget(spinBox, 1);

        subLayout->addWidget(new QLabel(QStringLiteral("Margins:")));
        spinBox = new QSpinBox;
        spinBox->setValue(g_margins);
        connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(setMargins(int)));
        subLayout->addWidget(spinBox, 1);

        subLayout->addWidget(new QLabel(QStringLiteral("Fade out:")));
        spinBox = new QSpinBox;
        spinBox->setValue(g_fadeoutWidth);
        spinBox->setRange(0, 200);
        spinBox->setSingleStep(10);
        connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(setFadeoutWidth(int)));
        subLayout->addWidget(spinBox, 1);

        mainLayout->addLayout(subLayout);
    }

public slots:
    void updateStyle()
    {
        foreach (QWidget *w, QApplication::allWidgets()) {
            QEvent e(QEvent::StyleChange);
            QApplication::sendEvent(w, &e);
        }
    }

    void setButtonWidth(int width)
    {
        g_buttonWidth = width;
        updateStyle();
    }

    void setMargins(int margins)
    {
        g_margins = margins;
        updateStyle();
    }

    void setFadeoutWidth(int width)
    {
        g_fadeoutWidth = width;
        updateStyle();
    }
};

#include "main.moc"

int main(int argc, char **argv){
    QApplication app(argc, argv);

    CustomStyle *customStyle = new CustomStyle;
    customStyle->setBaseStyle(QStyleFactory::create(QStringLiteral("fusion")));
    app.setStyle(customStyle);

    TestWidget wid1;
    wid1.resize(800, 600);
    wid1.show();

    return app.exec();
}

