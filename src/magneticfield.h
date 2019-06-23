#ifndef MAGNETICFIELD_H
#define MAGNETICFIELD_H

class MagneticField {
public:
    bool is_on() const;
    int position() const;

    void turn_off();
    void turn_on_at(const int position);

private:
    bool on = false;
    int pos = 0;
};

#endif // MAGNETICFIELD_H
