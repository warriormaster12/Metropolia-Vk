#include <iostream>
#include "window.h"
#include "logger.h"


int main(int argc, char* argv[]) {
    Logger::Init();
    MetroWindow::Init(1280, 720);
    while(MetroWindow::Running()) {

    }
    MetroWindow::Shutdown();
    return 0;
}
