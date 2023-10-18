//
// Created by Miha Oražem on 18. 10. 23.
//

#ifndef CRASHPAD_COMPRESSUTIL_H
#define CRASHPAD_COMPRESSUTIL_H
#include <QString>
#include <optional>
#include <vector>

namespace CompressUtil {
std::optional<QString> CompressRGProjectFiles(
    const std::vector<std::string>& files);
};

#endif  // CRASHPAD_COMPRESSUTIL_H
