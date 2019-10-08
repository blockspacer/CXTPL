
cxtpl_output
 += 
R"raw(﻿)raw"
 ; 


// NOTE: if output contains symbol like п»ї
// at the beginning - it is UTF file byte mask

// parameters:
//   std::string generator_path;
//   std::vector<std::string> generator_includes;

/* no newline */ 

cxtpl_output
 += 
R"raw(// This is generated file. Do not modify directly.
// Path to the code generator: )raw"
 ; 

cxtpl_output
 +=  generator_path  ; 

cxtpl_output
 += 
R"raw(.

)raw"
 ; 
 for(const auto& fileName: generator_includes) {

cxtpl_output
 +=  fileName  ; 

cxtpl_output
 += 
R"raw(
)raw"
 ; 
 } // end for
cxtpl_output
 += 
R"raw(
#include <iostream>
#include <cstring>
#include <type_traits>

namespace cxxctp {
namespace generated {

// some code here...

)raw"
 ; 

 int fileNameIndex = 0;
 for(const auto& fileName: generator_includes) {
cxtpl_output
 += 
R"raw(int some)raw"
 ; 

cxtpl_output
 += 
 std::to_string(  fileNameIndex  ) 
 ; 

cxtpl_output
 += 
R"raw( = 3;
)raw"
 ; 
   ++fileNameIndex;
 }
cxtpl_output
 += 
R"raw(int someJ = 5;
)raw"
 ; 

cxtpl_output
 += 
R"raw(

// some code here...

} // namespace cxxctp
} // namespace generated
)raw"
 ; 
