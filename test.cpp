#include "include/uri.hpp"
#include <iostream>
#include <map>
#include <ostream>

int main() {
   uripp::UriBuilderConfig config = uripp::UriBuilderConfig();
   config.scheme = "http";
   config.authority = "www.example.com:8080";
   config.query = "name=fady";
   config.path = "path/to/home";
   config.fragment = "fragment";

   uripp::Uri* uri = uripp::UriBuilder::build(config);

   // uripp::Uri uri = uripp::Uri("./hi/hello?query=yes#fragment");
   auto absolute_parts = !uri->isRelativeUri() ? uri->getURIComponents() : nullptr;
   auto relative_parts = uri->isRelativeUri() ? uri->getRelativeURIComponents() : nullptr;
   auto authorityParts = !uri->isRelativeUri() && uri->hasAuthority() ? uri->getAuthorityComponents() : nullptr;

   if (absolute_parts != nullptr) {
      for(auto it = absolute_parts->begin(); it != absolute_parts->end(); it++) {
         std::cout << uripp::Uri::ComponentToString(it->first) << ", " << it->second << std::endl;
      }
   }
   
   if (relative_parts != nullptr) {
      for(auto it2 = relative_parts->begin(); it2 != relative_parts->end(); it2++) {
         std::cout << uripp::Uri::ComponentToString(it2->first) << ", " << it2->second << std::endl;
      }
   }

   if (authorityParts != nullptr) {
      for(auto it3 = authorityParts->begin(); it3 != authorityParts->end(); it3++) {
         std::cout << uripp::Uri::ComponentToString(it3->first) << ", " << it3->second << std::endl;
      }
   }

    return 0;
}