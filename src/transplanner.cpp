#include <string>
#include <iostream>
#include <vector>
#include "StandardDataSink.h"
#include "StandardDataSource.h"
#include "StandardErrorDataSink.h"
#include "FileDataFactory.h"
#include "TransportationPlannerCommandLine.h"
#include "DSVReader.h"
#include "XMLReader.h"
#include "CSVBusSystem.h"
#include "OpenStreetMap.h"
#include "TransportationPlannerConfig.h"
#include "DijkstraTransportationPlanner.h"



using std::cout;
using std::cerr;
using std::endl;

int ParseaArgs(const std::vector<std::string> &args, std::string &datapath,std::string &resultpath);

int main(int argc, char* argv[]){
    std::string DataPath = "./data";
    std::string ResultsPath = "./results";
    std::vector<std::string> Arguments;
    for(int Index = 1; Index < argc; Index++){
        Arguments.push_back(argv[Index]);
    }
    if(ParseaArgs(Arguments,DataPath,ResultsPath)){
        cerr<<"Syntax Error: transplanner [--data=path | --results=path]";
        return 1;
    }
    cout<<DataPath<<" "<<ResultsPath<<endl;

    auto StdIn = std::make_shared<CStandardDataSource>();
    auto StdOut = std::make_shared<CStandardDataSink>();
    auto StdErr = std::make_shared<CStandardErrorDataSink>();
    auto Results = std::make_shared<CFileDataFactory>(ResultsPath);
    auto Data = std::make_shared<CFileDataFactory>(DataPath);
    auto StreetMapSource = Data->CreateSource("city.osm");
    auto StopsSource = Data->CreateSource("stops.csv");
    auto RoutesSource = Data->CreateSource("routes.csv");
    auto StreetMapXMLReader = std::make_shared<CXMLReader>(StreetMapSource);
    auto StopsDSVReader = std::make_shared<CDSVReader>(StopsSource,',');
    auto RoutesDSVReader = std::make_shared<CDSVReader>(RoutesSource,',');   
    auto StreetMap = std::make_shared<COpenStreetMap>(StreetMapXMLReader);
    auto BusSystem = std::make_shared<CCSVBusSystem>(StopsDSVReader,RoutesDSVReader);
    auto Config = std::make_shared<STransportationPlannerConfig>(StreetMap,BusSystem);
    auto Planner = std::make_shared<CDijkstraTransportationPlanner>(Config);
    CTransportationPlannerCommandLine TPCommandLIne(StdIn,StdOut,StdErr,Results,Planner);
    if(!TPCommandLIne.ProcessCommands()){
        return 1;
    }
    return 0;
}
//sharedptr of dijkstraplanner 
int ParseaArgs(const std::vector<std::string> &args,std::string &datapath,std::string &resultpath){
    for(auto &Argument:args){
        if(Argument.find("--data=") == 0){
            datapath = Argument.substr(7);
        }
        else if(Argument.find("--results=") == 0){
            resultpath = Argument.substr(10);
        }
        else{
            return 1;
        }
    }
    return 0;
}