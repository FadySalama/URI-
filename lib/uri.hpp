#pragma once
#include <iostream>
#include <map>
#include <ostream>
#include <regex>
#include <string>

namespace uripp {

    enum class absolute_uri_components {
        scheme,
        authority,
        path,
        query,
        fragment
    };

    enum class relative_uri_components {
        path,
        query,
        fragment
    };

    enum class authority_components {
        username,
        password,
        host,
        port
    };

    inline std::string ToString(absolute_uri_components comp){
        std::string strings[] = {"scheme", "authority", "path", "query", "fragment"};
        return strings[int(comp)];
    }

    inline std::string ToString(relative_uri_components comp){
        std::string strings[] = {"path", "query", "fragment"};
        return strings[int(comp)];
    }

    inline std::string ToString(authority_components comp){
        std::string strings[] = {"username", "password", "host", "port"};
        return strings[int(comp)];
    }

    class Uri {
        private:
        std::string href = "";

        std::regex uri_regex = std::regex("^(([^:/?#]+):)?(\\/\\/([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?", std::regex_constants::ECMAScript);

        std::regex authority_regex = std::regex("^(([^@]+)(:([^@]+))@)?(\\[[^\\]]+\\]|[^:\\[\\]]+)(:(\\d+))?$", std::regex_constants::ECMAScript);

        std::regex relative_uri_regex = std::regex("^((?:\\/\\/(?:[A-Za-z0-9\\-._~%!$&'()*+,;=:@]*)(?:\\/(?:[A-Za-z0-9\\-._~%!$&'()*+,;=:@]*))*)|(?:\\/(?:[A-Za-z0-9\\-._~%!$&'()*+,;=:@]+(?:\\/(?:[A-Za-z0-9\\-._~%!$&'()*+,;=:@]*))*))|(?:[A-Za-z0-9\\-._~%!$&'()*+,;=@]+(?:\\/(?:[A-Za-z0-9\\-._~%!$&'()*+,;=:@]*))*))?(\\?[A-Za-z0-9\\-._~%!$&'()*+,;=:@\\/\\?]*)?(\\#[A-Za-z0-9\\-._~%!$&'()*+,;=:@\\/\\?]*)?$");

        std::map<absolute_uri_components, std::string> uri_components_map = {{absolute_uri_components::scheme, ""}, {absolute_uri_components::path, ""}};
        std::map<authority_components, std::string> authority_components_map;
        std::map<relative_uri_components, std::string> relative_uri_components_map;

        bool is_absolute_uri = false;
        bool is_relative_uri = false;

        bool has_authority = false;
        bool has_query = false;
        bool has_fragment = false;

        bool isIPv6Host = false;
        bool has_username = false;
        bool has_password = false;
        bool has_port = false;


        
        public:
        Uri() {

        }

        Uri(std::string href) {
            this->href = href;

            std::smatch uri_groups;
            is_absolute_uri = std::regex_match(href, uri_groups, uri_regex);
            std::string schemeMatch = uri_groups[2].str();

            if(is_absolute_uri && !schemeMatch.empty()) {
                uri_components_map[absolute_uri_components::scheme] = schemeMatch;

                std::string authorityMatch = uri_groups[4].str();
                if (!authorityMatch.empty()) {
                    uri_components_map[absolute_uri_components::authority] = uri_groups[4].str();
                    has_authority = true; 
                }

                std::string pathMatch = uri_groups[5].str();
                if (!pathMatch.empty()) {
                    uri_components_map[absolute_uri_components::path] = pathMatch;
                }

                std::string queryMatch = uri_groups[7].str();
                if (!queryMatch.empty()) {
                    uri_components_map[absolute_uri_components::query] = queryMatch;
                    has_query = true;
                }

                std::string fragmentMatch = uri_groups[9].str();
                if (!fragmentMatch.empty()) {
                    uri_components_map[absolute_uri_components::fragment] = fragmentMatch;
                    has_fragment = true;
                }

                if(has_authority) {
                    std::smatch authority_groups;
                    bool isValidAuthority =  std::regex_match(uri_components_map[absolute_uri_components::authority], authority_groups, authority_regex);

                    if(!isValidAuthority) {
                        std::cout << "The authority component of href is not valid" << std::endl;
                        throw;
                    }

                    std::string userMatch = authority_groups[2].str();
                    if (!userMatch.empty()) {
                        authority_components_map[authority_components::username] = userMatch;
                        has_username = true;
                    }
                    
                    std::string passMatch = authority_groups[4].str();
                    if (!passMatch.empty()) {
                        authority_components_map[authority_components::password] = passMatch;
                        has_password = true;
                    }

                    std::string hostMatch = authority_groups[5].str();
                    authority_components_map[authority_components::host] = hostMatch;
                    if(authority_components_map[authority_components::host][0] == '[') isIPv6Host = true;

                    std::string portMatch = authority_groups[7].str();
                    if (!portMatch.empty()) {
                        authority_components_map[authority_components::port] = portMatch;
                        has_port = true;
                    }
                }
            } else {
                is_absolute_uri = false;
                is_relative_uri = std::regex_match(href, uri_groups, relative_uri_regex);

                if(!is_relative_uri) {
                    std::cout << "The provided href is not a valid absolute or relative URI" << std::endl;
                    throw;
                }   

                std::string pathMatch = uri_groups[1].str();
                if (!pathMatch.empty()) {
                    relative_uri_components_map[relative_uri_components::path] = pathMatch;
                }

                std::string queryMatch = uri_groups[2].str();
                if (!queryMatch.empty()) {
                    relative_uri_components_map[relative_uri_components::query] = queryMatch;
                    has_query = true;
                }

                std::string fragmentMatch = uri_groups[3].str();
                if (!fragmentMatch.empty()) {
                    relative_uri_components_map[relative_uri_components::fragment] = fragmentMatch;
                    has_fragment = true;
                }
            }
        }

        const std::string getHref() {
            return href;
        }

        const std::string getScheme() {
            return uri_components_map[absolute_uri_components::scheme];
        }

        const std::string getAuthority() {
            if(has_authority) 
                return uri_components_map[absolute_uri_components::authority];
            else 
                return nullptr;
        }

        const std::string getPath() {
            return uri_components_map[absolute_uri_components::path];
        }

        const std::string getQuery() {
            if(has_query) 
                return uri_components_map[absolute_uri_components::query];
            else 
                return nullptr;
        }

        const std::string getFragment() {
            if(has_fragment) 
                return uri_components_map[absolute_uri_components::fragment];
            else 
                return nullptr;
        }

        std::map<absolute_uri_components, std::string>* getURIComponents() {
            if(is_absolute_uri) {
                return &uri_components_map;
            }
            else {
                return nullptr;
            }
        }

        std::map<relative_uri_components, std::string>* getRelativeURIComponents() {
            if(is_relative_uri) {
                return &relative_uri_components_map;
            }
            else {
                return nullptr;
            }
        }

        const std::string getUsername() {
            if(has_username) 
                return authority_components_map[authority_components::username];
            else 
                return nullptr;
        }

        const std::string getPassword() {
            if(has_password) 
                return authority_components_map[authority_components::password];
            else 
                return nullptr;
        }

        const std::string getHost() {
            if(has_authority) 
                return authority_components_map[authority_components::host];
            else 
                return nullptr;
        }

        const std::string getPort() {
            if(has_port) 
                return authority_components_map[authority_components::port];
            else 
                return nullptr;
        }

        std::map<authority_components, std::string>* getAuthorityComponents() {
            if(is_absolute_uri) {
                return &authority_components_map;
            }
            else {
                return nullptr;
            }    
        }

        bool hasAuthority() {
            return has_authority;
        }

        bool hasQuery() {
            return has_query;
        }

        bool hasFragment() {
            return has_fragment;
        }

        bool hasUsername() {
            return has_username;
        }

        bool hasPassword() {
            return has_password;
        }

        bool hasPort() {
            return has_port;
        }

        bool isRelativeUri() {
            return is_relative_uri;
        }
    };
}
