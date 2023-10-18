//
// Created by Miha Oražem on 18. 10. 23.
//

#ifndef CRASHPAD_LOGGING_H
#define CRASHPAD_LOGGING_H

#ifdef CRASHPAD
#include <base/logging.h>
#else
#include <QDebug>
#define LOG(INFO) qDebug()
#define LOG(WARNING) qDebug()

#endif



#endif  // CRASHPAD_LOGGING_H
