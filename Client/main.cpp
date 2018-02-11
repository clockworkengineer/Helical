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
// Description: A simple Qt based SSH client that uses libssh to connect to a remote
// reserver and execute single commands, run a shell session and access the remote file system
// using SFTP to upload/download files. (SFTP currently needs to be implemeted).
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
