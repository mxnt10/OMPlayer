#ifndef OMPLAYER_SETTINGS_H
#define OMPLAYER_SETTINGS_H

#include <QDialog>

class Button;


/**
 * Classe para as configurações.
 **********************************************************************************************************************/


class Settings : public QDialog {
Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings() override;

signals:
    void emitclose();
    void emitvalue(const QString &value);

private slots:
    void onClose();
    void rendererReset();
    void setOpenglwidget();
    void setWidgetrend();
    void setQGLWidget2();
    void setX11rend();
    void setXVideorend();

private:
    Button *openglwidget, *qglwidget2, *widgetrend, *xvideorend, *x11rend;
    const char *theme;
};


#endif //OMPLAYER_SETTINGS_H
