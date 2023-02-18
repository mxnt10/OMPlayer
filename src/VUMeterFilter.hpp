#ifndef OMPLAYER_VUMETERFILTER_HPP
#define OMPLAYER_VUMETERFILTER_HPP

#include <QtAV/Filter.h>

using namespace QtAV;

class VUMeterFilter : public AudioFilter {
Q_OBJECT

public:
    explicit VUMeterFilter(QObject* parent = nullptr);


#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"

Q_SIGNALS:
    void leftLevelChanged(float value);
    void rightLevelChanged(float value);

#pragma clang diagnostic pop


protected:
    void process(Statistics* statistics, AudioFrame* frame) override;

private:
    float left{0}, right{0};
};

#endif //OMPLAYER_VUMETERFILTER_HPP
