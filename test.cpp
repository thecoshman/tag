#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "voxel_grid/coord.hpp"

SCENARIO( "coords can converted between forms", "[coord]" ) {
    GIVEN( "default world_coord" ) {
        voxel_grid::world_coord coord_w;
        REQUIRE( coord_w.x == 0 );
        REQUIRE( coord_w.y == 0 );
        REQUIRE( coord_w.z == 0 );

        WHEN( "converted to chunk coords" ) {
            auto chunk_pair = voxel_grid::from_world_coord(coord_w);

            THEN( "chunk_coord is correct" ) {
                REQUIRE( chunk_pair.first.x == 0 );
                REQUIRE( chunk_pair.first.y == 0 );
                REQUIRE( chunk_pair.first.z == 0 );
            }
            AND_THEN( "intra-chunk_coord is correct" ) {
                REQUIRE( chunk_pair.second.x == 0 );
                REQUIRE( chunk_pair.second.y == 0 );
                REQUIRE( chunk_pair.second.z == 0 );
            }
        }
    }

    GIVEN( "world_coord at chunk limit" ) {
        int size = voxel_grid::chunk_size - 1;
        voxel_grid::world_coord coord_w(size, size, size);
        REQUIRE( coord_w.x == size );
        REQUIRE( coord_w.y == size );
        REQUIRE( coord_w.z == size );

        WHEN( "converted to chunk coords" ) {
            auto chunk_pair = voxel_grid::from_world_coord(coord_w);

            THEN( "chunk_coord is correct" ) {
                REQUIRE( chunk_pair.first.x == 0 );
                REQUIRE( chunk_pair.first.y == 0 );
                REQUIRE( chunk_pair.first.z == 0 );
            }
            AND_THEN( "intra-chunk_coord is correct" ) {
                REQUIRE( chunk_pair.second.x == size );
                REQUIRE( chunk_pair.second.y == size );
                REQUIRE( chunk_pair.second.z == size );
            }
        }
    }

    GIVEN( "world_coord at chunk start" ) {
        int size = voxel_grid::chunk_size;
        voxel_grid::world_coord coord_w(size, size, size);
        REQUIRE( coord_w.x == size );
        REQUIRE( coord_w.y == size );
        REQUIRE( coord_w.z == size );

        WHEN( "converted to chunk_coords" ) {
            auto chunk_pair = voxel_grid::from_world_coord(coord_w);

            THEN( "chunk_coord is correct" ) {
                REQUIRE( chunk_pair.first.x == 1 );
                REQUIRE( chunk_pair.first.y == 1 );
                REQUIRE( chunk_pair.first.z == 1 );
            }
            AND_THEN( "intra-chunk_coord is correct" ) {
                REQUIRE( chunk_pair.second.x == 0 );
                REQUIRE( chunk_pair.second.y == 0 );
                REQUIRE( chunk_pair.second.z == 0 );
            }
        }
    }

    GIVEN( "default chunk_coord" ) {
        voxel_grid::chunk_coord coord_c;
        REQUIRE( coord_c.x == 0 );
        REQUIRE( coord_c.y == 0 );
        REQUIRE( coord_c.z == 0 );

        WHEN( "converted to a world_coord" ) {
            auto coord_w = voxel_grid::to_world_coord(coord_c);

            THEN( "world_coord is correct" ) {
                REQUIRE( coord_w.x == 0 );
                REQUIRE( coord_w.y == 0 );
                REQUIRE( coord_w.z == 0 );
            }
        }
    }

    GIVEN( "negative chunk_coord" ) {
        voxel_grid::chunk_coord coord_c(-1,-1,-1);
        REQUIRE( coord_c.x == -1 );
        REQUIRE( coord_c.y == -1 );
        REQUIRE( coord_c.z == -1 );

        WHEN( "converted to a world_coord" ) {
            auto coord_w = voxel_grid::to_world_coord(coord_c);

            THEN( "world_coord is correct" ) {
                int size = -voxel_grid::chunk_size;
                REQUIRE( coord_w.x == size );
                REQUIRE( coord_w.y == size );
                REQUIRE( coord_w.z == size );
            }
        }
    }

    GIVEN( "very negative chunk_coord" ) {
        voxel_grid::chunk_coord coord_c(-100,-100,-100);
        REQUIRE( coord_c.x == -100 );
        REQUIRE( coord_c.y == -100 );
        REQUIRE( coord_c.z == -100 );

        WHEN( "converted to a world_coord" ) {
            auto coord_w = voxel_grid::to_world_coord(coord_c);

            THEN( "world_coord is correct" ) {
                int size = -voxel_grid::chunk_size * 100;
                REQUIRE( coord_w.x == size );
                REQUIRE( coord_w.y == size );
                REQUIRE( coord_w.z == size );
            }
        }
    }

    GIVEN( "default chunk_coord and intra_chunk_coord" ) {
        voxel_grid::chunk_coord coord_c;
        voxel_grid::intra_chunk_coord coord_i;
        REQUIRE( coord_c.x == 0 );
        REQUIRE( coord_c.y == 0 );
        REQUIRE( coord_c.z == 0 );
        REQUIRE( coord_i.x == 0 );
        REQUIRE( coord_i.y == 0 );
        REQUIRE( coord_i.z == 0 );

        WHEN( "converted to a world_coord" ) {
            auto coord_w = voxel_grid::to_world_coord(coord_c);

            THEN( "world_coord is correct" ) {
                REQUIRE( coord_w.x == 0 );
                REQUIRE( coord_w.y == 0 );
                REQUIRE( coord_w.z == 0 );
            }
        }
    }

    GIVEN( "negative chunk_coord and intra_chunk_coord" ) {
        voxel_grid::chunk_coord coord_c(-1,-1,-1);
        REQUIRE( coord_c.x == -1 );
        REQUIRE( coord_c.y == -1 );
        REQUIRE( coord_c.z == -1 );

        WHEN( "converted to a world_coord" ) {
            auto coord_w = voxel_grid::to_world_coord(coord_c);

            THEN( "world_coord is correct" ) {
                int size = -voxel_grid::chunk_size;
                REQUIRE( coord_w.x == size );
                REQUIRE( coord_w.y == size );
                REQUIRE( coord_w.z == size );
            }
        }
    }
}