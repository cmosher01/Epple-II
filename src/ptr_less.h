/* 
 * File:   ptr_less.h
 * Author: Christopher
 *
 * Created on December 14, 2013, 5:44 PM
 */

#ifndef PTR_LESS_H
#define	PTR_LESS_H

template<typename T>
struct ptr_less {

    bool operator()(T* pa, T* pb) {
        return *pa < *pb;
    }

};

#endif	/* PTR_LESS_H */
