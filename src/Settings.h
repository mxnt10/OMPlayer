#ifndef OMPLAYER_SETTINGS_H
#define OMPLAYER_SETTINGS_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialog;
class QPoint;
QT_END_NAMESPACE

class Button;


/**
 * Classe para as configurações.
 **********************************************************************************************************************/


class Settings : public QDialog {
Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void emitclose();

private slots:
    void onClose();
    void setOpenglwidget();
    void setQglwidget2();
    void setWidgetrend();
    void setXvideorend();
    void setX11rend();

private:
    Button *openglwidget, *qglwidget2, *widgetrend, *xvideorend, *x11rend;
    QPoint setPos;
};


#endif //OMPLAYER_SETTINGS_H
