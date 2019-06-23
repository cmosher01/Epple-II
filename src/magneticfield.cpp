#include "magneticfield.h"

bool MagneticField::is_on() const {
    return this->on;
}

int MagneticField::position() const {
    return this->pos;
}

void MagneticField::turn_off() {
    this->on = false;
}

void MagneticField::turn_on_at(const int position) {
    this->on = true;
    this->pos = position;
}
