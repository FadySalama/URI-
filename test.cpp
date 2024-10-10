#include "lib/uri.hpp"
#include <iostream>
#include <map>
#include <ostream>

int main() {
   uripp::Uri uri = uripp::Uri("./hi/hello?query=yes#fragment");
   auto absolute_parts = uri.getURIComponents();
   auto relative_parts = uri.getRelativeURIComponents();
   auto authorityParts = uri.getAuthorityComponents();

   if (absolute_parts != nullptr) {
      for(auto it = absolute_parts->begin(); it != absolute_parts->end(); it++) {
         std::cout << uripp::ToString(it->first) << ", " << it->second << std::endl;
      }
   }
   
   if (relative_parts != nullptr) {
      for(auto it2 = relative_parts->begin(); it2 != relative_parts->end(); it2++) {
         std::cout << uripp::ToString(it2->first) << ", " << it2->second << std::endl;
      }
   }

   if (authorityParts != nullptr) {
      for(auto it3 = authorityParts->begin(); it3 != authorityParts->end(); it3++) {
         std::cout << uripp::ToString(it3->first) << ", " << it3->second << std::endl;
      }
   }

    return 0;
}