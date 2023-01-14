#include <Utils>
#include "CWidget.h"


/**********************************************************************************************************************/


/** Construtor */
CWidget::CWidget(QWidget *parent, QFlags<Qt::WindowType> type) : QWidget(parent) {
    setWindowFlags(type);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
}


/** Construtor sem parent */
CWidget::CWidget(QFlags<Qt::WindowType> type) {
    setWindowFlags(type);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
}


/** Outros construtores */
TrackWidget::TrackWidget(QWidget *parent) : QWidget(parent) { setMouseTracking(true); }
