#include <iostream>
#include <chrono>
#include <fstream>
#include <ctime>

#include "scheme_configuration/scheme.h"
#include "profile.h"

using namespace std;

int main()
{
    Scheme scheme, scheme1, scheme2;

    scheme.add({Type::R, 2, -1, -1});
    scheme.add({Type::R, 6, 1, -1});
    scheme.add({Type::E, 2, 0, -1});
    scheme.add({Type::J, 2, 5, 3});
    scheme.add({Type::R, 8, 1, 5});

    scheme1.add({Type::E, 1, -1, -1});
    scheme1.add({Type::R, 1, 1, -1});
    scheme1.add({Type::R, 2, 3, -1});
    scheme1.add({Type::R, 3, 5, -1});

    scheme1.add({Type::R, 4, -1, 0});
    scheme1.add({Type::E, 2, -1, 1});
    scheme1.add({Type::J, 1, -1, 3});
    scheme1.add({Type::J, 2, -1, 5});
    scheme1.add({Type::E, 3, -1, 7});

    scheme1.add({Type::R, 5, 8, 10});
    scheme1.add({Type::R, 6, 10, 12});
    scheme1.add({Type::R, 7, 12, 14});
    scheme1.add({Type::R, 8, 14, 16});

    scheme1.add({Type::E, 4, -1, 8}); //
    scheme1.add({Type::R, 9, -1, 10});
    scheme1.add({Type::J, 3, 12, -1});
    scheme1.add({Type::J, 4, 14, -1});
    scheme1.add({Type::E, 5, 16, -1});

    scheme1.add({Type::R, 10, 26, 28}); //
    scheme1.add({Type::R, 11, 28, 31});
    scheme1.add({Type::R, 12, 31, 33});
    scheme1.add({Type::R, 13, 33, 35});

    scheme1.add({Type::E, 6, 36, -1}); //
    scheme1.add({Type::E, 7, 38, -1});
    scheme1.add({Type::E, 8, 40, -1});
    scheme1.add({Type::E, 9, 42, -1});
    scheme1.add({Type::E, 10, 43, -1});

    scheme1.add({Type::R, 14, 45, 47});
    scheme1.add({Type::R, 15, 47, 49});
    scheme1.add({Type::R, 16, 49, 51});
    scheme1.add({Type::R, 17, 51, 53});

    scheme1.add({Type::R, 18, 45, -1});
    scheme1.add({Type::R, 19, 47, -1});
    scheme1.add({Type::R, 20, 49, -1});
    scheme1.add({Type::R, 21, 51, -1});
    scheme1.add({Type::R, 22, 53, -1});

    scheme1.add({Type::E, 11, 63, 65});
    scheme1.add({Type::E, 12, 67, 65});
    scheme1.add({Type::E, 13, 67, 69});
    scheme1.add({Type::E, 14, 71, 69});

    scheme2.add({Type::R, 1, -1, -1});
    scheme2.add({Type::E, 2, 1, -1});
    scheme2.add({Type::E, 3, -1, 3});
    scheme2.add({Type::R, 4, 0, 4});
    scheme2.add({Type::R, 5, 3, -1});
    scheme2.add({Type::R, 6, 4, -1});
    scheme2.add({Type::J, 7, 11, 9});
    scheme2.add({Type::E, 8, -1, 0});
    scheme2.add({Type::E, 9, -1, 4});
    scheme2.add({Type::J, 10, 11, -1});
    scheme2.add({Type::R, 11, 14, 16});
    scheme2.add({Type::R, 12, 16, 19});

    {
        LOG_DURATION("scheme updating");
        scheme.update();
    }
    {
        LOG_DURATION("scheme1 updating");
        scheme1.update();
    }
    {
        LOG_DURATION("scheme2 updating");
        scheme2.update();
    }

    char cTime[26];
    time_t tTime = time(NULL);
    ctime_s(cTime, sizeof(cTime), &tTime);
    string sTime = cTime;
    replace(sTime.begin(), sTime.end(), ' ', '_');
    replace(sTime.begin(), sTime.end(), ':', '-');
    sTime.pop_back();
    sTime.append(".txt");
    string filePath = "C:\\Dev\\Study\\C++\\Projects\\";
    filePath.append(sTime);

    ofstream file(filePath);
    file << scheme2.toString();
    file.close();
    system(filePath.c_str());

    return 0;
}