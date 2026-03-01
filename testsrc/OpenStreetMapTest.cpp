#include <gtest/gtest.h>
#include "OpenStreetMap.h"
#include "StringDataSource.h"

using TNodeID = uint64_t; 
using TWayID = uint64_t; 
static const TNodeID InvalidNodeID = std::numeric_limits<TNodeID>::max();
static const TWayID InvalidWayID = std::numeric_limits<TWayID>::max();

TEST(OSMTest, ComprehensiveFile){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                                "       <tag k=\"highway\" v=\"traffic_signals\"/>"
	                                                            "  </node>"
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Hacienda Avenue\"/>"
                                                                "  </way>\n"
                                                                "  <way id=\"101\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Donatello Court\"/>"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(),2);
    EXPECT_EQ(OpenStreetMap.WayCount(),2);
    EXPECT_NE(OpenStreetMap.NodeByIndex(0),nullptr);
    EXPECT_NE(OpenStreetMap.NodeByIndex(1),nullptr);
    EXPECT_EQ(OpenStreetMap.NodeByIndex(2),nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(0),nullptr);
    EXPECT_NE(OpenStreetMap.WayByIndex(1),nullptr);
    EXPECT_EQ(OpenStreetMap.WayByIndex(2),nullptr);                                                            
    //////////////////////////Node Function Test////////////////////////////////
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(OpenStreetMap.NodeByID(3),nullptr);
    EXPECT_EQ(TempNode->ID(),1);
    EXPECT_EQ(TempNode->Location(),CStreetMap::SLocation(38.5,-121.7));/////这里也要改
    EXPECT_EQ(TempNode->AttributeCount(),1);
    EXPECT_EQ(TempNode->GetAttributeKey(0),"highway");
    EXPECT_EQ(TempNode->GetAttributeKey(1),"");
    EXPECT_TRUE(TempNode->HasAttribute("highway"));
    EXPECT_EQ(TempNode->GetAttribute("highway"),"traffic_signals");
    EXPECT_EQ(TempNode->GetAttribute("invalidkey"),"");

    TempNode = OpenStreetMap.NodeByIndex(1);
    ASSERT_NE(TempNode,nullptr);
    EXPECT_EQ(TempNode, OpenStreetMap.NodeByID(TempNode->ID()));
    EXPECT_EQ(TempNode->ID(),2);
    EXPECT_EQ(TempNode->Location(),CStreetMap::SLocation(38.5,-121.8));
    EXPECT_EQ(TempNode->AttributeCount(),0);
    EXPECT_EQ(TempNode->GetAttributeKey(0),"");
    EXPECT_FALSE(TempNode->HasAttribute("highway"));
    EXPECT_EQ(TempNode->GetAttribute("invalidkey"),"");

    //////////////////////////Way Function Test////////////////////////////////
    auto TempWay = OpenStreetMap.WayByIndex(0);
    ASSERT_NE(TempWay,nullptr);
    EXPECT_EQ(TempWay, OpenStreetMap.WayByID(TempWay->ID()));
    EXPECT_EQ(OpenStreetMap.WayByID(1),nullptr);
    EXPECT_EQ(TempWay->ID(),100);
    EXPECT_EQ(TempWay->NodeCount(),2);
    EXPECT_EQ(TempWay->GetNodeID(0),1);
    EXPECT_EQ(TempWay->GetNodeID(1),2);
    EXPECT_EQ(TempWay->GetNodeID(2),InvalidNodeID);
    EXPECT_EQ(TempWay->AttributeCount(),2);
    EXPECT_EQ(TempWay->GetAttributeKey(0),"highway");
    EXPECT_EQ(TempWay->GetAttributeKey(1),"name");
    EXPECT_EQ(TempWay->GetAttributeKey(2),"");
    EXPECT_TRUE(TempWay->HasAttribute("highway"));
    EXPECT_TRUE(TempWay->HasAttribute("name"));
    EXPECT_FALSE(TempWay->HasAttribute("InvalidAttributeKey"));
    EXPECT_EQ(TempWay->GetAttribute("highway"),"residential");
    EXPECT_EQ(TempWay->GetAttribute("name"),"Hacienda Avenue");
    EXPECT_EQ(TempWay->GetAttribute("InvalidAttributeKey"),"");

    TempWay = OpenStreetMap.WayByIndex(1);
    ASSERT_NE(TempWay,nullptr);
    EXPECT_EQ(TempWay, OpenStreetMap.WayByID(TempWay->ID()));
    EXPECT_EQ(TempWay->ID(),101);
    EXPECT_EQ(TempWay->NodeCount(),2);
    EXPECT_EQ(TempWay->GetNodeID(0),1);
    EXPECT_EQ(TempWay->GetNodeID(1),2);
    EXPECT_EQ(TempWay->GetNodeID(2),InvalidNodeID);
    EXPECT_EQ(TempWay->AttributeCount(),2);
    EXPECT_EQ(TempWay->GetAttributeKey(0),"highway");
    EXPECT_EQ(TempWay->GetAttributeKey(1),"name");
    EXPECT_EQ(TempWay->GetAttributeKey(2),"");
    EXPECT_TRUE(TempWay->HasAttribute("highway"));
    EXPECT_TRUE(TempWay->HasAttribute("name"));
    EXPECT_FALSE(TempWay->HasAttribute("InvalidAttributeKey"));
    EXPECT_EQ(TempWay->GetAttribute("highway"),"residential");
    EXPECT_EQ(TempWay->GetAttribute("name"),"Donatello Court");
    EXPECT_EQ(TempWay->GetAttribute("InvalidAttributeKey"),"");
}

//Cover return false of ParseOpenStreetMap()
TEST(OSMTest, EmptyFiles){
    auto OSMDataSource = std::make_shared< CStringDataSource >("");
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
}                                                                

TEST(OSMTest, MissingHeaders){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                                "       <tag k=\"highway\" v=\"traffic_signals\"/>"
	                                                            "  </node>"
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Hacienda Avenue\"/>"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
}

TEST(OSMTest, InvalidNodeForm){
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
	                                                            "  <node id=\"1\" lat=\"abc\" lon=\"def\">\n"
                                                                "       <tag k=\"highway\" v=\"traffic_signals\"/>"
	                                                            "  </node>"
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Hacienda Avenue\"/>"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    EXPECT_EQ(OpenStreetMap.NodeCount(), 0);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
}

TEST(OSMTest, DuplicateNodeIDs) {
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                                "       <tag k=\"highway\" v=\"traffic_signals\"/>"
	                                                            "  </node>"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Hacienda Avenue\"/>"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    EXPECT_EQ(OpenStreetMap.NodeCount(), 0); // duplicate node clears everything
}

TEST(OSMTest, DuplicateWayIDs) {
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                                "       <tag k=\"highway\" v=\"traffic_signals\"/>"
	                                                            "  </node>"
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Hacienda Avenue\"/>"
                                                                "  </way>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Donatello Court\"/>"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    EXPECT_EQ(OpenStreetMap.WayCount(), 0); // duplicate way clears everything
}

TEST(OSMTest, WayWithInvalidNodeID) {
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"abc\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Hacienda Avenue\"/>"
                                                                "  </way>\n"
                                                                "</osm>");
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 0); 
    EXPECT_EQ(OpenStreetMap.WayCount(), 0); // duplicate way clears everything
}

TEST(OSMTest, DuplicateAttributes) {
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                                "       <tag k=\"highway\" v=\"traffic_signals\"/>"
                                                                "       <tag k=\"highway\" v=\"residential\"/>"
	                                                            "  </node>"
                                                                "  <way id=\"101\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Donatello Court\"/>"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

    EXPECT_EQ(OpenStreetMap.NodeCount(), 0); // parsing fails
    EXPECT_EQ(OpenStreetMap.WayCount(), 0);
}

TEST(OSMTest, NodeWithNoAttributes) {
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"                                               
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Hacienda Avenue\"/>"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    
    ASSERT_EQ(OpenStreetMap.NodeCount(), 1);
    auto TempNode = OpenStreetMap.NodeByIndex(0);
    EXPECT_EQ(TempNode->AttributeCount(), 0);
    EXPECT_EQ(TempNode->GetAttributeKey(0), "");
}
TEST(OSMTest, WayWithNoNodes) {
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                                "       <tag k=\"highway\" v=\"traffic_signals\"/>"
	                                                            "  </node>"
                                                                "  <way id=\"100\">\n"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
    
    ASSERT_EQ(OpenStreetMap.WayCount(), 1);
    auto TempWay = OpenStreetMap.WayByIndex(0);
    EXPECT_EQ(TempWay->NodeCount(), 0);
    EXPECT_EQ(TempWay->GetNodeID(0), InvalidNodeID);
}

TEST(OSMTest, FirstWayWithInvalidWayID) {
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                                "       <tag k=\"highway\" v=\"traffic_signals\"/>"
	                                                            "  </node>"
                                                                "  <way id=\"abc\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Hacienda Avenue\"/>"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);

}

TEST(OSMTest, DuplicateWayAttributes) {
    auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                                "       <tag k=\"highway\" v=\"traffic_signals\"/>"
	                                                            "  </node>"
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"                                                                
                                                                "  <way id=\"1\">\n"
		                                                        "    <nd ref=\"1\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"highway\" v=\"Hacienda Avenue\"/>"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
}

TEST(OSMTest, InvalidNodeIDInWays) {
     auto OSMDataSource = std::make_shared< CStringDataSource >("<?xml version='1.0' encoding='UTF-8'?>\n"
                                                                "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n"
	                                                            "  <node id=\"1\" lat=\"38.5\" lon=\"-121.7\">\n"
                                                                "       <tag k=\"highway\" v=\"traffic_signals\"/>"
	                                                            "  </node>"
	                                                            "  <node id=\"2\" lat=\"38.5\" lon=\"-121.8\"/>\n"
                                                                "  <way id=\"100\">\n"
		                                                        "    <nd ref=\"abc\"/>\n"
		                                                        "    <nd ref=\"2\"/>\n"
                                                                "    <tag k=\"highway\" v=\"residential\"/>"
		                                                        "    <tag k=\"name\" v=\"Hacienda Avenue\"/>"
                                                                "  </way>\n"
                                                                "</osm>"
                                                                );
    auto OSMReader = std::make_shared< CXMLReader >(OSMDataSource);
    COpenStreetMap OpenStreetMap(OSMReader);
}