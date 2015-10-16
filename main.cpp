#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

static int g_margins = 0;
static int g_fadeoutWidth = 20;
static int g_buttonWidth = 30;

#if QT_VERSION >= 0x050600
class CustomStyle : public QProxyStyle
{
public:
    QRect subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const
    {
        switch (sr) {
        case SE_TabBarScrollLeftButton: {
            // Return the rect of the left scroll button
            const bool vertical = opt->rect.height() > opt->rect.width();
            const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget);
            return vertical ? QRect(0, 0, opt->rect.width() - 1, buttonWidth)
                : QStyle::visualRect(widget->layoutDirection(), opt->rect, QRect(0, 0, buttonWidth, opt->rect.height() - 1));
            break; }
        case SE_TabBarScrollRightButton: {
            // Return the rect of the right scroll button
            const bool vertical = opt->rect.height() > opt->rect.width();
            const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget);
            return vertical ? QRect(0, opt->rect.height() - buttonWidth, opt->rect.width() - 1, buttonWidth)
                : QStyle::visualRect(widget->layoutDirection(), opt->rect, QRect(opt->rect.width() - buttonWidth, 0, buttonWidth, opt->rect.height()));
            break; }
        case SE_TabBarScrollRect:
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
                // Return the rect between the scroll buttons. A smaller rect will add more margins between
                // buttons and tabs. If any of the tabs are outside the scroll rect, the scroll buttons will
                // be enabled.
                // When the scroll buttons are clicked, the next tab outside the scroll rect will be scrolled
                // to visible. Since we don't want that tab to end up under the tab tear/fade-out indicator, we
                // make the scroll rect a bit smaller so they don't overlap.
                const bool vertical = tab->rect.height() > tab->rect.width();
                const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget);
                const int leftMargin = g_margins + (tab->scrollPosition == QStyleOptionTab::Beginning ? 0 : g_fadeoutWidth);
                const int rightMargin = g_margins + (tab->scrollPosition == QStyleOptionTab::End ? 0 : g_fadeoutWidth);
                const int x = buttonWidth + leftMargin;
                const int w = (vertical ? opt->rect.height() : opt->rect.width()) - x - buttonWidth - rightMargin;
                return vertical ? QRect(0, x, opt->rect.width(), w)
                    : QStyle::visualRect(widget->layoutDirection(), opt->rect, QRect(x, 0, w, opt->rect.height()));
            }
        case SE_TabBarTearIndicatorLeft: {
            // Return the rect of the fade out area on the left side of the tabbar.
            const bool vertical = opt->rect.height() > opt->rect.width();
            const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget);
            return vertical ? QRect(0, buttonWidth, opt->rect.width() - 2, g_fadeoutWidth)
                : QStyle::visualRect(widget->layoutDirection(), opt->rect, QRect(buttonWidth, 0, g_fadeoutWidth, opt->rect.height() - 2));
            break; }
        case SE_TabBarTearIndicatorRight: {
            // Return the rect of the fade out area on the right side of the tabbar.
            const bool vertical = opt->rect.height() > opt->rect.width();
            const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget);
            const int x = opt->rect.width() - buttonWidth - g_fadeoutWidth;
            return vertical ? QRect(0, opt->rect.height() - buttonWidth - g_fadeoutWidth, opt->rect.width() - 2, g_fadeoutWidth)
                : QStyle::visualRect(widget->layoutDirection(), opt->rect, QRect(x, 0, g_fadeoutWidth, opt->rect.height() - 2));
            break; }
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
                bool vertical = tab->shape == QTabBar::RoundedEast
                        || tab->shape == QTabBar::RoundedWest
                        || tab->shape == QTabBar::TriangularEast
                        || tab->shape == QTabBar::TriangularWest;
                const bool leftToRight = tab->direction == Qt::LeftToRight;
                QLinearGradient gradient(opt->rect.topLeft(), vertical ? opt->rect.bottomLeft() : opt->rect.topRight());
                gradient.setColorAt(leftToRight ? 0 : 0.8, tab->palette.window().color());
                gradient.setColorAt(leftToRight ? 0.8 : 0, Qt::transparent);
                p->fillRect(opt->rect, gradient);
            }
            break;
        case PE_IndicatorTabTearRight:
            // Fade out tab on right side:
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
                bool vertical = tab->shape == QTabBar::RoundedEast
                        || tab->shape == QTabBar::RoundedWest
                        || tab->shape == QTabBar::TriangularEast
                        || tab->shape == QTabBar::TriangularWest;
                const bool leftToRight = tab->direction == Qt::LeftToRight;
                QLinearGradient gradient(opt->rect.topLeft(), vertical ? opt->rect.bottomLeft() : opt->rect.topRight());
                gradient.setColorAt(leftToRight ? 0 : 0.8, Qt::transparent);
                gradient.setColorAt(leftToRight ? 0.8 : 0, tab->palette.window().color());
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
#endif

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
//        tabwidget1->setTabPosition(QTabWidget::West);
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

#if 1
    CustomStyle *customStyle = new CustomStyle;
    customStyle->setBaseStyle(QStyleFactory::create(QStringLiteral("fusion")));
    app.setStyle(customStyle);
#else
    app.setStyle("fusion");
#endif

//    app.setLayoutDirection(Qt::RightToLeft);

    TestWidget wid1;
    wid1.resize(800, 600);
    wid1.show();

    return app.exec();
}

