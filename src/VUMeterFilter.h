#ifndef OMPLAYER_VUMETERFILTER_H
#define OMPLAYER_VUMETERFILTER_H

#include <QtAV/Filter.h>

using namespace QtAV;

class VUMeterFilter : public AudioFilter {
    Q_OBJECT
    Q_PROPERTY(int leftLevel READ leftLevel WRITE setLeftLevel NOTIFY leftLevelChanged)
    Q_PROPERTY(int rightLevel READ rightLevel WRITE setRightLevel NOTIFY rightLevelChanged)

public:
    explicit VUMeterFilter(QObject* parent = nullptr);
    [[nodiscard]] int leftLevel() const {return left;}
    [[nodiscard]] int rightLevel() const {return right;}
    void setLeftLevel(int value) {left = value;}
    void setRightLevel(int value) {right = value;}

Q_SIGNALS:
    void leftLevelChanged(int value);
    void rightLevelChanged(int value);

protected:
    void process(Statistics* statistics, AudioFrame* frame) override;

private:
    int left{0}, right{0};
};

#endif //OMPLAYER_VUMETERFILTER_H
