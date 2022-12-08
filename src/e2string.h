/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file_header.h to edit this template
 */

/* 
 * File:   e2string.h
 * Author: user
 *
 * Created on December 8, 2022, 9:57 AM
 */

#ifndef E2STRING_H
#define E2STRING_H

#include <wx/string.h>

// wxWidgets 3.0+ uses wide strings exclusively, which it refers to as "Unicode strings"

const wchar_t* e2C(const wxString& s) {
    return s.wc_str();
}

#endif /* E2STRING_H */
