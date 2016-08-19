#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

static int g_margins = 0;
static int g_fadeoutWidth = 100;
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
            const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget) + g_margins;
            return vertical ? QRect(0, 0, opt->rect.width() - 1, buttonWidth)
                : QStyle::visualRect(widget->layoutDirection(), opt->rect, QRect(0, 0, buttonWidth, opt->rect.height() - 1));
            break; }
        case SE_TabBarScrollRightButton: {
            // Return the rect of the right scroll button
            const bool vertical = opt->rect.height() > opt->rect.width();
            const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget) + g_margins;
            return vertical ? QRect(0, opt->rect.height() - buttonWidth, opt->rect.width() - 1, buttonWidth)
                : QStyle::visualRect(widget->layoutDirection(), opt->rect, QRect(opt->rect.width() - buttonWidth, 0, buttonWidth, opt->rect.height()));
            break; }
        case SE_TabBarTearIndicatorLeft: {
            // Return the rect of the fade out area on the left side of the tabbar.
            const QStyleOptionTabV4 *tabOpt = static_cast<const QStyleOptionTabV4 *>(opt);
            const bool vertical = opt->rect.height() > opt->rect.width();
            const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget);

            int pixelsOutOnLeft;
            QRect unionRect = tabOpt->unionRect;

            if (vertical) {
                pixelsOutOnLeft = buttonWidth - unionRect.y();
            } else if (widget->layoutDirection() == Qt::LeftToRight) {
                pixelsOutOnLeft = buttonWidth - unionRect.x();
            } else {
                int diff = tabOpt->unionRect.width() - tabOpt->rect.width();
                pixelsOutOnLeft = diff + buttonWidth + tabOpt->unionRect.x();
            }

            int fadeout = qMax(5, qMin(g_fadeoutWidth, pixelsOutOnLeft));

            return vertical ? QRect(0, buttonWidth, opt->rect.width() - 2, fadeout)
                : QStyle::visualRect(widget->layoutDirection(), opt->rect, QRect(buttonWidth, 0, fadeout, opt->rect.height() - 2));
            break; }
        case SE_TabBarTearIndicatorRight: {
            // Return the rect of the fade out area on the right side of the tabbar.
            const QStyleOptionTabV4 *tabOpt = static_cast<const QStyleOptionTabV4 *>(opt);
            const bool vertical = opt->rect.height() > opt->rect.width();
            const int buttonWidth = pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, widget);

            int pixelsOutOnRight;
            QRect unionRect = tabOpt->unionRect;

            if (vertical) {
                int diff = tabOpt->unionRect.height() - tabOpt->rect.height();
                pixelsOutOnRight = diff + buttonWidth + tabOpt->unionRect.y();
            } else if (widget->layoutDirection() == Qt::RightToLeft) {
                pixelsOutOnRight = buttonWidth - unionRect.x();
            } else {
                int diff = tabOpt->unionRect.width() - tabOpt->rect.width();
                pixelsOutOnRight = diff + buttonWidth + tabOpt->unionRect.x();
            }

            int fadeout = qMax(5, qMin(g_fadeoutWidth, pixelsOutOnRight - (buttonWidth * 2)));
            const int x = opt->rect.width() - buttonWidth - fadeout;

            return vertical ? QRect(0, opt->rect.height() - buttonWidth - fadeout, opt->rect.width() - 2, fadeout)
                : QStyle::visualRect(widget->layoutDirection(), opt->rect, QRect(x, 0, fadeout, opt->rect.height() - 2));
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
                gradient.setColorAt(leftToRight ? 0 : 0.8, Qt::blue);
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
                gradient.setColorAt(leftToRight ? 0.8 : 0, Qt::blue);
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

        tabwidget1->setTabPosition(QTabWidget::West);

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

