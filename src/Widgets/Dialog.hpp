#ifndef OMPLAYER_DIALOG_HPP
#define OMPLAYER_DIALOG_HPP

#include <QDialog>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

class Dialog : public QDialog {
Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog() override;

public Q_SLOTS:
    void onClose();

protected:
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

Q_SIGNALS:
    void emitclose();

private:
    QGraphicsOpacityEffect *effect{};
    QPropertyAnimation *animation{};
    bool onclose{false};
};

#endif //OMPLAYER_DIALOG_HPP
