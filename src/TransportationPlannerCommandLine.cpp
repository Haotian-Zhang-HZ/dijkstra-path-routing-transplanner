#include "TransportationPlannerCommandLine.h"
#include "StringUtils.h"
#include "GeographicUtils.h"
#include <limits.h>
#include <sstream>
#include <iomanip>
#include <cmath>

const double NoPathExists = std::numeric_limits<double>::max();

struct CTransportationPlannerCommandLine::SImplementation{
    std::shared_ptr<CDataSource> CmdSource;
    std::shared_ptr<CDataSink> OutSink;
    std::shared_ptr<CDataSink> ErrSink;
    std::shared_ptr<CDataFactory> Results;
    std::shared_ptr<CTransportationPlanner> Planner;

    std::vector<CTransportationPlanner::TTripStep> Path;// Save the path as struct's member parameter
    // These 2 boolean variable is used to store the curr path type
    bool Shortest = false;
    bool Fastest = false;
    // These 2 variable is sed to store the info of the curr path
    double PathTime = -1;     
    double PathDistance = -1; 
    // Record the stored path's srcID and DestID
    CStreetMap::TNodeID PathSrcID = 0;
    CStreetMap::TNodeID PathDestID = 0;


    const std::string LINE1 = "------------------------------------------------------------------------\n";
    const std::string LINE2 = "help     Display this help menu\n";
    const std::string LINE3 = "exit     Exit the program\n";
    const std::string LINE4 = "count    Output the number of nodes in the map\n";
    const std::string LINE5 = "node     Syntax \"node [0, count)\" \n";
    const std::string LINE6 = "         Will output node ID and Lat/Lon for node\n";
    const std::string LINE7 = "fastest  Syntax \"fastest start end\" \n";
    const std::string LINE8 = "         Calculates the time for fastest path from start to end\n";
    const std::string LINE9 = "shortest Syntax \"shortest start end\" \n";
    const std::string LINE10 = "         Calculates the distance for the shortest path from start to end\n";
    const std::string LINE11 = "save     Saves the last calculated path to file\n";
    const std::string LINE12 = "print    Prints the steps for the last calculated path\n";


    SImplementation(std::shared_ptr<CDataSource> cmdsrc,
                    std::shared_ptr<CDataSink> outsink,
                    std::shared_ptr<CDataSink> errsink,
                    std::shared_ptr<CDataFactory> results,
                    std::shared_ptr<CTransportationPlanner> planner)
        : CmdSource(cmdsrc), OutSink(outsink), ErrSink(errsink), Results(results), Planner(planner) {}

    ~SImplementation(){

    }
    bool ReadRow(std::shared_ptr<CDataSource> src, std::string& row){
        row.clear();
        char Char;
        while(!src->End()){
            if(src->Get(Char)){
                if(Char != '\n'){
                row += Char;
                }
                else{
                    return true;
                }
            }
            else{
                break;
            }
        }
        return !row.empty();// return true if we read in sth
    }
    bool ProcessCommands(){
        while(true){
            OutSink->Write({'>',' '});
            std::string CMD;
            if(ReadRow(CmdSource,CMD)){
                if(CMD.empty()){
                    continue;
                }
                std::vector< std::string > CMDS = StringUtils::Split(CMD);
                // output help info
                if(CMDS[0] == "help"){
                    OutSink->Write(std::vector<char>(LINE1.begin(),LINE1.end()));
                    OutSink->Write(std::vector<char>(LINE2.begin(),LINE2.end()));
                    OutSink->Write(std::vector<char>(LINE3.begin(),LINE3.end()));
                    OutSink->Write(std::vector<char>(LINE4.begin(),LINE4.end()));
                    OutSink->Write(std::vector<char>(LINE5.begin(),LINE5.end()));
                    OutSink->Write(std::vector<char>(LINE6.begin(),LINE6.end()));
                    OutSink->Write(std::vector<char>(LINE7.begin(),LINE7.end()));
                    OutSink->Write(std::vector<char>(LINE8.begin(),LINE8.end()));
                    OutSink->Write(std::vector<char>(LINE9.begin(),LINE9.end()));
                    OutSink->Write(std::vector<char>(LINE10.begin(),LINE10.end()));
                    OutSink->Write(std::vector<char>(LINE11.begin(),LINE11.end()));
                    OutSink->Write(std::vector<char>(LINE12.begin(),LINE12.end()));
                }
                else if(CMDS[0] == "exit"){
                    break;
                }
                // output node count
                else if(CMDS[0] == "count"){
                    size_t Count = Planner->NodeCount();
                    std::string StrCount = std::to_string(Count) + " nodes\n";
                    OutSink->Write(std::vector<char>(StrCount.begin(),StrCount.end()));
                }
                // output node info
                else if(CMDS[0] == "node"){
                    if(CMDS.size() != 2){
                        std::string ErrMsg = "Invalid node command, see help.\n";
                        ErrSink->Write(std::vector<char>(ErrMsg.begin(),ErrMsg.end()));
                        continue;
                    }
                    try{
                        size_t NodeIndex = std::stoull(CMDS[1]);
                        CStreetMap::TNodeID NodeID = Planner->SortedNodeByIndex(NodeIndex)->ID();
                        CStreetMap::SLocation Location = Planner->SortedNodeByIndex(NodeIndex)->Location();
                        // double LatInDegree = Location.DLatitude;
                        // double LonInDegree = Location.DLongitude;
                        std::string LocationInDMS = SGeographicUtils::ConvertLLToDMS(Location);
                        std::string ResultStr = "Node " + CMDS[1] + ": id = " + std::to_string(NodeID) + " is at " + LocationInDMS + "\n";
                        OutSink->Write(std::vector<char>(ResultStr.begin(),ResultStr.end()));
                    }
                    // Invalid input - other than number
                    catch(...){
                        std::string ErrMsg = "Invalid node parameter, see help.\n";
                        ErrSink->Write(std::vector<char>(ErrMsg.begin(),ErrMsg.end()));
                        continue;
                    }
                }
                else if(CMDS[0] == "fastest"){
                    
                    // invalid parameter coiunt
                    if(CMDS.size() != 3){
                        std::string ErrMsg = "Invalid fastest command, see help.\n";
                        ErrSink->Write(std::vector<char>(ErrMsg.begin(),ErrMsg.end()));
                        continue;
                    }
                    try{
                        CStreetMap::TNodeID SrcID = std::stoull(CMDS[1]);
                        CStreetMap::TNodeID DestID = std::stoull(CMDS[2]);
                        PathSrcID = SrcID;
                        PathDestID = DestID;

                        double TimeInHours = Planner->FindFastestPath(SrcID,DestID,Path);
    
                        if(TimeInHours != NoPathExists){
                            // update path type and info
                            Fastest = true;
                            Shortest = false;
                            PathTime = TimeInHours;
                            PathDistance = -1;
                            // Calculate time in h/m/s 
                            int TimeInSeconds = int(TimeInHours * 3600);
                            int Hours = TimeInSeconds / 3600;
                            int Minutes = (TimeInSeconds % 3600) / 60;
                            int Seconds = TimeInSeconds % 60;
                            std::string ResultStr = "Fastest path takes ";
                            // if one of h/m/s is 0, do not include it. 
                            if(Hours > 0) {
                                if(Minutes > 0 || Seconds > 0){
                                    ResultStr += std::to_string(Hours) + " hr ";
                                }
                                else{
                                    ResultStr += std::to_string(Hours) + " hr";
                                }
                            }
                            if(Minutes > 0) {
                                if(Seconds > 0){
                                    ResultStr += std::to_string(Minutes) + " min ";// if we only do this, we could add an additional space berfore "." if seconds = 0
                                }
                                else{
                                    ResultStr += std::to_string(Minutes) + " min";// Take care of space behavior 
                                }
                            }
                            if(Seconds > 0) {
                                ResultStr += std::to_string(Seconds) + " sec";
                            }
                            ResultStr += ".\n";
                            OutSink->Write(std::vector<char>(ResultStr.begin(),ResultStr.end()));
                        }
                        else{
                            Path.clear();
                            // No path exist, clear the old value incase print will print the previously saved path
                            std::string ErrMsg = "No valid path exists.\n";
                            ErrSink->Write(std::vector<char>(ErrMsg.begin(), ErrMsg.end()));
                            continue;
                        }
                        
                    }
                    // conversion failed, invalid parameter
                    catch(...){
                        std::string ErrMsg = "Invalid fastest parameter, see help.\n";
                        ErrSink->Write(std::vector<char>(ErrMsg.begin(),ErrMsg.end()));
                        continue;
                    }  
                }
                else if(CMDS[0] == "shortest"){
                    // invalid parameter coiunt
                    if(CMDS.size() != 3){
                        std::string ErrMsg = "Invalid shortest command, see help.\n";
                        ErrSink->Write(std::vector<char>(ErrMsg.begin(),ErrMsg.end()));
                        continue;
                    }
                    try{
                        CStreetMap::TNodeID SrcID = std::stoull(CMDS[1]);
                        CStreetMap::TNodeID DestID = std::stoull(CMDS[2]);
                        PathSrcID = SrcID;
                        PathDestID = DestID;

                        std::vector<CStreetMap::TNodeID> path;
                        double Distance = Planner->FindShortestPath(SrcID,DestID,path);

                        Path.clear();
                        if(Distance != NoPathExists){
                            // update path type and info
                            Shortest = true;
                            Fastest = false;
                            PathTime = -1;
                            PathDistance = Distance;
                            //std::string DistanceStr = std::format("{:.1f}",Distance);// Only keep 1 digit CSIF doesn't support this
                            std::ostringstream oss;
                            oss << std::fixed << std::setprecision(1) << Distance;
                            std::string DistanceStr = oss.str();
                            std::string ResultStr = "Shortest path is " + DistanceStr + " mi.\n";
                            OutSink->Write(std::vector<char>(ResultStr.begin(),ResultStr.end()));
                            // update Path assume all walk
                            for(auto NodeID : path){
                                Path.push_back({CTransportationPlanner::ETransportationMode::Walk, NodeID});
                            } 
                        }
                        else{
                            // No path exist, clear the old value incase print will print the previously saved path
                            std::string ErrMsg = "No valid path exists.\n";
                            ErrSink->Write(std::vector<char>(ErrMsg.begin(), ErrMsg.end()));
                            continue;
                        }
                    }
                    // conversion failed, invalid parameter
                    catch(...){
                        std::string ErrMsg = "Invalid shortest parameter, see help.\n";
                        ErrSink->Write(std::vector<char>(ErrMsg.begin(),ErrMsg.end()));
                        continue;
                    }  
                }
                else if(CMDS[0] == "save"){
                    if(Path.empty()){
                        std::string ErrMsg = "No valid path to save, see help.\n";
                        ErrSink->Write(std::vector<char>(ErrMsg.begin(),ErrMsg.end()));
                        continue;
                    }
                    else{
                        std::string FileName;
                        if(Shortest){
                            FileName = std::to_string(PathSrcID) + '_' + std::to_string(PathDestID) + '_' + std::to_string(PathDistance) + "mi.csv";
                        }

                        if(Fastest){
                            FileName = std::to_string(PathSrcID) + '_' + std::to_string(PathDestID) + '_' + std::to_string(PathTime) + "hr.csv";
                        }
                        // Create a new file sink
                        auto FileSink = Results->CreateSink(FileName);
                        // write into filesink
                        // Header
                        std::string Header =  "mode,node_id\n";
                        FileSink->Write(std::vector<char>(Header.begin(),Header.end()));
                        for(size_t i = 0; i < Path.size(); i++){
                            std::string Mode;
                            CStreetMap::TNodeID NodeID = Path[i].second; 
                            auto TransMode = Path[i].first;
                            if(TransMode == CTransportationPlanner::ETransportationMode::Walk){
                                Mode = "Walk";
                            }
                            if(TransMode == CTransportationPlanner::ETransportationMode::Bike){
                                Mode = "Bike";
                            }
                            if(TransMode == CTransportationPlanner::ETransportationMode::Bus){
                                Mode = "Bus";
                            }

                            std::string Row = Mode + ',' + std::to_string(NodeID);
                            // Add '\n' only if not last line
                            if(i != Path.size() - 1){
                                Row += '\n';
                            }
                            FileSink->Write(std::vector<char>(Row.begin(),Row.end()));
                        }
                        // Output to outputsink
                        std::string Message = "Path saved to <results>/" + FileName + "\n";
                        OutSink->Write(std::vector<char>(Message.begin(),Message.end()));
                    }
                }
                else if(CMDS[0] == "print"){
                    // No saved path
                    if(Shortest == false && Fastest == false){
                        std::string ErrMsg = "No valid path to print, see help.\n";
                        ErrSink->Write(std::vector<char>(ErrMsg.begin(), ErrMsg.end()));
                        continue;
                    }
                    std::vector< std::string > Description;
                    if(Planner->GetPathDescription(Path,Description)){
                        for(auto Row : Description){
                            OutSink->Write(std::vector<char>(Row.begin(), Row.end()));
                            OutSink->Put('\n');
                        }
                    }
                }
                else{
                    std::string ErrMsg = "Unknown command \"" + CMDS[0] + "\" type help for help.\n";
                    ErrSink->Write(std::vector<char>(ErrMsg.begin(), ErrMsg.end()));
                    continue;
                }
                
            }
            else{
                // nothing to read
                break;
            }
        }  
        return true;
    }
};

// CTransportationPlannerCommandLine member functions
// Constructor for the Transportation Planner Command Line 
CTransportationPlannerCommandLine::CTransportationPlannerCommandLine(
    std::shared_ptr<CDataSource> cmdsrc, 
    std::shared_ptr<CDataSink> outsink,
    std::shared_ptr<CDataSink> errsink, 
    std::shared_ptr<CDataFactory> results, 
    std::shared_ptr<CTransportationPlanner> planner){
        DImplementation = std::make_unique<SImplementation>(cmdsrc,outsink,errsink,results,planner);
    }
// Destructor for the Transportation Planner Command Line 
CTransportationPlannerCommandLine::~CTransportationPlannerCommandLine(){
    
}
// Processes the commands input to the 
bool CTransportationPlannerCommandLine::ProcessCommands(){
    return DImplementation->ProcessCommands();
}