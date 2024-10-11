![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Static Badge](https://img.shields.io/badge/Compiler-C%2B%2B11-blue?style=for-the-badge)

# URI++

URI++ is a header-only C++ library that is for parsing and building both absolute and relative URIs and handling them programatically, as there is a general lack of URI parsers and builders in C++ that support relative URIs. The parsing is handled using Regular Expressions.

# Installation

Just copy over the `uri.pp` in your `include` directory and you are good to go!

# Example

```cpp
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

   uripp::Uri* p_uri = uripp::UriBuilder::build(config);
   uripp::Uri uri = *p_uri;

   auto absolute_parts = !p_uri->isRelativeUri() ? p_uri->getURIComponents() : nullptr;
   auto relative_parts = p_uri->isRelativeUri() ? p_uri->getRelativeURIComponents() : nullptr;
   auto authorityParts = !p_uri->isRelativeUri() && p_uri->hasAuthority() ? p_uri->getAuthorityComponents() : nullptr;

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

   
   std::cout << std::endl << uri;

   return 0;
}

```

# Documentation

For the full documentation, please visit the [documentation page](https://fadysalama.github.io/URI_PP/)
