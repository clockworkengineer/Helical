/*
 * File:   main.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Program: Helical
//
// Description:
//

// =============
// INCLUDE FILES
// =============

#include "helicalmainwindow.h"

#include <QApplication>

// ============================
// ===== MAIN ENTRY POINT =====
// ============================

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */

int main(int argc, char *argv[])
{
    QApplication helicalApplication(argc, argv);

    HelicalMainWindow helicalMain;

    helicalMain.show();

    return helicalApplication.exec();

}
