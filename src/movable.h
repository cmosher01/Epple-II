#ifndef MOVABLE_H
#define MOVABLE_H


class Movable {
public:
    virtual ~Movable();
    virtual bool move_by(int delta_position) = 0;
};

#endif // MOVABLE_H
