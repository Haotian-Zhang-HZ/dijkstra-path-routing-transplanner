#include <gtest/gtest.h>
#include "DijkstraPathRouter.h"

TEST(DijkstraPathRouter, SimpleTest){
    CDijkstraPathRouter PathRouter;

    EXPECT_EQ(PathRouter.VertexCount(),0);
    auto VertexID = PathRouter.AddVertex(std::string("foo"));
    EXPECT_EQ(PathRouter.VertexCount(),1);
    auto VetextTag = std::any_cast<std::string>(PathRouter.GetVertexTag(VertexID));
    EXPECT_EQ(VetextTag,"foo");
    
}

TEST(DijkstraPathRouter, ShortestPath){
    CDijkstraPathRouter PathRouter;
    /*
                5   
       A ---> B --->C
          4  2|  ___^
              V / 1
              D
    
    */
    auto VertexA = PathRouter.AddVertex(std::string("A"));
    auto VertexB = PathRouter.AddVertex(std::string("B"));
    auto VertexC = PathRouter.AddVertex(std::string("C"));
    auto VertexD = PathRouter.AddVertex(std::string("D"));
    EXPECT_EQ(PathRouter.VertexCount(),4);
    EXPECT_TRUE(PathRouter.AddEdge(VertexA,VertexB,4.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB,VertexC,5.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexB,VertexD,2.0));
    EXPECT_TRUE(PathRouter.AddEdge(VertexD,VertexC,1.0));
    std::vector<CPathRouter::TVertexID> Path;
    PathRouter.Precompute(std::chrono::steady_clock::now() + std::chrono::seconds(10));
    EXPECT_EQ(PathRouter.FindShortestPath(VertexA,VertexC,Path), 7.0);
    std::vector<CPathRouter::TVertexID> ExpectedPath{VertexA,VertexB,VertexD,VertexC};
    EXPECT_EQ(Path,ExpectedPath);
    
}

// GetVertexTag with invalid ID
TEST(DijkstraPathRouter, GetVertexTagInvalidID){
    CDijkstraPathRouter PathRouter;
    auto Vertex = PathRouter.AddVertex("X");
    auto Tag = PathRouter.GetVertexTag(100); // out of bounds
    EXPECT_FALSE(Tag.has_value()); // should return empty any
}

// AddEdge with invalid src/dest and negative weight
TEST(DijkstraPathRouter, AddEdgeInvalid){
    CDijkstraPathRouter PathRouter;
    auto A = PathRouter.AddVertex("A");
    auto B = PathRouter.AddVertex("B");

    EXPECT_FALSE(PathRouter.AddEdge(100, B, 1.0));  // invalid src
    EXPECT_FALSE(PathRouter.AddEdge(A, 100, 1.0));  // invalid dest
    EXPECT_FALSE(PathRouter.AddEdge(A, B, -5.0));   // negative weight
}

// AddEdge bidirectional
TEST(DijkstraPathRouter, AddEdgeBidirectional){
    CDijkstraPathRouter PathRouter;
    auto A = PathRouter.AddVertex("A");
    auto B = PathRouter.AddVertex("B");

    EXPECT_TRUE(PathRouter.AddEdge(A, B, 3.0, true));
    std::vector<CPathRouter::TVertexID> Path;
    PathRouter.Precompute(std::chrono::steady_clock::now() + std::chrono::seconds(30));

    EXPECT_EQ(PathRouter.FindShortestPath(A, B, Path), 3.0);
    EXPECT_EQ(PathRouter.FindShortestPath(B, A, Path), 3.0);
}

// FindShortestPath no path exists
TEST(DijkstraPathRouter, NoPath){
    CDijkstraPathRouter PathRouter;
    auto A = PathRouter.AddVertex("A");
    auto B = PathRouter.AddVertex("B");
    auto C = PathRouter.AddVertex("C");

    EXPECT_TRUE(PathRouter.AddEdge(A, B, 1.0)); // C disconnected
    std::vector<CPathRouter::TVertexID> Path;
    PathRouter.Precompute(std::chrono::steady_clock::now() + std::chrono::seconds(30));

    EXPECT_EQ(PathRouter.FindShortestPath(A, C, Path), CPathRouter::NoPathExists);
    EXPECT_TRUE(Path.empty());
}

// Loop and self-edge
TEST(DijkstraPathRouter, SelfLoop){
    CDijkstraPathRouter PathRouter;
    auto A = PathRouter.AddVertex("A");
    EXPECT_TRUE(PathRouter.AddEdge(A, A, 2.0)); // self loop
    std::vector<CPathRouter::TVertexID> Path;
    PathRouter.Precompute(std::chrono::steady_clock::now() + std::chrono::seconds(30));
    EXPECT_EQ(PathRouter.FindShortestPath(A, A, Path), 0.0);
    EXPECT_EQ(Path.size(), 1);
}

// Multiple shortest paths (same distance)
TEST(DijkstraPathRouter, MultipleShortestPaths){
    CDijkstraPathRouter PathRouter;
    /*
        A
       / \
      B   C - E
       \ /
        D
    */
    auto A = PathRouter.AddVertex("A");
    auto B = PathRouter.AddVertex("B");
    auto C = PathRouter.AddVertex("C");
    auto D = PathRouter.AddVertex("D");
    auto E = PathRouter.AddVertex("E");

    EXPECT_TRUE(PathRouter.AddEdge(A,B,1.0));
    EXPECT_TRUE(PathRouter.AddEdge(A,C,1.0));
    EXPECT_TRUE(PathRouter.AddEdge(B,D,1.0));
    EXPECT_TRUE(PathRouter.AddEdge(C,D,1.0));
    EXPECT_TRUE(PathRouter.AddEdge(C,E,1.0));

    std::vector<CPathRouter::TVertexID> Path;
    PathRouter.Precompute(std::chrono::steady_clock::now() + std::chrono::seconds(30));

    EXPECT_EQ(PathRouter.FindShortestPath(A,D,Path), 2.0);
    EXPECT_TRUE((Path == std::vector<CPathRouter::TVertexID>{A,B,D} || Path == std::vector<CPathRouter::TVertexID>{A,C,D}));
}

