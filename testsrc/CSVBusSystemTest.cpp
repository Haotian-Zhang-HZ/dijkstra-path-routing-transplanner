#include <gtest/gtest.h>
#include "CSVBusSystem.h"
#include "StringDataSource.h"

using TStopID = uint64_t;
using TWayID = uint64_t;
static const TStopID InvalidStopID = std::numeric_limits<TStopID>::max();

TEST(CSVBusSystem, SimpleFile){
    auto StopDataSource = std::make_shared< CStringDataSource >("stop_id,node_id\n"
                                                                "1,123\n"
                                                                "2,124");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("route,stop_id\n"
                                                                "A,1\n"
                                                                "A,2");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);
    
    EXPECT_EQ(BusSystem.StopCount(),2);
    EXPECT_EQ(BusSystem.RouteCount(), 1);

    auto StopObj = BusSystem.StopByIndex(0);
    EXPECT_NE(StopObj,nullptr);
    StopObj = BusSystem.StopByIndex(1);
    EXPECT_NE(StopObj,nullptr);
    StopObj = BusSystem.StopByID(1);
    ASSERT_NE(StopObj,nullptr);// we want to stop the test if stopObj is a nullptr because we will use StopObj->ID and will crash and raise seg fault if nullptr
    EXPECT_EQ(StopObj->ID(),1);
    EXPECT_EQ(StopObj->NodeID(),123);
    StopObj = BusSystem.StopByID(2);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),2);
    EXPECT_EQ(StopObj->NodeID(),124);

    EXPECT_EQ(BusSystem.StopByIndex(0)->ID(), 1);
    EXPECT_EQ(BusSystem.StopByIndex(1)->ID(), 2);

    auto RouteObj = BusSystem.RouteByIndex(0);
    ASSERT_NE(RouteObj, nullptr);
    EXPECT_EQ(RouteObj->Name(), "A");
    EXPECT_EQ(RouteObj->StopCount(), 2);
    EXPECT_EQ(RouteObj->GetStopID(0), 1);
    EXPECT_EQ(RouteObj->GetStopID(1), 2);
    RouteObj = BusSystem.RouteByName("A");
    ASSERT_NE(RouteObj, nullptr);
    EXPECT_EQ(RouteObj->Name(),"A");
    EXPECT_EQ(RouteObj->StopCount(), 2);
    EXPECT_EQ(RouteObj->GetStopID(0), 1);
    EXPECT_EQ(RouteObj->GetStopID(1), 2);
    EXPECT_EQ(RouteObj->GetStopID(2),InvalidStopID);

    //check consistency of StopByIndex and StopByID: will retrun the same stop obj ptr.
    auto StopObj1 = BusSystem.StopByIndex(0);
    auto StopObj2 = BusSystem.StopByID(1);
    EXPECT_EQ(StopObj1, StopObj2);

    //check consistency of RouteByIndex and RouteByName: will retrun the same route obj ptr.
    auto RouteObj1 = BusSystem.RouteByIndex(0);
    auto RouteObj2 = BusSystem.RouteByName("A");
    EXPECT_EQ(RouteObj1, RouteObj2);

}

TEST(CSVBusSystem, LargerFileFromProvidedData){
    auto StopDataSource = std::make_shared< CStringDataSource >("stop_id,node_id\n"
                                                                "22258,2849810514\n"
                                                                "22169,2849805223\n"
                                                                "22274,3954294407");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("route,stop_id\n"
                                                                "A,22258\n"
                                                                "A,22169\n"
                                                                "B,22274\n"
                                                                "B,22258\n"
                                                                "Z,22169\n"
                                                                "Z,22274");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);

    //////////////////////////Stops//////////////////////////
    EXPECT_EQ(BusSystem.StopCount(),3);
    auto StopObj = BusSystem.StopByIndex(0);
    EXPECT_NE(StopObj,nullptr);
    StopObj = BusSystem.StopByIndex(1);
    EXPECT_NE(StopObj,nullptr);
    StopObj = BusSystem.StopByIndex(2);
    EXPECT_NE(StopObj,nullptr);

    StopObj = BusSystem.StopByID(22258);
    ASSERT_NE(StopObj,nullptr);// we want to stop the test if stopObj is a nullptr because we will use StopObj->ID and will crash and raise seg fault if nullptr
    EXPECT_EQ(StopObj->ID(),22258);
    EXPECT_EQ(StopObj->NodeID(),2849810514);
    StopObj = BusSystem.StopByID(22169);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),22169);
    EXPECT_EQ(StopObj->NodeID(),2849805223);
    StopObj = BusSystem.StopByID(22274);
    ASSERT_NE(StopObj,nullptr);
    EXPECT_EQ(StopObj->ID(),22274);
    EXPECT_EQ(StopObj->NodeID(),3954294407);
    StopObj = BusSystem.StopByID(99999);
    EXPECT_EQ(StopObj, nullptr);

    // Verify that StopByIndex preserves the order in which stops
    // appear in the CSV file. The specification implies that
    // StopByIndex(i) should return the i-th stop read from the file.
    // If the implementation accidentally rebuilds the vector from
    // an unordered_map or otherwise loses insertion order,
    // these expectations would fail.

    EXPECT_EQ(BusSystem.StopByIndex(0)->ID(), 22258);
    EXPECT_EQ(BusSystem.StopByIndex(1)->ID(), 22169);
    EXPECT_EQ(BusSystem.StopByIndex(2)->ID(), 22274);

    ////////////////////////Routes////////////////////////////
    EXPECT_EQ(BusSystem.RouteCount(),3);
    auto RouteObj = BusSystem.RouteByIndex(0);
    EXPECT_NE(RouteObj,nullptr);
    RouteObj = BusSystem.RouteByIndex(1);
    EXPECT_NE(RouteObj,nullptr);
    RouteObj = BusSystem.RouteByIndex(2);
    EXPECT_NE(RouteObj,nullptr);

    // Verify that RouteByIndex preserves the order in which routes
    // appear in the CSV file. The specification implies that
    // RouteByIndex(i) should return the i-th route read from the file.
    // If the implementation accidentally rebuilds the vector from
    // an unordered_map or otherwise loses insertion order,
    // these expectations would fail.

    EXPECT_EQ(BusSystem.RouteByIndex(0)->Name(), "A");
    EXPECT_EQ(BusSystem.RouteByIndex(1)->Name(), "B");
    EXPECT_EQ(BusSystem.RouteByIndex(2)->Name(), "Z");

    RouteObj = BusSystem.RouteByName("A");
    ASSERT_NE(RouteObj,nullptr);
    EXPECT_EQ(RouteObj->Name(),"A");
    EXPECT_EQ(RouteObj->StopCount(),2);
    EXPECT_EQ(RouteObj->GetStopID(0),22258);
    EXPECT_EQ(RouteObj->GetStopID(1),22169);
    RouteObj = BusSystem.RouteByName("B");
    ASSERT_NE(RouteObj,nullptr);
    EXPECT_EQ(RouteObj->Name(),"B");
    EXPECT_EQ(RouteObj->StopCount(),2);
    EXPECT_EQ(RouteObj->GetStopID(0),22274);
    EXPECT_EQ(RouteObj->GetStopID(1),22258);
    RouteObj = BusSystem.RouteByName("Z");
    ASSERT_NE(RouteObj,nullptr);
    EXPECT_EQ(RouteObj->Name(),"Z");
    EXPECT_EQ(RouteObj->StopCount(),2);
    EXPECT_EQ(RouteObj->GetStopID(0),22169);
    EXPECT_EQ(RouteObj->GetStopID(1),22274);
    EXPECT_EQ(RouteObj->GetStopID(2), InvalidStopID);//test for index out of range
    RouteObj = BusSystem.RouteByName("");
    EXPECT_EQ(RouteObj, nullptr);

    EXPECT_EQ(BusSystem.RouteByName("C"),nullptr);//route name does not exist.
    
    
}


TEST(CSVBusSystem, EmptyStops) {
    auto StopDataSource = std::make_shared< CStringDataSource >("");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("wrong,header\n"
                                                                "A,1\n"
                                                                "A,2");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);

    EXPECT_EQ(BusSystem.StopCount(), 0);
    EXPECT_EQ(BusSystem.RouteCount(), 0);
    EXPECT_EQ(BusSystem.StopByIndex(0), nullptr);
    EXPECT_EQ(BusSystem.StopByID(1), nullptr);
    EXPECT_EQ(BusSystem.RouteByIndex(0), nullptr);
    EXPECT_EQ(BusSystem.RouteByName("A"), nullptr);
}


TEST(CSVBusSystem, WrongStopHeaders) {
    auto StopDataSource = std::make_shared< CStringDataSource >("wrong,header\n"
                                                                "1,123\n"
                                                                "2,124");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("wrong,header\n"
                                                                "A,1\n"
                                                                "A,2");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);

    EXPECT_EQ(BusSystem.StopCount(), 0);
    EXPECT_EQ(BusSystem.RouteCount(), 0);
    EXPECT_EQ(BusSystem.StopByIndex(0), nullptr);
    EXPECT_EQ(BusSystem.RouteByName("A"), nullptr);
}

//We must have valid stop header to readroute because in Simplemetation constructor, we have:
//if(ReadStops(stopsrc)){
//   ReadRoutes(routesrc);
//}
TEST(CSVBusSystem, WrongRouteHeaders) {
    auto StopDataSource = std::make_shared< CStringDataSource >("stop_id,node_id\n"
                                                                "1,123\n"
                                                                "2,124");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("wrong,header\n"
                                                                "A,1\n"
                                                                "A,2");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);

    EXPECT_EQ(BusSystem.StopCount(), 2);
    EXPECT_EQ(BusSystem.RouteCount(), 0);
    EXPECT_EQ(BusSystem.RouteByIndex(0), nullptr);
    EXPECT_EQ(BusSystem.RouteByName("A"), nullptr);
}

TEST(CSVBusSystem, EmptyRoute) {
    auto StopDataSource = std::make_shared< CStringDataSource >("stop_id,node_id\n"
                                                                "1,123\n"
                                                                "2,124");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);

    EXPECT_EQ(BusSystem.StopCount(), 2);
    EXPECT_EQ(BusSystem.RouteCount(), 0);
}

TEST(CSVBusSystem, InvalidIndexAndInvalidIDAndInvalidName){
    auto StopDataSource = std::make_shared< CStringDataSource >("stop_id,node_id\n"
                                                                "1,123\n"
                                                                "2,124");
    auto StopReader = std::make_shared< CDSVReader >(StopDataSource,',');
    auto RouteDataSource = std::make_shared< CStringDataSource >("route,stop_id\n"
                                                                "A,1\n"
                                                                "A,2");
    auto RouteReader = std::make_shared< CDSVReader >(RouteDataSource,',');
    
    CCSVBusSystem BusSystem(StopReader,RouteReader);

    auto StopObj = BusSystem.StopByIndex(2);
    EXPECT_EQ(StopObj,nullptr);

    StopObj = BusSystem.StopByID(3);
    EXPECT_EQ(StopObj,nullptr);

    auto RouteObj = BusSystem.RouteByIndex(2);
    EXPECT_EQ(RouteObj,nullptr);

    RouteObj = BusSystem.RouteByName("B");
    EXPECT_EQ(RouteObj,nullptr);
}

TEST(CSVBusSystem, DuplicateStopID){
    // Stop CSV has duplicatex stop_id=1
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                                "1,123\n"
                                                                "1,124\n");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource,',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                                "A,1\n"
                                                                "A,1");
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource,',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);
}

TEST(CSVBusSystem, InvalidStopID){
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                                "abc,123\n"  // invalid stop_id
                                                                "2,124");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource,',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                                "A,2\n"
                                                                "A,2");
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource,',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);

}
TEST(CSVBusSystem, RouteWithNonexistentStop){
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                                "1,100\n"
                                                                "2,101");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                                "A,1\n"
                                                                "A,999"   );
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);
}

TEST(CSVBusSystem, RouteWithInvalidStopIDString){
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                                "1,100\n"
                                                                "2,101");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                                "A,1\n"
                                                                "A,2\n"
                                                                "A,abc");
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);
}

TEST(CSVBusSystem, RouteWithIncompleteStopID){
    auto StopDataSource = std::make_shared<CStringDataSource>("stop_id,node_id\n"
                                                                "1,100\n"
                                                                "2,101");
    auto StopReader = std::make_shared<CDSVReader>(StopDataSource, ',');
    auto RouteDataSource = std::make_shared<CStringDataSource>("route,stop_id\n"
                                                                "A\n"
                                                                "A,3\n"
                                                                "A,abc"   );
    auto RouteReader = std::make_shared<CDSVReader>(RouteDataSource, ',');

    CCSVBusSystem BusSystem(StopReader, RouteReader);
}

///////////////////////////////Additional Test For Full Coverage////////////////////////
TEST(CSVBusSystem, BranchCoverageSupplement) {
    ////////////////////////////////
    // 1️⃣ ReadStops invalid header
    auto StopDataSource1 = std::make_shared<CStringDataSource>("wrong,header\n1,123");
    auto StopReader1 = std::make_shared<CDSVReader>(StopDataSource1, ',');
    auto RouteDataSource1 = std::make_shared<CStringDataSource>("route,stop_id\nA,1");
    auto RouteReader1 = std::make_shared<CDSVReader>(RouteDataSource1, ',');

    CCSVBusSystem BusSystem1(StopReader1, RouteReader1);
    EXPECT_EQ(BusSystem1.StopCount(), 0); // triggers DStopsByIndex.clear() & DStopsByID.clear()
    EXPECT_EQ(BusSystem1.RouteCount(), 0);

    ////////////////////////////////
    // 2️⃣ ReadStops invalid stop_id (trigger catch)
    auto StopDataSource2 = std::make_shared<CStringDataSource>("stop_id,node_id\nabc,100");
    auto StopReader2 = std::make_shared<CDSVReader>(StopDataSource2, ',');
    auto RouteDataSource2 = std::make_shared<CStringDataSource>("route,stop_id\nA,1");
    auto RouteReader2 = std::make_shared<CDSVReader>(RouteDataSource2, ',');

    CCSVBusSystem BusSystem2(StopReader2, RouteReader2);
    EXPECT_EQ(BusSystem2.StopCount(), 0); // triggers catch

    ////////////////////////////////
    // 3️⃣ ReadRoutes invalid header
    auto StopDataSource3 = std::make_shared<CStringDataSource>("stop_id,node_id\n1,100");
    auto StopReader3 = std::make_shared<CDSVReader>(StopDataSource3, ',');
    auto RouteDataSource3 = std::make_shared<CStringDataSource>("wrong,header\nA,1");
    auto RouteReader3 = std::make_shared<CDSVReader>(RouteDataSource3, ',');

    CCSVBusSystem BusSystem3(StopReader3, RouteReader3);
    EXPECT_EQ(BusSystem3.RouteCount(), 0); // triggers DRouteByIndex.clear() & DRouteByName.clear()

    ////////////////////////////////
    // 4️⃣ ReadRoutes line too short
    auto StopDataSource4 = std::make_shared<CStringDataSource>("stop_id,node_id\n1,100");
    auto StopReader4 = std::make_shared<CDSVReader>(StopDataSource4, ',');
    auto RouteDataSource4 = std::make_shared<CStringDataSource>("route,stop_id\nA"); // missing stop_id
    auto RouteReader4 = std::make_shared<CDSVReader>(RouteDataSource4, ',');

    CCSVBusSystem BusSystem4(StopReader4, RouteReader4);
    EXPECT_EQ(BusSystem4.RouteCount(), 0); // triggers DRouteByIndex.clear() & DRouteByName.clear()

    ////////////////////////////////
    // 5️⃣ ReadRoutes nonexistent stop_id
    auto StopDataSource5 = std::make_shared<CStringDataSource>("stop_id,node_id\n1,100");
    auto StopReader5 = std::make_shared<CDSVReader>(StopDataSource5, ',');
    auto RouteDataSource5 = std::make_shared<CStringDataSource>("route,stop_id\nA,999"); // 999 does not exist
    auto RouteReader5 = std::make_shared<CDSVReader>(RouteDataSource5, ',');

    CCSVBusSystem BusSystem5(StopReader5, RouteReader5);
    EXPECT_EQ(BusSystem5.RouteCount(), 0); // triggers DRouteByIndex.clear() & DRouteByName.clear()

    ////////////////////////////////
    // 6️⃣ ReadRoutes invalid stop_id string (trigger catch)
    auto StopDataSource6 = std::make_shared<CStringDataSource>("stop_id,node_id\n1,100");
    auto StopReader6 = std::make_shared<CDSVReader>(StopDataSource6, ',');
    auto RouteDataSource6 = std::make_shared<CStringDataSource>("route,stop_id\nA,abc"); // invalid string
    auto RouteReader6 = std::make_shared<CDSVReader>(RouteDataSource6, ',');

    CCSVBusSystem BusSystem6(StopReader6, RouteReader6);
    EXPECT_EQ(BusSystem6.RouteCount(), 0); // triggers catch

    ////////////////////////////////
    // 7️⃣ RouteByName returns non-nullptr branch
    auto StopDataSource7 = std::make_shared<CStringDataSource>("stop_id,node_id\n1,100");
    auto StopReader7 = std::make_shared<CDSVReader>(StopDataSource7, ',');
    auto RouteDataSource7 = std::make_shared<CStringDataSource>("route,stop_id\nA,1");
    auto RouteReader7 = std::make_shared<CDSVReader>(RouteDataSource7, ',');

    CCSVBusSystem BusSystem7(StopReader7, RouteReader7);
    auto RouteObj = BusSystem7.RouteByName("A");
    ASSERT_NE(RouteObj, nullptr);      // triggers return it->second branch
    EXPECT_EQ(RouteObj->Name(), "A");
    EXPECT_EQ(RouteObj->StopCount(), 1);

    ////////////////////////////////
    // 8️⃣ StopByIndex nullptr branch
    auto StopObj = BusSystem7.StopByIndex(2); // index out of range
    EXPECT_EQ(StopObj, nullptr);

    ////////////////////////////////
    // 9️⃣ StopByID nullptr branch
    StopObj = BusSystem7.StopByID(999); // non-existent stop_id
    EXPECT_EQ(StopObj, nullptr);
}