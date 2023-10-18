//
// Created by Miha Oražem on 18. 10. 23.
//

#ifndef CRASHPAD_XMEDICPROJECT_H
#define CRASHPAD_XMEDICPROJECT_H
#include <vector>
#include <string>

struct XMedicProject {
  std::vector<std::string> files;
  std::string report_uuid;
};

#endif  // CRASHPAD_XMEDICPROJECT_H
