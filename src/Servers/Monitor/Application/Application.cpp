#include "Application.h"

Application::Application(){
    Application::appID = "";
}

Application::Application(string appID){
    Application::appID = appID;
}

std::string Application::getAppID(){
    return Application::appID;
}