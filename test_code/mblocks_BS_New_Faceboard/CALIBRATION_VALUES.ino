#define how_many_cubes 32
//                                           NOT A CUBE , white1    , White2    ||S.F.Thing   White
//                                                                                    ,"3" + red_frame
//                                           [0]||||||    , [1]         ,[2]          ,[3]            ,[4]        ,[5]        ,[6]        ,[7]        ,[8]        ,[9]        ,[10]       ,[11]       ,[12]       ,[13]       ,[14]       ,[15]       ,[16]       ,[17]       ,[18]       ,[19]       ,[20]       ,[21]       ,[22]       ,[23]       ,[24]       ,[25]       ,[26]       ,[27]       ,[28]       ,[29]       ,[30]       [31]        |
int ID_numbers[how_many_cubes] =             {0x01234     , 0xEA575     , 0xEA6B2     , 0x8AA7CF      , 0xEA4DD   , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  };                           
int calibrations_forward[how_many_cubes]   = {100         , 100         , 100         , 100           , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       };
int calibrations_reverse[how_many_cubes]   = {100         , 100         , 100         , 100           , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       };
///// Special_offset adds time to brake in ms//////       ,             ,             ,               ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           };
int calibrations_special[how_many_cubes]   = {0           , 0           , 0           , 0             , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         };
int face_version________[how_many_cubes]   = {0           , 0           , 0           , 0             , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         };
int MAGNET_ROT_AXIS_45[how_many_cubes]     = {0           , 0           , 0           , 82            , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         };
int MAGNET_ROT_AXIS_02[how_many_cubes]     = {0           , 0           , 0           , 321           , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         };
int MAGNET_ROT_AXIS_13[how_many_cubes]     = {0           , 0           , 0           , 200           , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         };
int calibrations_plane__[how_many_cubes]   = {0           , 0           , 0           , 0             , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         }; 

//int MAGNET_
//int MAGNET_ROT_AXIS_DIRE[how_many_cubes]   = {0           , 0           , 0           , 1             , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         };
// 1 + means the value DECREASES as we go from plane 1234 to plane 

int forward_offset = 100; // These are the defaults if there isn't an entry for the specific Cube's  in the table.
int reverse_offset = 100;// These are the defaults if there isn't an entry for the specific Cube's  in the table.
int special_offset = 0;// These are the defaults if there isn't an entry for the specific Cube's  in the table.
int plane_change_offset = 0;// These are the defaults if there isn't an entry for the specific Cube's  in the table.