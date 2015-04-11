#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "coord.hpp"

SCENARIO( "coords can converted between forms", "[coord]" ) {
    GIVEN( "A default world_coord" ) {
        voxel_grid::world_coord coord_w;
        REQUIRE( coord_w.x == 0 );
        REQUIRE( coord_w.y == 0 );
        REQUIRE( coord_w.z == 0 );

        WHEN( "converted to chunk coords" ) {
            auto chunk_pair = voxel_grid::from_world_coord(coord_w);

            THEN( "chunk coord is correct" ) {
                REQUIRE( chunk_pair.first.x == 0 );
                REQUIRE( chunk_pair.first.y == 0 );
                REQUIRE( chunk_pair.first.z == 0 );
            }
            AND_THEN( "chunk coord is correct" ) {
                REQUIRE( chunk_pair.second.x == 0 );
                REQUIRE( chunk_pair.second.y == 0 );
                REQUIRE( chunk_pair.second.z == 0 );
            }
        }
    }
}