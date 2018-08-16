#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "tag/voxel_grid/coord.hpp"
#include "tag/block_instance.hpp"

SCENARIO( "coords can converted between forms", "[coord]" ) {
    GIVEN( "default world_coord" ) {
        tag::voxel_grid::world_coord coord_w;
        REQUIRE( coord_w.x == 0 );
        REQUIRE( coord_w.y == 0 );
        REQUIRE( coord_w.z == 0 );

        WHEN( "converted to chunk coords" ) {
            auto chunk_pair = tag::voxel_grid::from_world_coord(coord_w);

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
        int size = tag::voxel_grid::chunk_size - 1;
        tag::voxel_grid::world_coord coord_w(size, size, size);
        REQUIRE( coord_w.x == size );
        REQUIRE( coord_w.y == size );
        REQUIRE( coord_w.z == size );

        WHEN( "converted to chunk coords" ) {
            auto chunk_pair = tag::voxel_grid::from_world_coord(coord_w);

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
        int size = tag::voxel_grid::chunk_size;
        tag::voxel_grid::world_coord coord_w(size, size, size);
        REQUIRE( coord_w.x == size );
        REQUIRE( coord_w.y == size );
        REQUIRE( coord_w.z == size );

        WHEN( "converted to chunk_coords" ) {
            auto chunk_pair = tag::voxel_grid::from_world_coord(coord_w);

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
        tag::voxel_grid::chunk_coord coord_c;
        REQUIRE( coord_c.x == 0 );
        REQUIRE( coord_c.y == 0 );
        REQUIRE( coord_c.z == 0 );

        WHEN( "converted to a world_coord" ) {
            auto coord_w = tag::voxel_grid::to_world_coord(coord_c);

            THEN( "world_coord is correct" ) {
                REQUIRE( coord_w.x == 0 );
                REQUIRE( coord_w.y == 0 );
                REQUIRE( coord_w.z == 0 );
            }
        }
    }

    GIVEN( "negative chunk_coord" ) {
        tag::voxel_grid::chunk_coord coord_c(-1,-1,-1);
        REQUIRE( coord_c.x == -1 );
        REQUIRE( coord_c.y == -1 );
        REQUIRE( coord_c.z == -1 );

        WHEN( "converted to a world_coord" ) {
            auto coord_w = tag::voxel_grid::to_world_coord(coord_c);

            THEN( "world_coord is correct" ) {
                int size = -tag::voxel_grid::chunk_size;
                REQUIRE( coord_w.x == size );
                REQUIRE( coord_w.y == size );
                REQUIRE( coord_w.z == size );
            }
        }
    }

    GIVEN( "very negative chunk_coord" ) {
        tag::voxel_grid::chunk_coord coord_c(-100,-100,-100);
        REQUIRE( coord_c.x == -100 );
        REQUIRE( coord_c.y == -100 );
        REQUIRE( coord_c.z == -100 );

        WHEN( "converted to a world_coord" ) {
            auto coord_w = tag::voxel_grid::to_world_coord(coord_c);

            THEN( "world_coord is correct" ) {
                int size = -tag::voxel_grid::chunk_size * 100;
                REQUIRE( coord_w.x == size );
                REQUIRE( coord_w.y == size );
                REQUIRE( coord_w.z == size );
            }
        }
    }

    GIVEN( "default chunk_coord and intra_chunk_coord" ) {
        tag::voxel_grid::chunk_coord coord_c;
        tag::voxel_grid::intra_chunk_coord coord_i;
        REQUIRE( coord_c.x == 0 );
        REQUIRE( coord_c.y == 0 );
        REQUIRE( coord_c.z == 0 );
        REQUIRE( coord_i.x == 0 );
        REQUIRE( coord_i.y == 0 );
        REQUIRE( coord_i.z == 0 );

        WHEN( "converted to a world_coord" ) {
            auto coord_w = tag::voxel_grid::to_world_coord(coord_c);

            THEN( "world_coord is correct" ) {
                REQUIRE( coord_w.x == 0 );
                REQUIRE( coord_w.y == 0 );
                REQUIRE( coord_w.z == 0 );
            }
        }
    }

    GIVEN( "negative chunk_coord and intra_chunk_coord" ) {
        tag::voxel_grid::chunk_coord coord_c(-1,-1,-1);
        REQUIRE( coord_c.x == -1 );
        REQUIRE( coord_c.y == -1 );
        REQUIRE( coord_c.z == -1 );

        WHEN( "converted to a world_coord" ) {
            auto coord_w = tag::voxel_grid::to_world_coord(coord_c);

            THEN( "world_coord is correct" ) {
                int size = -tag::voxel_grid::chunk_size;
                REQUIRE( coord_w.x == size );
                REQUIRE( coord_w.y == size );
                REQUIRE( coord_w.z == size );
            }
        }
    }
}

SCENARIO( "coords compare correctly", "[coord]" ) {
    GIVEN( "two equal coords" ){
        tag::voxel_grid::chunk_coord a(1, 1, 1);
        tag::voxel_grid::chunk_coord b(1, 1, 1);

        WHEN( "testing are equal" ) {
            THEN( "result is correct" ) {
                auto test_result = a == b;
                REQUIRE(test_result);
            }
        }

        WHEN( "testing are not equal" ) {
            THEN( "result is correct" ) {
                auto test_result = !(a != b);
                REQUIRE(test_result);
            }
        }

        WHEN( "testing a less than b" ) {
            THEN( "result is correct" ) {
                auto test_result = !(a < b);
                REQUIRE(test_result);
            }
        }

        WHEN( "testing a less than or equal b" ) {
            THEN( "result is correct" ) {
                auto test_result = a <= b;
                REQUIRE(test_result);
            }
        }

        WHEN( "testing b less than a" ) {
            THEN( "result is correct" ) {
                auto test_result = !(b < a);
                REQUIRE(test_result);
            }
        }

        WHEN( "testing b less than or equal a" ) {
            THEN( "result is correct" ) {
                auto test_result = b <= a;
                REQUIRE(test_result);
            }
        }
    }

    GIVEN( "two unequal coords" ){
        tag::voxel_grid::chunk_coord a(0, 0, 0);
        tag::voxel_grid::chunk_coord b(1, 1, 1);

        WHEN( "testing are equal" ) {
            THEN( "result is correct" ) {
                auto test_result = !(a == b);
                REQUIRE(test_result);
            }
        }

        WHEN( "testing are not equal" ) {
            THEN( "result is correct" ) {
                auto test_result = a != b;
                REQUIRE(test_result);
            }
        }

        WHEN( "testing a less than b" ) {
            THEN( "result is correct" ) {
                auto test_result = a < b;
                REQUIRE(test_result);
            }
        }

        WHEN( "testing a less than or equal b" ) {
            THEN( "result is correct" ) {
                auto test_result = a <= b;
                REQUIRE(test_result);
            }
        }

        WHEN( "testing b less than a" ) {
            THEN( "result is correct" ) {
                auto test_result = !(b < a);
                REQUIRE(test_result);
            }
        }

        WHEN( "testing b less than or equal a" ) {
            THEN( "result is correct" ) {
                auto test_result = !(b <= a);
                REQUIRE(test_result);
            }
        }
    }

    GIVEN( "two almost equal coords" ){
        tag::voxel_grid::chunk_coord a(0, 1, 1);
        tag::voxel_grid::chunk_coord b(1, 1, 1);

        WHEN( "testing are equal" ) {
            THEN( "result is correct" ) {
                auto test_result = !(a == b);
                REQUIRE(test_result);
            }
        }

        WHEN( "testing are not equal" ) {
            THEN( "result is correct" ) {
                auto test_result = a != b;
                REQUIRE(test_result);
            }
        }

        WHEN( "testing a less than b" ) {
            THEN( "result is correct" ) {
                auto test_result = a < b;
                REQUIRE(test_result);
            }
        }

        WHEN( "testing a less than or equal b" ) {
            THEN( "result is correct" ) {
                auto test_result = a <= b;
                REQUIRE(test_result);
            }
        }

        WHEN( "testing b less than a" ) {
            THEN( "result is correct" ) {
                auto test_result = !(b < a);
                REQUIRE(test_result);
            }
        }

        WHEN( "testing b less than or equal a" ) {
            THEN( "result is correct" ) {
                auto test_result = !(b <= a);
                REQUIRE(test_result);
            }
        }
    }
}

SCENARIO( "block_instance meta data", "[block_instance]") {
    GIVEN( "block_instance with no meta data" ){
        tag::block_instance block_a(0,0,0,0);

        WHEN( "basic creation" ) {
            THEN( "copied correctly" ){
                auto block_b = block_a;

                auto test_result = block_a.type_id == block_b.type_id
                                && block_a.val_a == block_b.val_a
                                && block_a.val_b == block_b.val_b
                                && block_a.val_c == block_b.val_c;
                REQUIRE(test_result);
            }
        }

        WHEN( "assinging/reading meta data" ){
            THEN( "assigned correctly" ){
                block_a.set_meta_value("test", "passed");

                REQUIRE(block_a.get_meta_value("test") == "passed");
            }
        }

        WHEN( "erasing meta data" ){
            THEN( "assigned correctly" ){
                block_a.set_meta_value("test", "passed");
                block_a.erase_meta_value("test");

                REQUIRE(block_a.get_meta_value("test") == "");
            }
        }

        WHEN( "getting data that was never set" ){
            THEN( "assigned correctly" ){
                REQUIRE(block_a.get_meta_value("test") == "");
            }
        }

        WHEN( "move assinging block_instance with meta data") {
            block_a.set_meta_value("test", "passed");
            auto block_b = std::move(block_a);

            THEN( "assigned correctly" ){
                REQUIRE(block_b.get_meta_value("test") == "passed");
            }
        }

        WHEN( "move constructing block_instance with meta data") {
            block_a.set_meta_value("test", "passed");
            auto block_b(std::move(block_a));

            THEN( "assigned correctly" ){
                REQUIRE(block_b.get_meta_value("test") == "passed");
            }
        }

        WHEN( "copy assinging block_instance with meta data") {
            block_a.set_meta_value("test", "passed");
            auto block_b = block_a;

            THEN( "assigned correctly" ){
                REQUIRE(block_a.get_meta_value("test") == "passed");
                REQUIRE(block_b.get_meta_value("test") == "passed");
            }
        }

        WHEN( "copy constructing block_instance with meta data") {
            block_a.set_meta_value("test", "passed");
            auto block_b(block_a);

            THEN( "assigned correctly" ){
                REQUIRE(block_a.get_meta_value("test") == "passed");
                REQUIRE(block_b.get_meta_value("test") == "passed");
            }
        }

        WHEN( "copied block_instance") {
            block_a.set_meta_value("test", "passed");
            auto block_b(block_a);            
            block_a.set_meta_value("test", "still passed");

            THEN( "copy is decoupled" ){
                REQUIRE(block_a.get_meta_value("test") == "still passed");
                REQUIRE(block_b.get_meta_value("test") == "passed");
            }
        }
    }
}